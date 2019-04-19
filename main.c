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
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <assert.h>

#include "esUtil.h"
#define PI 3.1415926535897932384626433832795f
#include <math.h>

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#include <linux/input-event-codes.h>

#include <alsa/asoundlib.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

char *mouse;
char moufd;
char mapfd;
char *keyboard;
char kfd;
char kapfd;
float cur[2];
float position[3];

static struct {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
	EGLSurface surface;
	GLuint program;
	GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
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

	drm.fd = open("/dev/dri/card0", O_RDWR);

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

int vnum=14*3;
//static const char *vertex_shader_source;
//static const char *fragment_shader_source;
//char mvfd, mffd;
static int init_gl(void)
{
	EGLint major, minor, n;
	GLuint vertex_shader, fragment_shader;
	GLint ret;

	static const GLfloat vVertices[] = {
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

	static const GLfloat vColors[] = {
			// front
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f, // magenta
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			// back
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  1.0f,  0.0f, // yellow
			0.0f,  1.0f,  0.0f, // green
			// right
			1.0f,  0.0f,  1.0f, // magenta
			1.0f,  0.0f,  0.0f, // red
			1.0f,  1.0f,  1.0f, // white
			1.0f,  1.0f,  0.0f, // yellow
			// left
			0.0f,  0.0f,  0.0f, // black
			0.0f,  0.0f,  1.0f, // blue
			0.0f,  1.0f,  0.0f, // green
			0.0f,  1.0f,  1.0f, // cyan
			// top
			0.0f,  1.0f,  1.0f, // cyan
			1.0f,  1.0f,  1.0f, // white
			0.0f,  1.0f,  0.0f, // green
			1.0f,  1.0f,  0.0f, // yellow
			// bottom
			0.0f,  0.0f,  0.0f, // black
			1.0f,  0.0f,  0.0f, // red
			0.0f,  0.0f,  1.0f, // blue
			1.0f,  0.0f,  1.0f  // magenta
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
			"uniform mat4 modelviewMatrix;      \n"
			"uniform mat4 modelviewprojectionMatrix;\n"
			"uniform mat3 normalMatrix;         \n"
			"                                   \n"
			"attribute vec4 in_position;        \n"
			"attribute vec3 in_normal;          \n"
			"attribute vec4 in_color;           \n"
			"\n"
			"vec4 lightSource = vec4(2.0, 2.0, 20.0, 0.0);\n"
			"                                   \n"
			"varying vec4 vVaryingColor;        \n"
			"                                   \n"
			"void main()                        \n"
			"{                                  \n"
			"    gl_Position = modelviewprojectionMatrix * in_position;\n"
			"    vec3 vEyeNormal = normalMatrix * in_normal;\n"
			"    vec4 vPosition4 = modelviewMatrix * in_position;\n"
			"    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;\n"
			"    vec3 vLightDir = normalize(lightSource.xyz - vPosition3);\n"
			"    float diff = max(0.0, dot(vEyeNormal, vLightDir));\n"
			"    vVaryingColor = vec4(diff * in_color.rgb, 1.0);\n"
			"}                                  \n";



	static const char *fragment_shader_source =
			"precision mediump float;           \n"
			"                                   \n"
			"varying vec4 vVaryingColor;        \n"
			"                                   \n"
			"void main()                        \n"
			"{                                  \n"
			"    gl_FragColor = vVaryingColor;  \n"
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
	gl.modelviewprojectionmatrix = glGetUniformLocation(gl.program, "modelviewprojectionMatrix");
	gl.normalmatrix = glGetUniformLocation(gl.program, "normalMatrix");

	glViewport(0, 0, drm.mode->hdisplay, drm.mode->vdisplay);
	glEnable(GL_CULL_FACE);

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
//char mvspath[256];
//char mfspath[256];
int mpid,kpid,spid;
//long int rate;
//char channels;

GLfloat aspect,frustumW,frustumH;

int main(int argc, char *argv[])
{
	//read config
	conf.fd = open("editme.conf",0);
	if(conf.fd==-1){
		syscall(SYS_write,1,"ERROR: no editme.conf\n",21);
		return -1;
	}
	conf.size = read(conf.fd,&conf.text,2000);
	close(conf.fd);
	conf.op=0;
	int csmb;
	for(int symb=0;symb<conf.size;++symb){
		switch(conf.op){
		case 0:{
			switch(conf.text[symb]){
			case '/':{
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
			 }
			}
		case 1:{
			switch(conf.text[symb]){
			case '/':{
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
			}
		       }
		       /*
		case 2:{
			csmb=0;
			while(conf.text[symb]!='\n'){
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
				symb++;
			}
		       }
		case 3:{
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
		       */
		       /*
		case 4:{
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
		case 5:{
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

	//mouse process
	mouse = mmap(0,3,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	mpid=syscall(SYS_clone,0,0);
	if(mpid==0){
		moufd=syscall(SYS_open,mousepath,0);
		while(1){
			syscall(SYS_read,moufd,mouse,3);
		}
	}
	//keyboard process
	keyboard = mmap(0,72,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,kapfd,0);
	kpid=syscall(SYS_clone,0,0);
	if(kpid==0){
		kfd=syscall(SYS_open,keyboardpath,0);
		while(1){
			syscall(SYS_read,kfd,keyboard,72);
		}
	}
	//sound process
#ifdef SOUND
	spid=syscall(SYS_clone,0,0);
	if(spid==0){
#define PCM_DEVICE "default"
	
	char ch=1;
	float b=6.0;
	int aa=0;

	unsigned int pcm, tmp, dir;
	int rate, channels;//, seconds;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	char *buff;
	int buff_size, loops;

	if (argc < 4) {
		printf("Usage: %s <sample_rate> <channels> <seconds>\n",
								argv[0]);
		return -1;
	}
	

	//rate 	 = atoi(argv[1]);
	//channels = atoi(argv[2]);
	//seconds  = atoi(argv[3]);
	
	void sexithand(){
		snd_pcm_drain(pcm_handle);
		snd_pcm_close(pcm_handle);
		free(buff);
		syscall(SYS_exit,0);
	}

	struct sigaction soundexit;
	soundexit.sa_handler=&sexithand;

	rt_sigaction(15,soundexit,0);

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK, 0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/	
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
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

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
		printf("(mono)\n");
	else if (tmp == 2)
		printf("(stereo)\n");

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	//printf("seconds: %d\n", seconds);	

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);

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
	while(1){

		snd_pcm_writei(pcm_handle, buff, frames);	
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
		return ret;
	}

	FD_ZERO(&fds);
	FD_SET(0, &fds);
	FD_SET(drm.fd, &fds);

	ret = init_gbm();
	if (ret) {
		printf("failed to initialize GBM\n");
		return ret;
	}

	ret = init_gl();
	if (ret) {
		printf("failed to initialize EGL\n");
		return ret;
	}

	/* clear the color buffer */
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(gl.display, gl.surface);
	bo = gbm_surface_lock_front_buffer(gbm.surface);
	fb = drm_fb_get_from_bo(bo);

	/* set mode: */
	ret = drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0,
			&drm.connector_id, 1, drm.mode);
	if (ret) {
		printf("failed to set mode: %s\n", strerror(errno));
		return ret;
	}



	struct gbm_bo *next_bo;
	int waiting_for_flip = 1;
	aspect = (GLfloat)(drm.mode->vdisplay) / (GLfloat)(drm.mode->hdisplay);

	//moufd=syscall(SYS_open,mousepath,0);
	while (1) {
		//syscall(SYS_read,moufd,&mouse,3);
		cur[0]+=mouse[1];
		cur[1]+=mouse[2];
		switch(keyboard[20]){
		case 0x1b:{
			position[2]-=1.0;
			break;
			  }
		case 0x1d:{
			position[2]+=1.0;
			break;
			  }
		case 0x1c:{
			position[0]+=0.1;
			break;
			  }
		case 0x23:{
			position[0]-=0.1;
			break;
			  }
		case 0x24:{
			position[1]-=0.1;
			break;
			  }
		case 0x15:{
			position[1]+=0.1;
			break;
			  }
		case 0x2d:{
			aspect+=0.05;
			break;
			  }
		case 0x2b:{
			aspect-=0.05;
			break;
			  }
		case 0x76:{
			goto exit;
			  }
		}
		if(mouse[0]==0x9){
			if(position[0]<1&&position[0]>-1&&position[1]<1&&position[1]>-1&&position[2]<0){
				write(1,"\n*You just shot a cube!*\n",24);
			}
		}

		waiting_for_flip = 1;
		i++;

		/*if(mouse[0]==12){
			break;
		}*/
		ESMatrix modelview;

		/* clear the color buffer */
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		esMatrixLoadIdentity(&modelview);
		
		//ESMatrix camera;
		//esMatrixLoadIdentity(&camera);
		
		//esTranslate(&camera, position[0], position[1], position[2]);
		//esRotate(&camera, cur[0], 0.0f, 1.0f, 0.0f);
		//esRotate(&camera, cur[1], 0.0f, 0.0f, 1.0f);
		//esMatrixMultiply(&camera, &modelview, &modelview);
		
		esTranslate(&modelview, position[0], position[1], position[2]);
		//esRotate(&modelview, 45.0f + (0.25f * i), 1.0f, 0.0f, 0.0f);
		//esRotate(&modelview, 45.0f - (0.5f * i), 0.0f, 1.0f, 0.0f);
		//esRotate(&modelview, 10.0f + (0.15f * i), 0.0f, 0.0f, 1.0f);
		esRotate(&modelview, cur[0], 0.0f, 1.0f, 0.0f);
		esRotate(&modelview, cur[1], 0.0f, 0.0f, 1.0f);

		//GLfloat aspect = (GLfloat)(drm.mode->vdisplay) / (GLfloat)(drm.mode->hdisplay);

		ESMatrix projection;
		esMatrixLoadIdentity(&projection);

		//esFrustum(&projection, -frustumW, frustumW, -frustumH, frustumH, 1.0f, 100.0f);
		esFrustum(&projection, -2.8f, +2.8f, -2.8f * aspect, +2.8f * aspect, 10.00f, 100000.0f);

		ESMatrix modelviewprojection;
		esMatrixLoadIdentity(&modelviewprojection);
		esMatrixMultiply(&modelviewprojection, &modelview, &projection);

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

		glUniformMatrix4fv(gl.modelviewmatrix, 1, GL_FALSE, &modelview.m[0][0]);
		glUniformMatrix4fv(gl.modelviewprojectionmatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);
		glUniformMatrix3fv(gl.normalmatrix, 1, GL_FALSE, normal);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, vnum/3);
		//glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
		//glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

		eglSwapBuffers(gl.display, gl.surface);
		next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		fb = drm_fb_get_from_bo(next_bo);

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
				return ret;
			} else if (ret == 0) {
				printf("select timeout!\n");
				return -1;
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
	syscall(62,mpid,15);
	syscall(62,kpid,15);
	drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0,
			&drm.connector_id, 1, drm.mode);
	//drmModeFreeCrtc (crtc);

	if (bo) {
		drmModeRmFB(drm.fd, fb->fb_id);
		gbm_surface_release_buffer (gbm.surface, bo);
	}

	eglDestroySurface (gl.display, gl.surface);
	gbm_surface_destroy (gbm.surface);
	eglDestroyContext (gl.display, gl.context);
	eglTerminate (gl.display);
	gbm_device_destroy (gbm.dev);

	return ret;
}
