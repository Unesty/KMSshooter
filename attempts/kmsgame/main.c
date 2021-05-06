/*
 * Copyright (c) 2012 Arvin Schnell <arvin.schnell@gmail.com>
 * Copyright (c) 2012 Rob Clark <rob@ti.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* Based on a egl cube test app originally written by Arvin Schnell */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <assert.h>

#include "esUtil.h"
#include "esTransform.c"
#define PI 3.1415926535897932384626433832795f
#include <math.h>

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>


#define DYNAMIC_KEYS
#include <linux/input-event-codes.h>

//#define SOUND
#include <alsa/asoundlib.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

char mouse[3];
char moufd;
char mapfd;
char *keyboard;
char kfd;
char kapfd;
//float cur[2];
float *cur;
//float position[3];

char cardpath[256];

static struct {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
	EGLSurface surface;
	GLuint program;
	GLint modelviewmatrix, modelviewprojectionmatrix, projectionmatrix, /*normalmatrix,*/ posoffvector/*, mouse*/;
	GLuint vbo;
	GLuint positionsoffset, colorsoffset, normalsoffset;
} gl;

static struct {
	struct gbm_device *dev;
	struct gbm_surface *surface;
} gbm;

static struct {
	int fd;
	drmModeModeInfo *mode;
	uint32_t crtc_id;
	uint32_t connector_id;
} drm;

struct drm_fb {
	struct gbm_bo *bo;
	uint32_t fb_id;
};

static uint32_t find_crtc_for_encoder(const drmModeRes *resources,
				      const drmModeEncoder *encoder) {
	int i;

	for (i = 0; i < resources->count_crtcs; i++) {
		/* possible_crtcs is a bitmask as described here:
		 * https://dvdhrm.wordpress.com/2012/09/13/linux-drm-mode-setting-api
		 */
		const uint32_t crtc_mask = 1 << i;
		const uint32_t crtc_id = resources->crtcs[i];
		if (encoder->possible_crtcs & crtc_mask) {
			return crtc_id;
		}
	}

	/* no match found */
	return -1;
}

static uint32_t find_crtc_for_connector(const drmModeRes *resources,
					const drmModeConnector *connector) {
	int i;

	for (i = 0; i < connector->count_encoders; i++) {
		const uint32_t encoder_id = connector->encoders[i];
		drmModeEncoder *encoder = drmModeGetEncoder(drm.fd, encoder_id);

		if (encoder) {
			const uint32_t crtc_id = find_crtc_for_encoder(resources, encoder);

			drmModeFreeEncoder(encoder);
			if (crtc_id != 0) {
				return crtc_id;
			}
		}
	}

	/* no match found */
	return -1;
}

static int init_drm(void)
{
	drmModeRes *resources;
	drmModeConnector *connector = NULL;
	drmModeEncoder *encoder = NULL;
	int i, area;

	drm.fd = open(cardpath, O_RDWR);

	if (drm.fd < 0) {
		printf("could not open drm device\n");
		return -1;
	}

	resources = drmModeGetResources(drm.fd);
	if (!resources) {
		printf("drmModeGetResources failed: %s\n", strerror(errno));
		return -1;
	}

	/* find a connected connector: */
	for (i = 0; i < resources->count_connectors; i++) {
		connector = drmModeGetConnector(drm.fd, resources->connectors[i]);
		if (connector->connection == DRM_MODE_CONNECTED) {
			/* it's connected, let's use this! */
			break;
		}
		drmModeFreeConnector(connector);
		connector = NULL;
	}

	if (!connector) {
		/* we could be fancy and listen for hotplug events and wait for
		 * a connector..
		 */
		printf("no connected connector!\n");
		return -1;
	}

	/* find prefered mode or the highest resolution mode: */
	for (i = 0, area = 0; i < connector->count_modes; i++) {
		drmModeModeInfo *current_mode = &connector->modes[i];

		if (current_mode->type & DRM_MODE_TYPE_PREFERRED) {
			drm.mode = current_mode;
		}

		int current_area = current_mode->hdisplay * current_mode->vdisplay;
		if (current_area > area) {
			drm.mode = current_mode;
			area = current_area;
		}
	}

	if (!drm.mode) {
		printf("could not find mode!\n");
		return -1;
	}

	/* find encoder: */
	for (i = 0; i < resources->count_encoders; i++) {
		encoder = drmModeGetEncoder(drm.fd, resources->encoders[i]);
		if (encoder->encoder_id == connector->encoder_id)
			break;
		drmModeFreeEncoder(encoder);
		encoder = NULL;
	}

	if (encoder) {
		drm.crtc_id = encoder->crtc_id;
	} else {
		uint32_t crtc_id = find_crtc_for_connector(resources, connector);
		if (crtc_id == 0) {
			printf("no crtc found!\n");
			return -1;
		}

		drm.crtc_id = crtc_id;
	}

	drm.connector_id = connector->connector_id;

	return 0;
}

