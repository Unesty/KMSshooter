/*
 * modeset - DRM Modesetting Example
 *
 * Written 2012 by David Herrmann <dh.herrmann@googlemail.com>
 * Dedicated to the Public Domain.
 */

/*
 * DRM Modesetting Howto
 * This document describes the DRM modesetting API. Before we can use the DRM
 * API, we have to include xf86drm.h and xf86drmMode.h. Both are provided by
 * libdrm which every major distribution ships by default. It has no other
 * dependencies and is pretty small.
 *
 * Please ignore all forward-declarations of functions which are used later. I
 * reordered the functions so you can read this document from top to bottom. If
 * you reimplement it, you would probably reorder the functions to avoid all the
 * nasty forward declarations.
 *
 * For easier reading, we ignore all memory-allocation errors of malloc() and
 * friends here. However, we try to correctly handle all other kinds of errors
 * that may occur.
 *
 * All functions and global variables are prefixed with "modeset_*" in this
 * file. So it should be clear whether a function is a local helper or if it is
 * provided by some external library.
 */

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>

#include <math.h>

struct modeset_dev;
static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn,
			     struct modeset_dev *dev);
static int modeset_create_fb(int fd, struct modeset_dev *dev);
static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn,
			     struct modeset_dev *dev);
static int modeset_open(int *out, const char *node);
static int modeset_prepare(int fd);
static void modeset_draw(void);
static void modeset_cleanup(int fd);

/*
 * When the linux kernel detects a graphics-card on your machine, it loads the
 * correct device driver (located in kernel-tree at ./drivers/gpu/drm/<xy>) and
 * provides two character-devices to control it. Udev (or whatever hotplugging
 * application you use) will create them as:
 *     /dev/dri/card0
 *     /dev/dri/controlID64
 * We only need the first one. You can hard-code this path into your application
 * like we do here, but it is recommended to use libudev with real hotplugging
 * and multi-seat support. However, this is beyond the scope of this document.
 * Also note that if you have multiple graphics-cards, there may also be
 * /dev/dri/card1, /dev/dri/card2, ...
 *
 * We simply use /dev/dri/card0 here but the user can specify another path on
 * the command line.
 *
 * modeset_open(out, node): This small helper function opens the DRM device
 * which is given as @node. The new fd is stored in @out on success. On failure,
 * a negative error code is returned.
 * After opening the file, we also check for the DRM_CAP_DUMB_BUFFER capability.
 * If the driver supports this capability, we can create simple memory-mapped
 * buffers without any driver-dependent code. As we want to avoid any radeon,
 * nvidia, intel, etc. specific code, we depend on DUMB_BUFFERs here.
 */

static int modeset_open(int *out, const char *node)
{
	int fd, ret;
	uint64_t has_dumb;

	fd = open(node, O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		ret = -errno;
		fprintf(stderr, "cannot open '%s': %m\n", node);
		return ret;
	}

	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
	    !has_dumb) {
		fprintf(stderr, "drm device '%s' does not support dumb buffers\n",
			node);
		close(fd);
		return -EOPNOTSUPP;
	}

	*out = fd;
	return 0;
}

/*
 * As a next step we need to find our available display devices. libdrm provides
 * a drmModeRes structure that contains all the needed information. We can
 * retrieve it via drmModeGetResources(fd) and free it via
 * drmModeFreeResources(res) again.
 *
 * A physical connector on your graphics card is called a "connector". You can
 * plug a monitor into it and control what is displayed. We are definitely
 * interested in what connectors are currently used, so we simply iterate
 * through the list of connectors and try to display a test-picture on each
 * available monitor.
 * However, this isn't as easy as it sounds. First, we need to check whether the
 * connector is actually used (a monitor is plugged in and turned on). Then we
 * need to find a CRTC that can control this connector. CRTCs are described
 * later on. After that we create a framebuffer object. If we have all this, we
 * can mmap() the framebuffer and draw a test-picture into it. Then we can tell
 * the DRM device to show the framebuffer on the given CRTC with the selected
 * connector.
 *
 * As we want to draw moving pictures on the framebuffer, we actually have to
 * remember all these settings. Therefore, we create one "struct modeset_dev"
 * object for each connector+crtc+framebuffer pair that we successfully
 * initialized and push it into the global device-list.
 *
 * Each field of this structure is described when it is first used. But as a
 * summary:
 * "struct modeset_dev" contains: {
 *  - @next: points to the next device in the single-linked list
 *
 *  - @width: width of our buffer object
 *  - @height: height of our buffer object
 *  - @stride: stride value of our buffer object
 *  - @size: size of the memory mapped buffer
 *  - @handle: a DRM handle to the buffer object that we can draw into
 *  - @map: pointer to the memory mapped buffer
 *
 *  - @mode: the display mode that we want to use
 *  - @fb: a framebuffer handle with our buffer object as scanout buffer
 *  - @conn: the connector ID that we want to use with this buffer
 *  - @crtc: the crtc ID that we want to use with this connector
 *  - @saved_crtc: the configuration of the crtc before we changed it. We use it
 *                 so we can restore the same mode when we exit.
 * }
 */