static int init_gbm(void)
{
	gbm.dev = gbm_create_device(drm.fd);

	gbm.surface = gbm_surface_create(gbm.dev,
			drm.mode->hdisplay, drm.mode->vdisplay,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
	if (!gbm.surface) {
		printf("failed to create gbm surface\n");
		return -1;
	}

	return 0;
}

int vnum=14*3*4+9;
//static const char *vertex_shader_source;
//static const char *fragment_shader_source;
//char mvfd, mffd;
static GLfloat vVertices[] = {
-1.0f, 1.0f, 1.0f,	// Front-top-left
-1.0f, -1.0f, 1.0f,	// Front-top-right
1.0f, 1.0f, 1.0f,	// Front-bottom-left
1.0f, -1.0f, 1.0f,	// Front-bottom-right
1.0f, -1.0f, -1.0f,	// Back-bottom-right
-1.0f, -1.0f, 1.0f,	// Front-top-right
-1.0f, -1.0f, -1.0f,	// Back-top-right
-1.0f, 1.0f, 1.0f,	// Front-top-left
-1.0f, 1.0f, -1.0f,	// Back-top-left
1.0f, 1.0f, 1.0f,	// Front-bottom-left
1.0f, 1.0f, -1.0f,	// Back-bottom-left
1.0f, -1.0f, -1.0f,	// Back-bottom-right
-1.0f, 1.0f, -1.0f,	// Back-top-left
-1.0f, -1.0f, -1.0f,	// Back-top-right
-1.0f, -1.0f, -1.0f,

25-1.0f, 25+1.0f, 25+1.0f,
25-1.0f, 25+1.0f, 25+1.0f,
25-1.0f, 25+1.0f, 25+1.0f,	// Front-top-left
25-1.0f, 25-1.0f, 25+1.0f,	// Front-top-right
25+1.0f, 25+1.0f, 25+1.0f,	// Front-bottom-left
25+1.0f, 25-1.0f, 25+1.0f,	// Front-bottom-right
25+1.0f, 25-1.0f, 25-1.0f,	// Back-bottom-right
25-1.0f, 25-1.0f, 25+1.0f,	// Front-top-right
25-1.0f, 25-1.0f, 25-1.0f,	// Back-top-right
25-1.0f, 25+1.0f, 25+1.0f,	// Front-top-left
25-1.0f, 25+1.0f, 25-1.0f,	// Back-top-left
25+1.0f, 25+1.0f, 25+1.0f,	// Front-bottom-left
25+1.0f, 25+1.0f, 25-1.0f,	// Back-bottom-left
25+1.0f, 25-1.0f, 25-1.0f,	// Back-bottom-right
25-1.0f, 25+1.0f, 25-1.0f,	// Back-top-left
25-1.0f, 25-1.0f, 25-1.0f,	// Back-top-right
25-1.0f, 25-1.0f, 25-1.0f,

30-1.0f, 30+1.0f, 5+1.0f,
30-1.0f, 30+1.0f, 5+1.0f,
30-1.0f, 30+1.0f, 5+1.0f,	// Front-top-left
30-1.0f, 30-1.0f, 5+1.0f,	// Front-top-right
30+1.0f, 30+1.0f, 5+1.0f,	// Front-bottom-left
30+1.0f, 30-1.0f, 5+1.0f,	// Front-bottom-right
30+1.0f, 30-1.0f, 5-1.0f,	// Back-bottom-right
30-1.0f, 30-1.0f, 5+1.0f,	// Front-top-right
30-1.0f, 30-1.0f, 5-1.0f,	// Back-top-right
30-1.0f, 30+1.0f, 5+1.0f,	// Front-top-left
30-1.0f, 30+1.0f, 5-1.0f,	// Back-top-left
30+1.0f, 30+1.0f, 5+1.0f,	// Front-bottom-left
30+1.0f, 30+1.0f, 5-1.0f,	// Back-bottom-left
30+1.0f, 30-1.0f, 5-1.0f,	// Back-bottom-right
30-1.0f, 30+1.0f, 5-1.0f,	// Back-top-left
30-1.0f, 30-1.0f, 5-1.0f,	// Back-top-right
30-1.0f, 30-1.0f, 5-1.0f,

-60.0f, 60.0f, 60.0f,
-60.0f, 60.0f, 60.0f,
-60.0f, 60.0f, 60.0f,	// Front-top-right
-60.0f, -60.0f, 60.0f,	// Front-top-left
60.0f, 60.0f, 60.0f,	// Front-bottom-left
60.0f, -60.0f, 60.0f,	// Front-bottom-right
60.0f, -60.0f, -60.0f,	// Back-bottom-right
-60.0f, -60.0f, 60.0f,	// Front-top-right
-60.0f, -60.0f, -60.0f,	// Back-top-right
-60.0f, 60.0f, 60.0f,	// Front-top-left
-60.0f, 60.0f, -60.0f,	// Back-top-left
60.0f, 60.0f, 60.0f,	// Front-bottom-left
60.0f, 60.0f, -60.0f,	// Back-bottom-left
60.0f, -60.0f, -60.0f,	// Back-bottom-right
-60.0f, 60.0f, -60.0f,	// Back-top-left
-60.0f, -60.0f, -60.0f,	// Back-top-right
-20.0f, -20.0f, -20.0f,	// Back-top-right
-10.0f, -20.0f, -20.0f,	// Back-top-right
/*
		// front
		-1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		// back
		+1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, -1.0f,
		// right
		+1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f,
		// left
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, +1.0f,
		// top
		-1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, -1.0f,
		// bottom
		-1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f,
		*/
};
uint vertex_shader, fragment_shader;
static int init_gl(void)
{
	int major, minor, n;
	int ret;


	static const GLfloat vColors[] = {
			// front
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f, // magenta
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			0.0f,  1.0f,  0.0f, // green
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  0.0f,  0.0f, // red
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  0.0f, // green
			0.0f,  1.0f,  1.0f, // cyan
			0.0f,  0.0f,  1.0f, // blue
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  0.0f, // green
			1.0f,  1.0f,  0.0f, // yellow
			1.0f,  1.0f,  1.0f, // white
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  0.0f, // yellow
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			1.0f,  0.0f,  1.0f, // magenta
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  1.0f, // white
			1.0f,  0.0f,  0.0f, // red
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  0.0f, // green
			0.0f,  1.0f,  1.0f, // cyan
			0.0f,  0.0f,  1.0f, // blue
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  0.0f, // green
			1.0f,  1.0f,  0.0f, // yellow
			1.0f,  1.0f,  1.0f, // white
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  0.0f, // yellow
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			1.0f,  0.0f,  1.0f, // magenta
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  1.0f, // white
			1.0f,  0.0f,  0.0f, // red
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  1.0f, // white
	};

	static const GLfloat vNormals[] = {
			// front
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+1.0f, +0.0f, +1.0f, // forward
			+1.0f, -1.0f, +1.0f, // forward
			+1.0f, -1.0f, 0.0f, // right
			-1.0f, -1.0f, 0.0f, // down
			-1.0f, -1.0f, +0.0f, // down
			-1.0f, +1.0f, +0.0f, // left
			-1.0f, +1.0f, +0.0f, // left
			+1.0f, +1.0f, +0.0f, // up
			+1.0f, +1.0f, -1.0f, // up
			+1.0f, +0.0f, -1.0f, // right
			// back
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			// front
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+1.0f, +0.0f, +1.0f, // forward
			+1.0f, -1.0f, +1.0f, // forward
			+1.0f, -1.0f, 0.0f, // right
			-1.0f, -1.0f, 0.0f, // down
			-1.0f, -1.0f, +0.0f, // down
			-1.0f, +1.0f, +0.0f, // left
			-1.0f, +1.0f, +0.0f, // left
			+1.0f, +1.0f, +0.0f, // up
			+1.0f, +1.0f, -1.0f, // up
			+1.0f, +0.0f, -1.0f, // right
			// back
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			// front
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+1.0f, +0.0f, +1.0f, // forward
			+1.0f, -1.0f, +1.0f, // forward
			+1.0f, -1.0f, 0.0f, // right
			-1.0f, -1.0f, 0.0f, // down
			-1.0f, -1.0f, +0.0f, // down
			-1.0f, +1.0f, +0.0f, // left
			-1.0f, +1.0f, +0.0f, // left
			+1.0f, +1.0f, +0.0f, // up
			+1.0f, +1.0f, -1.0f, // up
			+1.0f, +0.0f, -1.0f, // right
			// back
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			// front
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+1.0f, +0.0f, +1.0f, // forward
			+1.0f, -1.0f, +1.0f, // forward
			+1.0f, -1.0f, 0.0f, // right
			-1.0f, -1.0f, 0.0f, // down
			-1.0f, -1.0f, +0.0f, // down
			-1.0f, +1.0f, +0.0f, // left
			-1.0f, +1.0f, +0.0f, // left
			+1.0f, +1.0f, +0.0f, // up
			+1.0f, +1.0f, -1.0f, // up
			+1.0f, +0.0f, -1.0f, // right
			// back
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			// front
			+0.0f, +0.0f, +1.0f, // forward
			+0.0f, +0.0f, +1.0f, // forward
			+1.0f, +0.0f, +1.0f, // forward
			+1.0f, -1.0f, +1.0f, // forward
			+1.0f, -1.0f, 0.0f, // right
			-1.0f, -1.0f, 0.0f, // down
			-1.0f, -1.0f, +0.0f, // down
			-1.0f, +1.0f, +0.0f, // left
			-1.0f, +1.0f, +0.0f, // left
			+1.0f, +1.0f, +0.0f, // up
			+1.0f, +1.0f, -1.0f, // up
			+1.0f, +0.0f, -1.0f, // right
			// back
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			+0.0f, +0.0f, -1.0f, // backward
			// right
			+1.0f, +0.0f, +0.0f, // right
			+1.0f, +0.0f, +0.0f, // right
			// left
			-1.0f, +0.0f, +0.0f, // left
			-1.0f, +0.0f, +0.0f, // left
			// top
			+0.0f, +1.0f, +0.0f, // up
			+0.0f, +1.0f, +0.0f, // up
			// bottom
			+0.0f, -1.0f, +0.0f, // down
			+0.0f, -1.0f, +0.0f  // down
	};

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	static const EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};


	static const char *vertex_shader_source =
			"precision mediump float;           \n"
			"uniform mat4 modelviewMatrix;      \n"
			"uniform vec4 posoffvector;         \n"
			//"uniform vec2 mouse;         \n"
			//"uniform mat4 modelviewprojectionMatrix;\n"
			"uniform mat4 projectionMatrix;     \n"
			//"uniform mat3 normalMatrix;         \n"
			"                                   \n"
			"attribute vec4 in_position;        \n"
			"attribute vec3 in_normal;          \n"
			"attribute vec4 in_color;           \n"
			"vec4 lightSource = vec4(2.0, 2.0, 20.0, 0.0);\n"
			"                                   \n"
			"varying vec4 vVaryingColor;        \n"
			"                                   \n"
			"void main()                        \n"
			"{                                  \n"
			"    gl_Position = (projectionMatrix * modelviewMatrix) * (in_position-posoffvector);\n"
			//"    gl_Position = posoffvector + (projectionMatrix * modelviewMatrix) * (in_position-posoffvector);\n"
			//"    gl_Position = in_position * modelviewprojectionMatrix;\n"
			//"    vec3 vEyeNormal = normalMatrix * in_normal;\n"
			"    vec3 vEyeNormal = in_normal;\n"
			"    vec4 vPosition4 = modelviewMatrix * in_position;\n"
			"    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;\n"
			"    vec3 vLightDir = normalize(lightSource.xyz - vPosition3);\n"
			"    float diff = max(0.0, dot(vEyeNormal, vLightDir));\n"
			"    vVaryingColor = vec4(diff * in_color.rgb, 1.0);\n"
			"}                                  \n";



	static const char *fragment_shader_source =
			"precision mediump float;           \n"
			"                                   \n"
			//"varying vec4 vVaryingColor;        \n"
			//"uniform vec4 posoffvector;      \n"
			//"uniform vec2 mouse;      \n"
			"                                   \n"
			"void main()                        \n"
			"{                                  \n"
			//"    gl_FragColor = vVaryingColor;  \n"
			"    gl_FragColor = vec4(vec3(gl_FragCoord.z),1.0);  \n"
			"}                                  \n";


	PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;
	get_platform_display =
		(void *) eglGetProcAddress("eglGetPlatformDisplayEXT");
	assert(get_platform_display != NULL);

	gl.display = get_platform_display(EGL_PLATFORM_GBM_KHR, gbm.dev, NULL);

	if (!eglInitialize(gl.display, &major, &minor)) {
		printf("failed to initialize\n");
		return -1;
	}

	printf("Using display %p with EGL version %d.%d\n",
			gl.display, major, minor);

	printf("EGL Version \"%s\"\n", eglQueryString(gl.display, EGL_VERSION));
	printf("EGL Vendor \"%s\"\n", eglQueryString(gl.display, EGL_VENDOR));
	printf("EGL Extensions \"%s\"\n", eglQueryString(gl.display, EGL_EXTENSIONS));

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		printf("failed to bind api EGL_OPENGL_ES_API\n");
		return -1;
	}

	if (!eglChooseConfig(gl.display, config_attribs, &gl.config, 1, &n) || n != 1) {
		printf("failed to choose config: %d\n", n);
		return -1;
	}

	gl.context = eglCreateContext(gl.display, gl.config,
			EGL_NO_CONTEXT, context_attribs);
	if (gl.context == NULL) {
		printf("failed to create context\n");
		return -1;
	}

	gl.surface = eglCreateWindowSurface(gl.display, gl.config, gbm.surface, NULL);
	if (gl.surface == EGL_NO_SURFACE) {
		printf("failed to create egl surface\n");
		return -1;
	}

	/* connect the context to the surface */
	eglMakeCurrent(gl.display, gl.surface, gl.surface, gl.context);

	printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		printf("vertex shader compilation failed!:\n");
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &ret);
		if (ret > 1) {
			log = malloc(ret);
			glGetShaderInfoLog(vertex_shader, ret, NULL, log);
			printf("%s", log);
		}

		return -1;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		printf("fragment shader compilation failed!:\n");
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetShaderInfoLog(fragment_shader, ret, NULL, log);
			printf("%s", log);
		}

		return -1;
	}

	gl.program = glCreateProgram();

	glAttachShader(gl.program, vertex_shader);
	glAttachShader(gl.program, fragment_shader);

	glBindAttribLocation(gl.program, 0, "in_position");
	glBindAttribLocation(gl.program, 1, "in_normal");
	glBindAttribLocation(gl.program, 2, "in_color");

	glLinkProgram(gl.program);

	glGetProgramiv(gl.program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char *log;

		printf("program linking failed!:\n");
		glGetProgramiv(gl.program, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetProgramInfoLog(gl.program, ret, NULL, log);
			printf("%s", log);
		}

		return -1;
	}

	glUseProgram(gl.program);

	gl.modelviewmatrix = glGetUniformLocation(gl.program, "modelviewMatrix");
	gl.projectionmatrix = glGetUniformLocation(gl.program, "projectionMatrix");
	//gl.modelviewprojectionmatrix = glGetUniformLocation(gl.program, "modelviewprojectionMatrix");
	//gl.normalmatrix = glGetUniformLocation(gl.program, "normalMatrix");
	gl.posoffvector = glGetUniformLocation(gl.program, "posoffvector");
	//gl.mouse = glGetUniformLocation(gl.program, "mouse");

	glViewport(0, 0, drm.mode->hdisplay, drm.mode->vdisplay);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	gl.positionsoffset = 0;
	gl.colorsoffset = sizeof(vVertices);
	gl.normalsoffset = sizeof(vVertices) + sizeof(vColors);
	glGenBuffers(1, &gl.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices) + sizeof(vColors) + sizeof(vNormals), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, gl.positionsoffset, sizeof(vVertices), &vVertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, gl.colorsoffset, sizeof(vColors), &vColors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, gl.normalsoffset, sizeof(vNormals), &vNormals[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.positionsoffset);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.normalsoffset);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.colorsoffset);
	glEnableVertexAttribArray(2);

	return 0;
}


static void
drm_fb_destroy_callback(struct gbm_bo *bo, void *data)
{
	struct drm_fb *fb = data;
	struct gbm_device *gbm = gbm_bo_get_device(bo);

	if (fb->fb_id)
		drmModeRmFB(drm.fd, fb->fb_id);

	free(fb);
}

static struct drm_fb * drm_fb_get_from_bo(struct gbm_bo *bo)
{
	struct drm_fb *fb = gbm_bo_get_user_data(bo);
	uint32_t width, height, stride, handle;
	int ret;

	if (fb)
		return fb;

	fb = calloc(1, sizeof *fb);
	fb->bo = bo;

	width = gbm_bo_get_width(bo);
	height = gbm_bo_get_height(bo);
	stride = gbm_bo_get_stride(bo);
	handle = gbm_bo_get_handle(bo).u32;

	ret = drmModeAddFB(drm.fd, width, height, 24, 32, stride, handle, &fb->fb_id);
	if (ret) {
		printf("failed to create fb: %s\n", strerror(errno));
		free(fb);
		return NULL;
	}

	gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);

	return fb;
}

static void page_flip_handler(int fd, unsigned int frame,
		  unsigned int sec, unsigned int usec, void *data)
{
	int *waiting_for_flip = data;
	*waiting_for_flip = 0;
}

struct config{
	char text[2000];
	char fd;
	int size;
	int op;
}conf;
char mousepath[256];
char keyboardpath[256];
char keyoffset;
char key[20];
//char mvspath[256];
//char mfspath[256];
int ppid,mpid,kpid,spid=0;
#ifdef SOUND
unsigned int rate=48000;
char channels;
#endif