struct modeset_dev {
	struct modeset_dev *next;

	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint32_t handle;
	uint8_t *map;

	drmModeModeInfo mode;
	uint32_t fb;
	uint32_t conn;
	uint32_t crtc;
	drmModeCrtc *saved_crtc;
};

static struct modeset_dev *modeset_list = NULL;

/*
 * So as next step we need to actually prepare all connectors that we find. We
 * do this in this little helper function:
 *
 * modeset_prepare(fd): This helper function takes the DRM fd as argument and
 * then simply retrieves the resource-info from the device. It then iterates
 * through all connectors and calls other helper functions to initialize this
 * connector (described later on).
 * If the initialization was successful, we simply add this object as new device
 * into the global modeset device list.
 *
 * The resource-structure contains a list of all connector-IDs. We use the
 * helper function drmModeGetConnector() to retrieve more information on each
 * connector. After we are done with it, we free it again with
 * drmModeFreeConnector().
 * Our helper modeset_setup_dev() returns -ENOENT if the connector is currently
 * unused and no monitor is plugged in. So we can ignore this connector.
 */

static int modeset_prepare(int fd)
{
	drmModeRes *res;
	drmModeConnector *conn;
	unsigned int i;
	struct modeset_dev *dev;
	int ret;

	/* retrieve resources */
	res = drmModeGetResources(fd);
	if (!res) {
		fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n",
			errno);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; ++i) {
		/* get information for each connector */
		conn = drmModeGetConnector(fd, res->connectors[i]);
		if (!conn) {
			fprintf(stderr, "cannot retrieve DRM connector %u:%u (%d): %m\n",
				i, res->connectors[i], errno);
			continue;
		}

		/* create a device structure */
		dev = malloc(sizeof(*dev));
		memset(dev, 0, sizeof(*dev));
		dev->conn = conn->connector_id;

		/* call helper function to prepare this connector */
		ret = modeset_setup_dev(fd, res, conn, dev);
		if (ret) {
			if (ret != -ENOENT) {
				errno = -ret;
				fprintf(stderr, "cannot setup device for connector %u:%u (%d): %m\n",
					i, res->connectors[i], errno);
			}
			free(dev);
			drmModeFreeConnector(conn);
			continue;
		}

		/* free connector data and link device into global list */
		drmModeFreeConnector(conn);
		dev->next = modeset_list;
		modeset_list = dev;
	}

	/* free resources again */
	drmModeFreeResources(res);
	return 0;
}

/*
 * Now we dig deeper into setting up a single connector. As described earlier,
 * we need to check several things first:
 *   * If the connector is currently unused, that is, no monitor is plugged in,
 *     then we can ignore it.
 *   * We have to find a suitable resolution and refresh-rate. All this is
 *     available in drmModeModeInfo structures saved for each crtc. We simply
 *     use the first mode that is available. This is always the mode with the
 *     highest resolution.
 *     A more sophisticated mode-selection should be done in real applications,
 *     though.
 *   * Then we need to find an CRTC that can drive this connector. A CRTC is an
 *     internal resource of each graphics-card. The number of CRTCs controls how
 *     many connectors can be controlled indepedently. That is, a graphics-cards
 *     may have more connectors than CRTCs, which means, not all monitors can be
 *     controlled independently.
 *     There is actually the possibility to control multiple connectors via a
 *     single CRTC if the monitors should display the same content. However, we
 *     do not make use of this here.
 *     So think of connectors as pipelines to the connected monitors and the
 *     CRTCs are the controllers that manage which data goes to which pipeline.
 *     If there are more pipelines than CRTCs, then we cannot control all of
 *     them at the same time.
 *   * We need to create a framebuffer for this connector. A framebuffer is a
 *     memory buffer that we can write XRGB32 data into. So we use this to
 *     render our graphics and then the CRTC can scan-out this data from the
 *     framebuffer onto the monitor.
 */