GLfloat aspect,frustumW,frustumH;
float nearcp=20.0f;
float farcp=2000.0f;
float viewsize=4.0f;

float *posoff;
float rposoff[2000][3];
ESMatrix modelview;
ESMatrix projection;
float cubeoff[2]={0,0};

struct segasteon{
	int a;
	int b;
	int c;
}sas={0,0,3};

int main(int argc, char *argv[])
{
	//read config
	conf.fd = open("editme.conf",0);
	if(conf.fd==-1){
		conf.fd = open("/etc/editme.conf",0);
	if(conf.fd==-1){
		syscall(SYS_write,1,"ERROR: no editme.conf\n",21);
		return -1;
	}
	}
	conf.size = read(conf.fd,&conf.text,2000);
	close(conf.fd);
	conf.op=0;
	int csmb;
	for(int symb=0;symb<conf.size;++symb){
		switch(conf.op){
		case 0:
			switch(conf.text[symb]){
			case '/':
				csmb=0;
				mousepath[csmb]=conf.text[symb];
				while(symb<conf.size){
					symb++;
					csmb++;
					if(conf.text[symb]=='\n'){
						conf.op++;
						break;
					}
					mousepath[csmb]=conf.text[symb];
				 }
				break;
			 }
			break;
		case 1:
			switch(conf.text[symb]){
			case '/':
				csmb=0;
				keyboardpath[csmb]=conf.text[symb];
				while(symb<conf.size){
					symb++;
					csmb++;
					if(conf.text[symb]=='\n'){
						conf.op++;
						break;
					}
					keyboardpath[csmb]=conf.text[symb];
				 }
				break;
			}
		case 2:
			if(conf.text[symb]>='0'&&conf.text[symb]<='9'){
				csmb=0;
				while(csmb<3){
				switch(conf.text[symb]){
				case '0':
					switch(csmb){
					case 0:
						csmb++;
						break;
					case 1:
						keyoffset=keyoffset*10;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '1':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=1;
						break;
					case 1:
						keyoffset=keyoffset*10+1;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+1;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '2':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=2;
						break;
					case 1:
						keyoffset=keyoffset*10+2;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+2;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '3':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=3;
						break;
					case 1:
						keyoffset=keyoffset*10+3;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+3;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '4':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=4;
						break;
					case 1:
						keyoffset=keyoffset*10+4;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+4;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '5':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=5;
						break;
					case 1:
						keyoffset=keyoffset*10+5;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+5;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '6':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=6;
						break;
					case 1:
						keyoffset=keyoffset*10+6;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+6;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '7':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=7;
						break;
					case 1:
						keyoffset=keyoffset*10+7;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+7;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '8':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=8;
						break;
					case 1:
						keyoffset=keyoffset*10+8;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+8;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '9':
					switch(csmb){
					case 0:
						csmb++;
						keyoffset=9;
						break;
					case 1:
						keyoffset=keyoffset*10+9;
						csmb++;
						break;
					case 2:
						keyoffset=keyoffset*10+9;
						conf.op++;
						csmb++;
						break;
					}
					break;
				default:
					conf.op++;
					csmb=3;
					break;
				}
				symb++;
				}
			}
		case 3:
			switch(conf.text[symb]){
			case '/':
				csmb=0;
				cardpath[csmb]=conf.text[symb];
				while(symb<conf.size){
					symb++;
					csmb++;
					if(conf.text[symb]=='\n'){
						conf.op++;
						break;
					}
					cardpath[csmb]=conf.text[symb];
				 }
				break;
			}
			break;
		case 4:
			if(conf.text[symb]>='0'&&conf.text[symb]<='9'){
				csmb=0;
				while(csmb<3){
				switch(conf.text[symb]){
				case '0':
					switch(csmb){
					case 0:
						csmb++;
						break;
					case 1:
						nearcp=nearcp*10;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '1':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=1;
						break;
					case 1:
						nearcp=nearcp*10+1;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+1;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '2':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=2;
						break;
					case 1:
						nearcp=nearcp*10+2;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+2;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '3':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=3;
						break;
					case 1:
						nearcp=nearcp*10+3;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+3;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '4':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=4;
						break;
					case 1:
						nearcp=nearcp*10+4;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+4;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '5':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=5;
						break;
					case 1:
						nearcp=nearcp*10+5;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+5;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '6':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=6;
						break;
					case 1:
						nearcp=nearcp*10+6;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+6;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '7':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=7;
						break;
					case 1:
						nearcp=nearcp*10+7;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+7;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '8':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=8;
						break;
					case 1:
						nearcp=nearcp*10+8;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+8;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '9':
					switch(csmb){
					case 0:
						csmb++;
						nearcp=9;
						break;
					case 1:
						nearcp=nearcp*10+9;
						csmb++;
						break;
					case 2:
						nearcp=nearcp*10+9;
						conf.op++;
						csmb++;
						break;
					}
					break;
				default:
					conf.op++;
					csmb=3;
					break;
				}
				symb++;
				}
			}
		case 5:
			if(conf.text[symb]>='0'&&conf.text[symb]<='9'){
				csmb=0;
				while(csmb<3){
				switch(conf.text[symb]){
				case '0':
					switch(csmb){
					case 0:
						csmb++;
						break;
					case 1:
						farcp=farcp*10;
						csmb++;
						break;
					case 2:
						farcp=farcp*10;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '1':
					switch(csmb){
					case 0:
						csmb++;
						farcp=1;
						break;
					case 1:
						farcp=farcp*10+1;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+1;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '2':
					switch(csmb){
					case 0:
						csmb++;
						farcp=2;
						break;
					case 1:
						farcp=farcp*10+2;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+2;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '3':
					switch(csmb){
					case 0:
						csmb++;
						farcp=3;
						break;
					case 1:
						farcp=farcp*10+3;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+3;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '4':
					switch(csmb){
					case 0:
						csmb++;
						farcp=4;
						break;
					case 1:
						farcp=farcp*10+4;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+4;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '5':
					switch(csmb){
					case 0:
						csmb++;
						farcp=5;
						break;
					case 1:
						farcp=farcp*10+5;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+5;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '6':
					switch(csmb){
					case 0:
						csmb++;
						farcp=6;
						break;
					case 1:
						farcp=farcp*10+6;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+6;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '7':
					switch(csmb){
					case 0:
						csmb++;
						farcp=7;
						break;
					case 1:
						farcp=farcp*10+7;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+7;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '8':
					switch(csmb){
					case 0:
						csmb++;
						farcp=8;
						break;
					case 1:
						farcp=farcp*10+8;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+8;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '9':
					switch(csmb){
					case 0:
						csmb++;
						farcp=9;
						break;
					case 1:
						farcp=farcp*10+9;
						csmb++;
						break;
					case 2:
						farcp=farcp*10+9;
						conf.op++;
						csmb++;
						break;
					}
					break;
				default:
					conf.op++;
					csmb=3;
					break;
				}
				symb++;
				}
			}
#ifdef SOUND
		case 6:{
			/*
			csmb=0;
			while(symb<conf.size){
				if(conf.text[symb]=='0')
					csmb++;
				if(conf.text[symb]=='1'){
					rate+=10^csmb;
					csmb++;}
				if(conf.text[symb]=='2'){
					rate+=10^csmb*2;
					csmb++;}
				if(conf.text[symb]=='3'){
					rate+=10^csmb*3;
					csmb++;}
				if(conf.text[symb]=='4'){
					rate+=10^csmb*4;
					csmb++;}
				if(conf.text[symb]=='5'){
					rate+=10^csmb*5;
					csmb++;}
				if(conf.text[symb]=='6'){
					rate+=10^csmb*6;
					csmb++;}
				if(conf.text[symb]=='7'){
					rate+=10^csmb*7;
					csmb++;}
				if(conf.text[symb]=='8'){
					rate+=10^csmb*8;
					csmb++;}
				if(conf.text[symb]=='9'){
					rate+=10^csmb*9;
					csmb++;}
				if(conf.text[symb]=='\n'&&rate){
					conf.op++;
					break;
				}
				symb++;
			}*/
		       }
		case 7:{
				if(conf.text[symb]=='0')
					channels=0;
				if(conf.text[symb]=='1')
					channels=1;
				if(conf.text[symb]=='2')
					channels=2;
				if(conf.text[symb]=='3')
					channels=3;
				if(conf.text[symb]=='4')
					channels=4;
				if(conf.text[symb]=='5')
					channels=5;
				if(conf.text[symb]=='6')
					channels=6;
				if(conf.text[symb]=='7')
					channels=7;
				if(conf.text[symb]=='8')
					channels=8;
				if(conf.text[symb]=='9')
					channels=9;
		       }
#endif
		       /*
		case 8:{
			switch(conf.text[symb]){
			case '/':{
				csmb=0;
				mvspath[csmb]=conf.text[symb];
				while(symb<conf.size){
					symb++;
					csmb++;
					if(conf.text[symb]=='\n'){
						conf.op++;
						break;
					}
					mvspath[csmb]=conf.text[symb];
				 }
				break;
				}
			}
		       }
		case 9:{
			switch(conf.text[symb]){
			case '/':{
				csmb=0;
				mfspath[csmb]=conf.text[symb];
				while(symb<conf.size){
					symb++;
					csmb++;
					if(conf.text[symb]=='\n'){
						conf.op++;
						break;
					}
					mfspath[csmb]=conf.text[symb];
				 }
				break;
				}
			}
		       }
		       */
		}
	}
	/*
	//load shaders
	mvfd=syscall(SYS_open,mvspath,0);
	if(syscall(SYS_read,mvfd,&vertex_shader_source,8192)==-1){
		close(mvfd);
		write(1,"ERROR: no vertex shader file\n",30);
		return 4;
	}
	close(3);//mvfd is 32 LOL
	mffd=syscall(SYS_open,mfspath,0);
	if(syscall(SYS_read,mffd,&fragment_shader_source,8192)==-1){
		close(mffd);
		write(1,"ERROR: no fragment shader file\n",31);
		return 4;
	}
	close(mffd);
	*/
	
	syscall(SYS_rt_sigaction,17,&sas,0,8);
	
	//sound process
#ifdef SOUND
	spid=syscall(SYS_clone,0,0);
	if(spid==0){
#define PCM_DEVICE "default"
	
	char ch=1;
	float b=6.0;
	//int aa=100;

	unsigned int pcm, tmp, dir;
	//int rate, channels;//, seconds;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	double buff;
	//int buff_size, loops;

	/*
	if (argc < 4) {
		printf("Usage: %s <sample_rate> <channels> <seconds>\n",
								argv[0]);
		return -1;
	}
	*/
	

	//rate 	 = atoi(argv[1]);
	//channels = atoi(argv[2]);
	//seconds  = atoi(argv[3]);
	
	void sexithand(){
		snd_pcm_drain(pcm_handle);
		snd_pcm_close(pcm_handle);
		//free(buff);
		syscall(SYS_exit,0);
	}

	struct sigaction soundexit;
	soundexit.sa_handler=&sexithand;

	//rt_sigaction(15,soundexit,0);
	syscall(SYS_rt_sigaction,17,&soundexit,0,15);

	// Open the PCM device in playback mode
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK, 0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));

	// Allocate parameters object and fill it with default values
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	// Set parameters
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
						SND_PCM_FORMAT_S32_LE) < 0) 
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) 
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	// Write parameters
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	// Resume information
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1){
		printf("(mono)\n");
	}else if (tmp == 2){
		printf("(stereo)\n");
	}else{
		printf("(%d)\n",channels);
	}

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	//printf("seconds: %d\n", seconds);	

	// Allocate buffer to hold single period
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	//buff_size = frames * channels * 2; // 2 -> sample size ;
	//buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

	/*
	for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {

		if (pcm = read(2, buff, buff_size) == 0) {
			printf("Early end of file.\n");
			return 0;
		}

		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
			printf("XRUN.\n");
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
		}

	}
	*/
	/*
	char smpl;
	for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {
		for(aa=0;aa<buff_size;) {
			//b=(sin((aa)/channels*3.1415926/rate/(mouse[0]+1))+sin((aa)/channels*3.1415926/rate/(mouse[2]+1)));
			
			for(ch=1;ch<=channels;ch++) {
				//syscall(1,1,&b,1);
				buff[aa]=b*mouse[ch]*10;
			}
			aa=aa++;
		}
		//putchar('\n');
		//write(1,b,1);
		snd_pcm_writei(pcm_handle, buff, frames);
	}
	*/
	if(channels==1){
	while(1){
		if(mouse[0]==0x9){
			buff=99999999999999999999999999999.0f;
			snd_pcm_writei(pcm_handle, &buff, frames);
			//aa++;
			//if(aa>1000)
			//	aa=500;
			buff=0;
			for(char wait=0;wait<17;wait++)
				snd_pcm_writei(pcm_handle, &buff, frames);	
		}
	}
	}else{
	while(1){
		if(mouse[0]==0x9){
			buff=7777777777777779999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0d;
			snd_pcm_writei(pcm_handle, &buff, 16);
			//aa++;
			//if(aa>1000)
			//	aa=500;
			buff=0;
			snd_pcm_writei(pcm_handle, &buff, 16);	
			snd_pcm_writei(pcm_handle, &buff, 16);	

		}else{
			buff=0;
			snd_pcm_writei(pcm_handle, &buff, 16);	
		}

	}
	}
}
#endif
	
	fd_set fds;
	drmEventContext evctx = {
			.version = DRM_EVENT_CONTEXT_VERSION,
			.page_flip_handler = page_flip_handler,
	};
	struct gbm_bo *bo;
	struct drm_fb *fb;
	uint32_t i = 0;
	int ret;

	ret = init_drm();
	if (ret) {
		printf("failed to initialize DRM\n");
		goto exit;
	}

	FD_ZERO(&fds);
	FD_SET(0, &fds);
	FD_SET(drm.fd, &fds);

	ret = init_gbm();
	if (ret) {
		printf("failed to initialize GBM\n");
		goto exit;
	}

	ret = init_gl();
	if (ret) {
		printf("failed to initialize EGL\n");
		goto exit;
	}

	/* clear the color buffer */
	//glClearColor(0.5, 0.5, 0.5, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(gl.display, gl.surface);
	bo = gbm_surface_lock_front_buffer(gbm.surface);
	fb = drm_fb_get_from_bo(bo);

	/* set mode: */
	ret = drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0,
			&drm.connector_id, 1, drm.mode);
	if (ret) {
		printf("failed to set mode: %s\n", strerror(errno));
		goto exit;
	}
	struct gbm_bo *next_bo;
	int waiting_for_flip = 1;
	/*
		eglSwapBuffers(gl.display, gl.surface);
		next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		fb = drm_fb_get_from_bo(next_bo);

		ret = drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,
				DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		if (ret) {
			printf("failed to queue page flip: %s\n", strerror(errno));
			ret= -1;
			goto exit;
		}

		while (waiting_for_flip) {
			ret = select(drm.fd + 1, &fds, NULL, NULL, NULL);
			if (ret < 0) {
				printf("select err: %s\n", strerror(errno));
				goto exit;
			} else if (ret == 0) {
				printf("select timeout!\n");
				ret= -1;
				goto exit;
			} else if (FD_ISSET(0, &fds)) {
				printf("user interrupted!\n");
				break;
			}
			drmHandleEvent(drm.fd, &evctx);
		}
		gbm_surface_release_buffer(gbm.surface, bo);
		bo = next_bo;
		*/
	//mouse process
	//mouse = mmap(0,3,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	posoff = mmap(0,12,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,kapfd,0);
	cur = mmap(0,8,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//modelview = mmap(0,16*4,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//int ppid = getpid();
	char *quit = mmap(0,1,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	quit[0]=0;
	mpid=syscall(SYS_clone,0,0);
	if(mpid==0){
		moufd=syscall(SYS_open,mousepath,0);
		if(moufd==-1){
			syscall(SYS_munmap,mouse,3);
			syscall(1,1,"Meow! Where is the mouse?\n",26);
			return 0;
		}
		while(1){
			syscall(SYS_read,moufd,mouse,3);
			cur[0]+=(float)mouse[1]/200;
			cur[1]-=(float)mouse[2]/200;
			posoff[0]+=(float)mouse[1]/100;
			posoff[1]+=(float)mouse[2]/100;
		}
	}
	//keyboard process
	keyboard = mmap(0,72,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,kapfd,0);
	kpid=syscall(SYS_clone,0,0);
	if(kpid==0){
		kfd=syscall(SYS_open,keyboardpath,0);
		if(kfd==-1){
			//syscall(SYS_munmap,keyboard,72);
			syscall(1,1,"no keyboard found\n",18);
			return 0;
		}
		char keyboard[72];
#ifdef DYNAMIC_KEYS
		char ck=0;
		while(ck<7){
			syscall(SYS_read,kfd,keyboard,72);
			if(keyboard[44]==1){
				key[ck]=keyboard[20];
				ck++;
			}
		}
#endif
		while(1){
			syscall(SYS_read,kfd,keyboard,72);
#ifndef DYNAMIC_KEYS
		//PS/2 keys
		switch(keyboard[20]){
		case 0x1b:{
			posoff[2]-=0.1;
			break;
			  }
		case 0x1d:{
			posoff[2]+=0.1;
			break;
			  }
		case 0x1c:{
			posoff[0]+=0.1;
			break;
			  }
		case 0x23:{
			posoff[0]-=0.1;
			break;
			  }
		case 0x24:{
			posoff[1]-=0.1;
			break;
			  }
		case 0x15:{
			posoff[1]+=0.1;
			break;
			  }
		case 0x2d:{
			aspect+=0.01;
			break;
			  }
		case 0x2b:{
			aspect-=0.01;
			break;
			  }
		case 0x76:{
			//goto exit;
			syscall(SYS_exit,0);
			  }
		}
#else
		//if(keyboard[44]==1){
			if(keyboard[keyoffset]==key[0])
				posoff[2]-=0.6;
			else if(keyboard[keyoffset]==key[1])
				posoff[2]+=0.6;
			else if(keyboard[keyoffset]==key[2])
				posoff[0]+=0.6;
			else if(keyboard[keyoffset]==key[3])
				posoff[0]-=0.6;
			else if(keyboard[keyoffset]==key[4])
				posoff[1]-=0.6;
			else if(keyboard[keyoffset]==key[5])
				posoff[1]+=0.6;
			else if (keyboard[44]==1 && keyboard[keyoffset]==key[6]){
				//syscall(SYS_exit,0);
				//syscall(SYS_kill,ppid,15);
				quit[0]=1;
				printf("quit");
			}
		//}
		
		if(keyboard[44]==0){
			if(keyboard[keyoffset]==key[0] || keyboard[keyoffset]==key[1])
				posoff[2]=0.0;
			if(keyboard[keyoffset]==key[2] || keyboard[keyoffset]==key[3])
				posoff[0]=0.0;
			if(keyboard[keyoffset]==key[4] || keyboard[keyoffset]==key[5])
				posoff[1]=0.0;
		}
		
#endif
		}
	}



	aspect = (GLfloat)(drm.mode->vdisplay) / (GLfloat)(drm.mode->hdisplay);
	posoff[0]=0;
	posoff[1]=0;
	posoff[2]=20;
		esMatrixLoadIdentity(&projection);
		esMatrixLoadIdentity(&modelview);
		esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		//ESMatrix modelviewprojection;
		//esMatrixLoadIdentity(&modelviewprojection);
		glUniformMatrix4fv(gl.modelviewmatrix, 1, GL_FALSE, &modelview.m[0][0]);
	glUniformMatrix4fv(gl.projectionmatrix, 1, GL_FALSE, &projection.m[0][0]);
	//ioctl(dri_fd, DRM_IOCTL_DROP_MASTER, 0);
	
	drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
#define g 0.098
	while(1) {
		//posoff[1]=posoff[1]-g*(posoff[1]>-50);
		//syscall(SYS_read,moufd,&mouse,3);
		//cur[0]+=mouse[1];
		//cur[1]+=mouse[2];
		/*
		if(mouse[0]==0x9){
			if(posoff[0]<(1)&&posoff[0]>(-1)&&posoff[1]<(1)&&posoff[1]>(-1)&&posoff[2]>0){
				write(1,"*You just shot a cube!*\n",25);
				posoff[0]+=rand()%2;
				posoff[1]+=rand()%2;

				cubeoff[0]=random()*0.2;
				//cubeoff[1]=random()*1.1;
				vVertices[0]+=cubeoff[0];
				vVertices[3]+=cubeoff[0];
				vVertices[6]+=cubeoff[0];
				vVertices[9]+=cubeoff[0];
				vVertices[12]+=cubeoff[0];
				vVertices[15]+=cubeoff[0];
				vVertices[18]+=cubeoff[0];
				vVertices[21]+=cubeoff[0];
				vVertices[24]+=cubeoff[0];
				vVertices[27]+=cubeoff[0];
				vVertices[30]+=cubeoff[0];
				vVertices[33]+=cubeoff[0];
				vVertices[36]+=cubeoff[0];
				vVertices[39]+=cubeoff[0];
				vVertices[1]+=cubeoff[0];
				vVertices[4]+=cubeoff[0];
				vVertices[7]+=cubeoff[0];
				vVertices[10]+=cubeoff[0];
				vVertices[13]+=cubeoff[0];
				vVertices[16]+=cubeoff[0];
				vVertices[19]+=cubeoff[0];
				vVertices[22]+=cubeoff[0];
				vVertices[25]+=cubeoff[0];
				vVertices[28]+=cubeoff[0];
				vVertices[31]+=cubeoff[0];
				vVertices[34]+=cubeoff[0];
				vVertices[37]+=cubeoff[0];
				vVertices[40]+=cubeoff[0];
				glBufferSubData(GL_ARRAY_BUFFER, gl.positionsoffset, vnum, &vVertices[0]);
				*/
	/*vVertices[] = {
	-1.0f, 1.0f, 1.0f,     // Front-top-left
	-1.0f, -1.0f, 1.0f,      // Front-top-right
	1.0f, 1.0f, 1.0f,    // Front-bottom-left
	1.0f, -1.0f, 1.0f,     // Front-bottom-right
	1.0f, -1.0f, -1.0f,    // Back-bottom-right
	-1.0f, -1.0f, 1.0f,      // Front-top-right
	-1.0f, -1.0f, -1.0f,     // Back-top-right
	-1.0f, 1.0f, 1.0f,     // Front-top-left
	-1.0f, 1.0f, -1.0f,    // Back-top-left
	1.0f, 1.0f, 1.0f,    // Front-bottom-left
	1.0f, 1.0f, -1.0f,   // Back-bottom-left
	1.0f, -1.0f, -1.0f,    // Back-bottom-right
	-1.0f, 1.0f, -1.0f,    // Back-top-left
	-1.0f, -1.0f, -1.0f      // Back-top-right
	*/
				//vVertices[

		/*
			}else if(posoff[1]>100000){
				write(1,"*You have reached the space! You win!*\n",40);
				goto exit;
			}
		}
		*/
		/*
		switch(mouse[0]){
		case 0x09:
			farcp-=10.0f;
			projection.m[0][0]=1.0f;
			projection.m[0][1]=0.0f;
			projection.m[0][2]=0.0f;
			projection.m[0][3]=0.0f;
			projection.m[1][0]=0.0f;
			projection.m[1][1]=1.0f;
			projection.m[1][2]=0.0f;
			projection.m[1][3]=0.0f;
			projection.m[2][0]=0.0f;
			projection.m[2][1]=0.0f;
			projection.m[2][2]=1.0f;
			projection.m[2][3]=0.0f;
			projection.m[3][0]=0.0f;
			projection.m[3][1]=0.0f;
			projection.m[3][2]=0.0f;
			projection.m[3][3]=1.0f;
			esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		case 0x29:
			farcp+=10.0f;
			projection.m[0][0]=1.0f;
			projection.m[0][1]=0.0f;
			projection.m[0][2]=0.0f;
			projection.m[0][3]=0.0f;
			projection.m[1][0]=0.0f;
			projection.m[1][1]=1.0f;
			projection.m[1][2]=0.0f;
			projection.m[1][3]=0.0f;
			projection.m[2][0]=0.0f;
			projection.m[2][1]=0.0f;
			projection.m[2][2]=1.0f;
			projection.m[2][3]=0.0f;
			projection.m[3][0]=0.0f;
			projection.m[3][1]=0.0f;
			projection.m[3][2]=0.0f;
			projection.m[3][3]=1.0f;
			esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		case 0x0a:
			nearcp+=1.0f;
			projection.m[0][0]=1.0f;
			projection.m[0][1]=0.0f;
			projection.m[0][2]=0.0f;
			projection.m[0][3]=0.0f;
			projection.m[1][0]=0.0f;
			projection.m[1][1]=1.0f;
			projection.m[1][2]=0.0f;
			projection.m[1][3]=0.0f;
			projection.m[2][0]=0.0f;
			projection.m[2][1]=0.0f;
			projection.m[2][2]=1.0f;
			projection.m[2][3]=0.0f;
			projection.m[3][0]=0.0f;
			projection.m[3][1]=0.0f;
			projection.m[3][2]=0.0f;
			projection.m[3][3]=1.0f;
			esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		case 0x1a:
			nearcp-=1.0f;
			projection.m[0][0]=1.0f;
			projection.m[0][1]=0.0f;
			projection.m[0][2]=0.0f;
			projection.m[0][3]=0.0f;
			projection.m[1][0]=0.0f;
			projection.m[1][1]=1.0f;
			projection.m[1][2]=0.0f;
			projection.m[1][3]=0.0f;
			projection.m[2][0]=0.0f;
			projection.m[2][1]=0.0f;
			projection.m[2][2]=1.0f;
			projection.m[2][3]=0.0f;
			projection.m[3][0]=0.0f;
			projection.m[3][1]=0.0f;
			projection.m[3][2]=0.0f;
			projection.m[3][3]=1.0f;
			esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		case 0x0c:
			viewsize-=1.0f;
			projection.m[0][0]=1.0f;
			projection.m[0][1]=0.0f;
			projection.m[0][2]=0.0f;
			projection.m[0][3]=0.0f;
			projection.m[1][0]=0.0f;
			projection.m[1][1]=1.0f;
			projection.m[1][2]=0.0f;
			projection.m[1][3]=0.0f;
			projection.m[2][0]=0.0f;
			projection.m[2][1]=0.0f;
			projection.m[2][2]=1.0f;
			projection.m[2][3]=0.0f;
			projection.m[3][0]=0.0f;
			projection.m[3][1]=0.0f;
			projection.m[3][2]=0.0f;
			projection.m[3][3]=1.0f;
			esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		case 0x2c:
			viewsize+=1.0f;
			projection.m[0][0]=1.0f;
			projection.m[0][1]=0.0f;
			projection.m[0][2]=0.0f;
			projection.m[0][3]=0.0f;
			projection.m[1][0]=0.0f;
			projection.m[1][1]=1.0f;
			projection.m[1][2]=0.0f;
			projection.m[1][3]=0.0f;
			projection.m[2][0]=0.0f;
			projection.m[2][1]=0.0f;
			projection.m[2][2]=1.0f;
			projection.m[2][3]=0.0f;
			projection.m[3][0]=0.0f;
			projection.m[3][1]=0.0f;
			projection.m[3][2]=0.0f;
			projection.m[3][3]=1.0f;
			esFrustum(&projection, -2.8f*viewsize, +2.8f*viewsize, -2.8f * aspect*viewsize, +2.8f * aspect*viewsize, nearcp, farcp);
		}
		*/

		waiting_for_flip = 1;
		//i++;

		/*if(mouse[0]==12){
			break;
		}*/

		/* clear the color buffer */
		//glClearColor((10/(posoff[1]+.01)), (1000.0f/(posoff[1]+.01)), (2500.0f/(posoff[1]+.01)), 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//ESMatrix modelview;
		//esMatrixLoadIdentity(&modelview);

		//esTranslate(&modelview, 0.0, 0.0, 0.0);
		//esTranslate(&modelview, posoff[0], posoff[1], posoff[2]);
		//esRotate(&modelview, 90, 0.0f, 0.0f, 1.0f);
		/*
		modelview.m[0][0]=1.0f;
		modelview.m[0][1]=0.0f;
		modelview.m[0][2]=0.0f;
		modelview.m[0][3]=0.0f;
		modelview.m[1][0]=0.0f;
		modelview.m[1][1]=1.0f;
		modelview.m[1][2]=0.0f;
		modelview.m[1][3]=0.0f;
		modelview.m[2][0]=0.0f;
		modelview.m[2][1]=0.0f;
		modelview.m[2][2]=1.0f;
		modelview.m[2][3]=0.0f;
		modelview.m[3][0]=0.0f;
		modelview.m[3][1]=0.0f;
		modelview.m[3][2]=0.0f;
		modelview.m[3][3]=1.0f;
		*/
		/*
#define esMatrixMultiply(result, srcA, srcB){\
    ESMatrix    _tmp;\
    int _i;
	for (_i=0; _i<4; _i++)\
	{\
		_tmp.m[_i][0] = (srcA.m[_i][0] * srcB.m[0][0]) +\
						(srcA.m[_i][1] * srcB.m[1][0]) +\
						(srcA.m[_i][2] * srcB.m[2][0]) +\
						(srcA.m[_i][3] * srcB.m[3][0]) ;\
		_tmp.m[_i][1] = (srcA.m[_i][0] * srcB.m[0][1]) + \
						(srcA.m[_i][1] * srcB.m[1][1]) +\
						(srcA.m[_i][2] * srcB.m[2][1]) +\
						(srcA.m[_i][3] * srcB.m[3][1]) ;\
		_tmp.m[_i][2] = (srcA.m[_i][0] * srcB.m[0][2]) + \
						(srcA.m[_i][1] * srcB.m[1][2]) +\
						(srcA.m[_i][2] * srcB.m[2][2]) +\
						(srcA.m[_i][3] * srcB.m[3][2]) ;\
		_tmp.m[_i][3] = (srcA.m[_i][0] * srcB.m[0][3]) + \
						(srcA.m[_i][1] * srcB.m[1][3]) +\
						(srcA.m[_i][2] * srcB.m[2][3]) +\
						(srcA.m[_i][3] * srcB.m[3][3]) ;\
	}\
    memcpy(&result, &_tmp, sizeof(ESMatrix));\
}
#define esRotate(result, angle, x, y, z){\
	GLfloat sinAngle, cosAngle;\
	GLfloat mag = sqrtf(x * x + y * y + z * z);\
	sinAngle = sinf ( angle * PI / 180.0f );\
	cosAngle = cosf ( angle * PI / 180.0f );\
	if ( mag > 0.0f ){\
		GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;\
		GLfloat oneMinusCos;\
		ESMatrix rotMat;\
		x /= mag;\
		y /= mag;\
		z /= mag;\
		xx = x * x;\
		yy = y * y;\
		zz = z * z;\
		xy = x * y;\
		yz = y * z;\
		zx = z * x;\
		xs = x * sinAngle;\
		ys = y * sinAngle;\
		zs = z * sinAngle;\
		oneMinusCos = 1.0f - cosAngle;\
		rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;\
		rotMat.m[0][1] = (oneMinusCos * xy) - zs;\
		rotMat.m[0][2] = (oneMinusCos * zx) + ys;\
		rotMat.m[0][3] = 0.0F;\
		rotMat.m[1][0] = (oneMinusCos * xy) + zs;\
		rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;\
		rotMat.m[1][2] = (oneMinusCos * yz) - xs;\
		rotMat.m[1][3] = 0.0F;\
		rotMat.m[2][0] = (oneMinusCos * zx) - ys;\
		rotMat.m[2][1] = (oneMinusCos * yz) + xs;\
		rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;\
		rotMat.m[2][3] = 0.0F;\
		rotMat.m[3][0] = 0.0F;\
		rotMat.m[3][1] = 0.0F;\
		rotMat.m[3][2] = 0.0F;\
		rotMat.m[3][3] = 1.0F;\
		esMatrixMultiply(result,rotMat,result);\
	}\
}
#define mouseRotate(result,_w,_x,_y,_z){\
	GLfloat sinAngle, cosAngle;\
	GLfloat mag = sqrtf(x * x + y * y + z * z);\
	if(mag > 0.0f){\
		sinAngle = sinf ( angle * PI / 180.0f );\
		cosAngle = cosf ( angle * PI / 180.0f );\
		GLfloat xx, xs, oneMinusCos;\
	}
	esMatrixMultiply(result,view,result);
}
		*/



		/*
		ESMatrix rotX,rotY,rotA;
		ESMatrixLoadIdentity(&rotX);
		esRotate(&rotX, mouse[2], 1.0f, 0.0f, 0.0f);
		ESMatrixLoadIdentity(&rotY);
		esRotate(&rotY, mouse[1], 0.0f, 1.0f, 0.0f);
		esMatrixMultiply(&rotA,&rotY,&rotX);
		float rotAl;
		rotAl=sqrt(rotA.m[0][0]*rotA.m[0][0]+);
		*/
		esRotate(&modelview, sqrt(cur[0]*cur[0]+cur[1]*cur[1]), cur[0], cur[1], 0.0f);

		//posoff[0]=position[0];
		//posoff[1]=position[1];
		//posoff[2]=position[2];
		//esTranslate(&modelview, position[0], position[1], position[2]);
		//esTranslate(&modelview, posoff[0], posoff[1], posoff[2]);
		
		//esRotate(&modelview, 45.0f + (0.25f * i), 1.0f, 0.0f, 0.0f);
		//esRotate(&modelview, 45.0f - (0.5f * i), 0.0f, 1.0f, 0.0f);
		//esRotate(&modelview, 10.0f + (0.15f * i), 0.0f, 0.0f, 1.0f);
		

		//GLfloat aspect = (GLfloat)(drm.mode->vdisplay) / (GLfloat)(drm.mode->hdisplay);

		//ESMatrix projection;
		//esMatrixLoadIdentity(&projection);

		//esFrustum(&projection, -frustumW, frustumW, -frustumH, frustumH, 1.0f, 100.0f);
		//esFrustum(&projection, -2.8f*3, +2.8f*3, -2.8f * aspect*3, +2.8f * aspect*3, 10.00f, 100000000.0f);

		//esMatrixMultiply(&modelviewprojection, &modelview, &projection);

		/*
		float normal[9];
		normal[0] = modelview.m[0][0];
		normal[1] = modelview.m[0][1];
		normal[2] = modelview.m[0][2];
		normal[3] = modelview.m[1][0];
		normal[4] = modelview.m[1][1];
		normal[5] = modelview.m[1][2];
		normal[6] = modelview.m[2][0];
		normal[7] = modelview.m[2][1];
		normal[8] = modelview.m[2][2];
		*/

		//glUniformMatrix4fv(gl.modelviewmatrix, 1, GL_FALSE, &modelview.m[0][0]);
		//glUniformMatrix4fv(gl.modelviewprojectionmatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);
		//glUniformMatrix3fv(gl.normalmatrix, 1, GL_FALSE, &modelview.);
		glUniform4fv(gl.posoffvector, 1, posoff);
		//glUniform2fv(gl.mouse, 1, cur);
		//write(1,&posoff,16);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, vnum/3);
		//glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

		eglSwapBuffers(gl.display, gl.surface);
		next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		fb = drm_fb_get_from_bo(next_bo);

		//drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
	//			DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		/*
		 * Here you could also update drm plane layers if you want
		 * hw composition
		 */

		
		
		ret = drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,
				DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		if (ret) {
			printf("failed to queue page flip: %s\n", strerror(errno));
			return -1;
		}

		while (waiting_for_flip) {
			ret = select(drm.fd + 1, &fds, NULL, NULL, NULL);
			if (ret < 0) {
				printf("select err: %s\n", strerror(errno));
				goto exit;
			} else if (ret == 0) {
				printf("select timeout!\n");
				ret=-1;
				goto exit;
			} else if (FD_ISSET(0, &fds)) {
				printf("user interrupted!\n");
				break;
			}
			drmHandleEvent(drm.fd, &evctx);
		}
		
		

		/* release last buffer to render on again: */
		gbm_surface_release_buffer(gbm.surface, bo);
		bo = next_bo;
	}
exit:
	syscall(SYS_kill,mpid,15);
	syscall(SYS_kill,kpid,15);
#ifdef SOUND
	syscall(SYS_kill,spid,15);
#endif
	syscall(SYS_munmap,mouse,3);
	//syscall(SYS_munmap,cur,8);
	syscall(SYS_munmap,posoff,12);
	drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0,
			&drm.connector_id, 1, drm.mode);
	//drmModeFreeCrtc (crtc);

	if (bo) {
		drmModeRmFB(drm.fd, fb->fb_id);
		gbm_surface_release_buffer (gbm.surface, bo);
	}

	glDetachShader(gl.program,vertex_shader);
	glDetachShader(gl.program,fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(gl.program);
	//glDeleteBuffers(&bo);
	eglDestroySurface (gl.display, gl.surface);
	gbm_surface_destroy (gbm.surface);
	eglDestroyContext (gl.display, gl.context);
	eglTerminate (gl.display);
	gbm_device_destroy (gbm.dev);

	syscall(SYS_wait4,mpid,0,0,0);
	syscall(SYS_wait4,kpid,0,0,0);
#ifdef SOUND
	syscall(SYS_wait4,spid,0,0,0);
	syscall(SYS_kill,spid,9);
#endif
	syscall(SYS_kill,mpid,9);
	syscall(SYS_kill,kpid,9);
	return ret;
}