static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn,
			     struct modeset_dev *dev)
{
	int ret;

	/* check if a monitor is connected */
	if (conn->connection != DRM_MODE_CONNECTED) {
		fprintf(stderr, "ignoring unused connector %u\n",
			conn->connector_id);
		return -ENOENT;
	}

	/* check if there is at least one valid mode */
	if (conn->count_modes == 0) {
		fprintf(stderr, "no valid mode for connector %u\n",
			conn->connector_id);
		return -EFAULT;
	}

	/* copy the mode information into our device structure */
	memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
	dev->width = conn->modes[0].hdisplay;
	dev->height = conn->modes[0].vdisplay;
	fprintf(stderr, "mode for connector %u is %ux%u\n",
		conn->connector_id, dev->width, dev->height);

	/* find a crtc for this connector */
	ret = modeset_find_crtc(fd, res, conn, dev);
	if (ret) {
		fprintf(stderr, "no valid crtc for connector %u\n",
			conn->connector_id);
		return ret;
	}

	/* create a framebuffer for this CRTC */
	ret = modeset_create_fb(fd, dev);
	if (ret) {
		fprintf(stderr, "cannot create framebuffer for connector %u\n",
			conn->connector_id);
		return ret;
	}

	return 0;
}

/*
 * modeset_find_crtc(fd, res, conn, dev): This small helper tries to find a
 * suitable CRTC for the given connector. We have actually have to introduce one
 * more DRM object to make this more clear: Encoders.
 * Encoders help the CRTC to convert data from a framebuffer into the right
 * format that can be used for the chosen connector. We do not have to
 * understand any more of these conversions to make use of it. However, you must
 * know that each connector has a limited list of encoders that it can use. And
 * each encoder can only work with a limited list of CRTCs. So what we do is
 * trying each encoder that is available and looking for a CRTC that this
 * encoder can work with. If we find the first working combination, we are happy
 * and write it into the @dev structure.
 * But before iterating all available encoders, we first try the currently
 * active encoder+crtc on a connector to avoid a full modeset.
 *
 * However, before we can use a CRTC we must make sure that no other device,
 * that we setup previously, is already using this CRTC. Remember, we can only
 * drive one connector per CRTC! So we simply iterate through the "modeset_list"
 * of previously setup devices and check that this CRTC wasn't used before.
 * Otherwise, we continue with the next CRTC/Encoder combination.
 */

static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn,
			     struct modeset_dev *dev)
{
	drmModeEncoder *enc;
	unsigned int i, j;
	int32_t crtc;
	struct modeset_dev *iter;

	/* first try the currently conected encoder+crtc */
	if (conn->encoder_id)
		enc = drmModeGetEncoder(fd, conn->encoder_id);
	else
		enc = NULL;

	if (enc) {
		if (enc->crtc_id) {
			crtc = enc->crtc_id;
			for (iter = modeset_list; iter; iter = iter->next) {
				if (iter->crtc == crtc) {
					crtc = -1;
					break;
				}
			}

			if (crtc >= 0) {
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	/* If the connector is not currently bound to an encoder or if the
	 * encoder+crtc is already used by another connector (actually unlikely
	 * but lets be safe), iterate all other available encoders to find a
	 * matching CRTC. */
	for (i = 0; i < conn->count_encoders; ++i) {
		enc = drmModeGetEncoder(fd, conn->encoders[i]);
		if (!enc) {
			fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n",
				i, conn->encoders[i], errno);
			continue;
		}

		/* iterate all global CRTCs */
		for (j = 0; j < res->count_crtcs; ++j) {
			/* check whether this CRTC works with the encoder */
			if (!(enc->possible_crtcs & (1 << j)))
				continue;

			/* check that no other device already uses this CRTC */
			crtc = res->crtcs[j];
			for (iter = modeset_list; iter; iter = iter->next) {
				if (iter->crtc == crtc) {
					crtc = -1;
					break;
				}
			}

			/* we have found a CRTC, so save it and return */
			if (crtc >= 0) {
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	fprintf(stderr, "cannot find suitable CRTC for connector %u\n",
		conn->connector_id);
	return -ENOENT;
}

/*
 * modeset_create_fb(fd, dev): After we have found a crtc+connector+mode
 * combination, we need to actually create a suitable framebuffer that we can
 * use with it. There are actually two ways to do that:
 *   * We can create a so called "dumb buffer". This is a buffer that we can
 *     memory-map via mmap() and every driver supports this. We can use it for
 *     unaccelerated software rendering on the CPU.
 *   * We can use libgbm to create buffers available for hardware-acceleration.
 *     libgbm is an abstraction layer that creates these buffers for each
 *     available DRM driver. As there is no generic API for this, each driver
 *     provides its own way to create these buffers.
 *     We can then use such buffers to create OpenGL contexts with the mesa3D
 *     library.
 * We use the first solution here as it is much simpler and doesn't require any
 * external libraries. However, if you want to use hardware-acceleration via
 * OpenGL, it is actually pretty easy to create such buffers with libgbm and
 * libEGL. But this is beyond the scope of this document.
 *
 * So what we do is requesting a new dumb-buffer from the driver. We specify the
 * same size as the current mode that we selected for the connector.
 * Then we request the driver to prepare this buffer for memory mapping. After
 * that we perform the actual mmap() call. So we can now access the framebuffer
 * memory directly via the dev->map memory map.
 */
static struct gbm_device *gbm_device;
static struct gbm_surface *gbm_surface;
static struct gbm_bo *previous_bo = NULL;

static uint32_t previous_fb;

static EGLDisplay display;
static EGLContext context;
static EGLSurface egl_surface;
const char* vertt=
"#version 300 es\n"
"in vec4 pos;\n"
"out vec4 out_st;\n"
"void main()\n"
"{\n"
"    out_st=pos;\n"
"}\n";

const char *fragt=
"#version 300 es\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"out vec4 fc;\n"
"void main()\n"
"{\n"
"fc=vec4(0.4,0.8,0.8,0.8);\n"
"}\n";
GLfloat v=0;
GLfloat pos[3][4]=
{
{1,0.3,1.0,0},
{0,0.3,-1,0},
{0.5,0.9,1.0,0}
}
;

char delog[20000];
static GLuint
create_shader(const char *source, GLenum shader_type)
{
	GLuint shader;
	GLint status;

	shader = glCreateShader(shader_type);

	glShaderSource(shader, 1, (const char **) &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		GLsizei len;
		glGetShaderInfoLog(shader, 2000, &len, delog);
		fprintf(stdout, "Error: compiling %s: %*s\n",
			shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
			len, delog);
	}

	printf("compiled");
	return shader;
}

static int modeset_create_gbm_fb(int fd, struct modeset_dev *dev)
{
	gbm_device = gbm_create_device (device);
	display = eglGetDisplay (gbm_device);
	eglInitialize (display, NULL, NULL);
	
	// create an OpenGL context
	eglBindAPI (EGL_OPENGL_ES_API);
	EGLint attributes[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
	EGL_NONE};
	EGLConfig config;
	EGLint num_config;
	eglChooseConfig (display, attributes, &config, 1, &num_config);
	context = eglCreateContext (display, config, EGL_NO_CONTEXT, NULL);
	
	// create the GBM and EGL surface
	gbm_surface = gbm_surface_create (gbm_device, mode_info.hdisplay, mode_info.vdisplay, GBM_BO_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
	egl_surface = eglCreateWindowSurface (display, config, gbm_surface, NULL);
	if(!eglMakeCurrent (display, egl_surface, egl_surface, context)){
		fprintf(stderr, "eglMakeCurrent failed\n");
	}

	GLuint frag, vert;
	GLuint program;
	GLint status;

	vert = create_shader(vertt, GL_VERTEX_SHADER);
	frag = create_shader(fragt, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, frag);
	glAttachShader(program, vert);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		GLsizei len;
		glGetProgramInfoLog(program, 1000, &len, delog);
		fprintf(stdout, "Error: linking:\n%*s\n", len, delog);
	}

	glUseProgram(program);

	glBindAttribLocation(program, 0, "pos");
	glLinkProgram(program);
	glEnableVertexAttribArray(0);
}

static int modeset_create_fb(int fd, struct modeset_dev *dev)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	int ret;

	/* create dumb buffer */
	memset(&creq, 0, sizeof(creq));
	creq.width = dev->width;
	creq.height = dev->height;
	creq.bpp = 32;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		fprintf(stderr, "cannot create dumb buffer (%d): %m\n",
			errno);
		return -errno;
	}
	dev->stride = creq.pitch;
	dev->size = creq.size;
	dev->handle = creq.handle;

	/* create framebuffer object for the dumb-buffer */
	ret = drmModeAddFB(fd, dev->width, dev->height, 24, 32, dev->stride,
			   dev->handle, &dev->fb);
	if (ret) {
		fprintf(stderr, "cannot create framebuffer (%d): %m\n",
			errno);
		ret = -errno;
		goto err_destroy;
	}

	/* prepare buffer for memory mapping */
	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = dev->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		fprintf(stderr, "cannot map dumb buffer (%d): %m\n",
			errno);
		ret = -errno;
		goto err_fb;
	}

	/* perform actual memory mapping */
	dev->map = mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
	//mmap(NULL, 4227072, PROT_READ|PROT_WRITE, MAP_SHARED, 3, 0x100000000) = 0x7fc8908d4000
	//asm("mov $9,%rax; xor %rdi,%rdi; mov $4227072,%rsi; mov $3,%rdx; mov $1,%r10; mov $3,%r8 mov $0x100000000,%r9; syscall; mov %rax, 10+dev(%rip);");
	if (dev->map == MAP_FAILED) {
		fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n",
			errno);
		ret = -errno;
		goto err_fb;
	}

	/* clear the framebuffer to 0 */
	memset(dev->map, 0, dev->size);

	return 0;

err_fb:
	drmModeRmFB(fd, dev->fb);
err_destroy:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = dev->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	return ret;
}

/*
 * Finally! We have a connector with a suitable CRTC. We know which mode we want
 * to use and we have a framebuffer of the correct size that we can write to.
 * There is nothing special left to do. We only have to program the CRTC to
 * connect each new framebuffer to each selected connector for each combination
 * that we saved in the global modeset_list.
 * This is done with a call to drmModeSetCrtc().
 *
 * So we are ready for our main() function. First we check whether the user
 * specified a DRM device on the command line, otherwise we use the default
 * /dev/dri/card0. Then we open the device via modeset_open(). modeset_prepare()
 * prepares all connectors and we can loop over "modeset_list" and call
 * drmModeSetCrtc() on every CRTC/connector combination.
 *
 * But printing empty black pages is boring so we have another helper function
 * modeset_draw() that draws some colors into the framebuffer for 5 seconds and
 * then returns. And then we have all the cleanup functions which correctly free
 * all devices again after we used them. All these functions are described below
 * the main() function.
 *
 * As a side note: drmModeSetCrtc() actually takes a list of connectors that we
 * want to control with this CRTC. We pass only one connector, though. As
 * explained earlier, if we used multiple connectors, then all connectors would
 * have the same controlling framebuffer so the output would be cloned. This is
 * most often not what you want so we avoid explaining this feature here.
 * Furthermore, all connectors will have to run with the same mode, which is
 * also often not guaranteed. So instead, we only use one connector per CRTC.
 *
 * Before calling drmModeSetCrtc() we also save the current CRTC configuration.
 * This is used in modeset_cleanup() to restore the CRTC to the same mode as was
 * before we changed it.
 * If we don't do this, the screen will stay blank after we exit until another
 * application performs modesetting itself.
 */

int main(int argc, char **argv)
{
	int ret, fd;
	const char *card;
	struct modeset_dev *iter;

	/* check which DRM device to open */
	if (argc > 1)
		card = argv[1];
	else
		card = "/dev/dri/card0";

	fprintf(stderr, "using card '%s'\n", card);

	/* open the DRM device */
	ret = modeset_open(&fd, card);
	if (ret)
		goto out_return;

	/* prepare all connectors and CRTCs */
	ret = modeset_prepare(fd);
	if (ret)
		goto out_close;

	/* perform actual modesetting on each found connector+CRTC */
	for (iter = modeset_list; iter; iter = iter->next) {
		iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
		ret = drmModeSetCrtc(fd, iter->crtc, iter->fb, 0, 0,
				     &iter->conn, 1, &iter->mode);
		if (ret)
			fprintf(stderr, "cannot set CRTC for connector %u (%d): %m\n",
				iter->conn, errno);
	}

	/* draw some colors for 5seconds */
	modeset_draw();

	/* cleanup everything */
	modeset_cleanup(fd);

	ret = 0;

out_close:
	close(fd);
out_return:
	if (ret) {
		errno = -ret;
		fprintf(stderr, "modeset failed with error %d: %m\n", errno);
	} else {
		fprintf(stderr, "exiting\n");
	}
	return ret;
}

/*
 * A short helper function to compute a changing color value. No need to
 * understand it.
 */

static uint8_t next_color(bool *up, uint8_t cur, unsigned int mod)
{
	uint8_t next;

	next = cur + (*up ? 1 : -1) * (rand() % mod);
	if ((*up && next < cur) || (!*up && next > cur)) {
		*up = !*up;
		next = cur;
	}

	return next;
}

/*
 * modeset_draw(): This draws a solid color into all configured framebuffers.
 * Every 100ms the color changes to a slightly different color so we get some
 * kind of smoothly changing color-gradient.
 *
 * The color calculation can be ignored as it is pretty boring. So the
 * interesting stuff is iterating over "modeset_list" and then through all lines
 * and width. We then set each pixel individually to the current color.
 *
 * We do this 50 times as we sleep 100ms after each redraw round. This makes
 * 50*100ms = 5000ms = 5s so it takes about 5seconds to finish this loop.
 *
 * Please note that we draw directly into the framebuffer. This means that you
 * will see flickering as the monitor might refresh while we redraw the screen.
 * To avoid this you would need to use two framebuffers and a call to
 * drmModeSetCrtc() to switch between both buffers.
 * You can also use drmModePageFlip() to do a vsync'ed pageflip. But this is
 * beyond the scope of this document.
 */


float prop[11366][1768];

/*float biggest(unsigned int k, unsigned int j)
{
	char w1=0;
	char w2=0;
	if(prop[k][j]-prop[k+1][j]<0)
		w1++;
	if(prop[k][j+1]-prop[k+1][j+1]<0)
		w2++;
	if(prop[k+w1][j]-prop[k+w2][j+1]<0)
		return prop[k+w2][j+1];
	else
		return prop[k+w1][j+1];
}*/

static void modeset_draw(void)
{
	uint8_t r, g, b;
	//bool r_up, g_up, b_up;
	unsigned int i, j, k, off,blink,jt,kt,fede;
	int mx,my;
	struct modeset_dev *iter;
	char mouse[3];

	srand(time(NULL));
	//r = rand() % 0xff;
	//g = rand() % 0xff;
	//b = rand() % 0xff;
	//r_up = g_up = b_up = true;
	for (j = 0; j < 768; ++j) {
		for (k = 0; k < 1366; ++k) {
			prop[k][j]=rand() % 0xff;
		}
	}

	/*for (j = 4; j < 200; ++j) {
		for (k = 4; k < 200; ++k) {
			prop[k][j]=biggest(k-4,j-4);
		}
	}
	*/
	fede=open("/dev/input/mouse0",0,0);
	for (i = 0; i < 1000; ++i) {
		//r = next_color(&r_up, r, 20);
		//g = next_color(&g_up, g, 10);
		//b = next_color(&b_up, b, 5);
		read(fede,&mouse,3);
		mx=256-mouse[1];
		my=256-mouse[2];
		/*if(mouse[0]==0x08){
			mx=mouse[1];
			my=mouse[2];
		}else if(mouse[0]==0x18){
			mx=-mouse[1];
			my=mouse[2];
		}else if(mouse[0]==0x28){
			mx=mouse[1];
			my=-mouse[2];
		}else if(mouse[0]==0x38){
			mx=-mouse[1];
			my=-mouse[2];
		}*/
		/*for (j = 0; j < 200; ++j) {
			for (k = 0; k < 200; ++k) {
				if(k<4){
					kt=199+k;
				}else{
					kt=k;
				}
				if(j<4){
					jt=199+j;
				}else{
					jt=j;
				}
				prop[k][j]=biggest(kt-4,jt-4);
			}
		}*/

		for (iter = modeset_list; iter; iter = iter->next) {
			for (j = 0; j < iter->height; ++j) {
				for (k = 0; k < iter->width; ++k) {
					off = iter->stride * j + k * 4;
					//r=(iter->height*(off+i*10000)*j+i*100000)/(iter->height*iter->width)+prop[2][3];
					//g=((-(iter->width))^(off*3)/(i/2+1))*10000/(iter->height*iter->width)+prop[(iter->height/768)*4][(iter->width/1366)*4];
					//b=iter->width*(off+i*10000)/(iter->height*iter->width)+prop[(int)(iter->width/1366*4)][(int)(iter->height/768*4)];
					/*
					prop[0][0]=blink/800;
					prop[0][1]=blink/4;
					prop[0][2]=blink/2;
					prop[0][3]=blink/40;
					prop[1][0]=blink/(i+1000);
					prop[1][1]=prop[1][2]*prop[2][1]*(j*k/2);
					prop[1][2]=blink+prop[3][3]/((prop[1][3]+10)*20);
					prop[1][3]=(i-blink/10000)^(i/(1000/(i+9))+(j+k)*4500);
					prop[2][0]=(blink-i)^2;
					prop[2][1]=blink*4000*j*k;
					prop[2][2]=blink*4*i;
					prop[2][3]=blink^60/(i/10+8);
					prop[3][0]=blink*i;
					prop[3][1]=blink+i*9000;
					prop[3][2]=blink;
					prop[3][3]=prop[1][1]+prop[2][2]-prop[3][3];
*/
					r=prop[((k)*100000/1365*(1366/(mx/10)))/100000][((j)*100000/767*(768/(my/10)))/100000];
					g=prop[(k)*100000/1365*(1366/(mx/10))/100000][(j)*100000/767*(768/(my/10))/100000];
					b=prop[(int)((k)*100000/(iter->width-1)*(1366/(mx/10))/100000)][(int)((j)*100000/(iter->height-1)*(768/(my/10)))/100000];
					*(uint32_t*)&iter->map[off] =
						     (r << 16) | (g << 8) | b;
				}
			}
		}

	//	usleep(100000);
	}
}

/*
 * modeset_cleanup(fd): This cleans up all the devices we created during
 * modeset_prepare(). It resets the CRTCs to their saved states and deallocates
 * all memory.
 * It should be pretty obvious how all of this works.
 */

static void modeset_cleanup(int fd)
{
	struct modeset_dev *iter;
	struct drm_mode_destroy_dumb dreq;

	while (modeset_list) {
		/* remove from global list */
		iter = modeset_list;
		modeset_list = iter->next;

		/* restore saved CRTC configuration */
		drmModeSetCrtc(fd,
			       iter->saved_crtc->crtc_id,
			       iter->saved_crtc->buffer_id,
			       iter->saved_crtc->x,
			       iter->saved_crtc->y,
			       &iter->conn,
			       1,
			       &iter->saved_crtc->mode);
		drmModeFreeCrtc(iter->saved_crtc);

		/* unmap buffer */
		munmap(iter->map, iter->size);

		/* delete framebuffer */
		drmModeRmFB(fd, iter->fb);

		/* delete dumb buffer */
		memset(&dreq, 0, sizeof(dreq));
		dreq.handle = iter->handle;
		drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

		/* free allocated memory */
		free(iter);
	}
}

/*
 * I hope this was a short but easy overview of the DRM modesetting API. The DRM
 * API offers much more capabilities including:
 *  - double-buffering or tripple-buffering (or whatever you want)
 *  - vsync'ed page-flips
 *  - hardware-accelerated rendering (for example via OpenGL)
 *  - output cloning
 *  - graphics-clients plus authentication
 *  - DRM planes/overlays/sprites
 *  - ...
 * If you are interested in these topics, I can currently only redirect you to
 * existing implementations, including:
 *  - plymouth (which uses dumb-buffers like this example; very easy to understand)
 *  - kmscon (which uses libuterm to do this)
 *  - wayland (very sophisticated DRM renderer; hard to understand fully as it
 *             uses more complicated techniques like DRM planes)
 *  - xserver (very hard to understand as it is split across many files/projects)
 *
 * But understanding how modesetting (as described in this document) works, is
 * essential to understand all further DRM topics.
 *
 * Any feedback is welcome. Feel free to use this code freely for your own
 * documentation or projects.
 *
 *  - Hosted on http://github.com/dvdhrm/docs
 *  - Written by David Herrmann <dh.herrmann@googlemail.com>
 */
