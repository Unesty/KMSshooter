/*
 * Copyright (c) 2012 Arvin Schnell <arvin.schnell@gmail.com>
 * Copyright (c) 2012 Rob Clark <rob@ti.com>
 * Copyleft 2019 Unesty <unesty@protonmail.com>
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
//#include <minigbm/gbm.c>

//#define USE_EGL
#ifdef USE_EGL
#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#define USE_VULKAN

#ifdef USE_VULKAN
#include <volk.h>
#endif

#include <assert.h>

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/ioctl.h>


#define DYNAMIC_KEYS
#include <linux/input-event-codes.h>

//#define SOUND
#include <alsa/asoundlib.h>

#include <linux/kd.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifdef USE_EGL
static struct {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
	EGLSurface surface;
	uint program;
	int unis[50];
	uint vbo;
	uint positionsoffset, colorsoffset, normalsoffset;
	int unum;
} gl;
#endif

static struct {
	struct gbm_device *dev;
	struct gbm_surface *surface;
	//struct gbm_surface *cursor_surface;
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

/*
static int init_drm(void)
{
}
*/

/*
static int init_gbm(void)
{
}
*/

//char mouse[3];
//char moufd;
char mapfd;
char *keyboard;
char kfd;
char kapfd;
//float cur[2];
//float *cur;
//float position[3];

char cardpath[256];

int vnum=14*3*4+9;
char mvfd, mffd;
char mvspath[512];
char mfspath[512];
float vVertices[69000];
#ifdef USE_EGL
int init_gl(void)
{
	int major, minor, n;
	uint vertex_shader, fragment_shader;
	int ret;


	static const int context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	static const int config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};


	//load shaders
	char v_s_s[999];//faster than malloc, free
	char f_s_s[999];
	
	
	mvfd=syscall(SYS_open,mvspath,0);
	if(read(mvfd,&v_s_s,999)==-1){
		close(mvfd);
		printf("%s",mvspath);
		write(1,"ERROR: no vertex shader file\n",30);
		return 4;
	}
	close(mvfd);
	//close(3);//mvfd is 32 LOL
	mffd=syscall(SYS_open,mfspath,0);
	if(read(mffd,&f_s_s,999)==-1){
		close(mffd);
		write(1,"ERROR: no fragment shader file\n",31);
		return 4;
	}
	close(mffd);

	static const char *vertex_shader_source;

	vertex_shader_source = &v_s_s;

	static const char *fragment_shader_source;

	fragment_shader_source = &f_s_s;

	write(1,&f_s_s,91);
	write(1,&v_s_s,240);


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

	gl.unis[0] = glGetUniformLocation(gl.program, "posoffvector");
	//gl.unum++;
	//gl.unis[1] = glGetUniformLocation(gl.program, "mouse");
	gl.unis[2] = glGetUniformLocation(gl.program, "defchTex");

	glViewport(0, 0, drm.mode->hdisplay, drm.mode->vdisplay);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	gl.positionsoffset = 0;
	glGenBuffers(1, &gl.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, gl.positionsoffset, sizeof(vVertices), &vVertices[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.positionsoffset);
	glEnableVertexAttribArray(0);

	return 0;
}
#endif


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

//variables for input
char mousepath[256];
//unsigned int cursor_size[2];
char keyboardpath[256];
char kdevsize=72;
char keyoffset;
char key[20];
int ppid,mpid,kpid,spid=0;
char ttyfd;
char ttypath[10]="/dev/tty0";
char kbmode=0x01;

#ifdef SOUND
unsigned int rate=48000;
char channels;
#endif

float aspect;
float viewsize=1.0f;
char atomic_kms;
char doublebuffering;
char vsync;

char grAPI=1;

float *posoff;
//Vulkan variables
VkInstance vkinstance1;
int physdevnum[1]={1};


struct segasteon{
	int a;
	int b;
	int c;
}sas={0,0,3};

//game world
struct stats{
	int lvl;
	int health;
	int food;
	float pos[2];
};
struct item{
	int top;
	int bottom;
	int left;
	int right;
	float z;
	char state;
	void (*function)(char *arguments);
};
int itmcnt=0;
int itm;
struct item items[9999];
char menuopen=0;
void openmenu(){
	menuopen=1-menuopen;
	if(menuopen){
		vVertices[vnum]=0.2;
		vnum++;
		vVertices[vnum]=0.2;
		vnum++;
		vVertices[vnum]=0.0;
		vnum++;
		vVertices[vnum]=0.2;
		vnum++;
		vVertices[vnum]=0.8;
		vnum++;
		vVertices[vnum]=0.0;
		vnum++;
		vVertices[vnum]=0.8;
		vnum++;
		vVertices[vnum]=0.8;
		vnum++;
		vVertices[vnum]=0.0;
		vnum++;
		vVertices[vnum]=0.8;
		vnum++;
		vVertices[vnum]=0.2;
		vnum++;
		vVertices[vnum]=0.0;
		vnum++;
#ifdef USE_EGL
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), 0, GL_STATIC_DRAW);	
#endif
	}
}

//

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
						kdevsize=kdevsize*10;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '1':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=1;
						break;
					case 1:
						kdevsize=kdevsize*10+1;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+1;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '2':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=2;
						break;
					case 1:
						kdevsize=kdevsize*10+2;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+2;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '3':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=3;
						break;
					case 1:
						kdevsize=kdevsize*10+3;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+3;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '4':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=4;
						break;
					case 1:
						kdevsize=kdevsize*10+4;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+4;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '5':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=5;
						break;
					case 1:
						kdevsize=kdevsize*10+5;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+5;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '6':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=6;
						break;
					case 1:
						kdevsize=kdevsize*10+6;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+6;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '7':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=7;
						break;
					case 1:
						kdevsize=kdevsize*10+7;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+7;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '8':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=8;
						break;
					case 1:
						kdevsize=kdevsize*10+8;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+8;
						conf.op++;
						csmb++;
						break;
					}
					break;
				case '9':
					switch(csmb){
					case 0:
						csmb++;
						kdevsize=9;
						break;
					case 1:
						kdevsize=kdevsize*10+9;
						csmb++;
						break;
					case 2:
						kdevsize=kdevsize*10+9;
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

		case 4:
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
		case 5:
			switch(conf.text[symb]){
			case '0':
				grAPI=0;//OpenGL
				conf.op++;
				break;
			case '1':
				grAPI=1;//Vulkan
				conf.op++;
				break;
			}
			break;
#ifdef SOUND
		case 6:{
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
			}
		       }
		case 7:{
			switch(conf.text[symb])
			case '0':
				channels=0;
				conf.op++;
				break;
			case '1':
				channels=1;
				conf.op++;
				break;
			case '2':
				channels=2;
				conf.op++;
				break;
			case '3':
				channels=3;
				conf.op++;
				break;
			case '4':
				channels=4;
				conf.op++;
				break;
			case '5':
				channels=5;
				conf.op++;
				break;
			case '6':
				channels=6;
				conf.op++;
				break;
			case '7':
				channels=7;
				conf.op++;
				brreak;
			case '8':
				channels=8;
				conf.op++;
				break;
			case '9':
				channels=9;
				conf.op++;
				break;
			break;
		       }
#else
		case 6:
		       conf.op+=2;
#endif
		case 8:{
			switch(conf.text[symb]){
			case '/':{
				csmb=0;
//				write(1,"vs read\n",8);
				mvspath[csmb]=conf.text[symb];
				while(symb<conf.size){
					symb++;
					csmb++;
					if(conf.text[symb]=='\n'){
						conf.op++;
						printf("%s",mvspath);
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
		}
	}
	
	//parse arguments
	for(int arg=1;arg<argc;arg++){
		switch(argv[arg][0]){
		case '-':
			switch(argv[arg][1]){
			case '-':
				switch(argv[arg][2]){
					case 'h':
					switch(argv[arg][3]){
						case 'e':
							switch(argv[arg][4]){
								case 'l':
									switch(argv[arg][5]){
										case 'p':
											goto help_tip;
									}
							}
					}
				}
				break;
			case 'h':
			help_tip:
				printf("Usage:\n%s [OPTIONS]\nOptions to replace editme.conf in /etc or $HOME/config/ or ./:\n --mou [PATH]	set mouse device\n --kb [PATH]	set keyboard device\n --api [0 or 1]	0 for OpenGLES, 1 for Vulkan\n --help or -h print this text\n",argv[0]);
				return 0;
			}
		}
	}
	

	
	
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
	
	
	posoff = mmap(0,12,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,kapfd,0);
	char quit[1];
	quit[0]=0;
	//mouse process
	/*
	//mouse = mmap(0,3,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	cur = mmap(0,8,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//int ppid = getpid();
	char *quit = mmap(0,1,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
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
			drmModeMoveCursor(drm.fd,drm.crtc_id,mouse[1],mouse[2]);
			cur[0]+=(float)mouse[1];
			cur[1]-=(float)mouse[2];
			posoff[0]+=(float)mouse[1]/100;
			posoff[1]+=(float)mouse[2]/100;
			if(mouse[0]==0x09){
				write(1,&cur[0],1);
			}else if(mouse[0]==0x0c){
				quit[0]=1;
				printf("stop, it's time to stop\n");
			}
		}
	}
	*/
	//keyboard process
	//TODO get current kbdmode and current tty, probably using libXKB
	//ttyfd=open(ttypath,0,0);
	//ioctl(ttyfd,KDSKBMODE,0x04);
	//close(ttyfd);
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
				posoff[0]-=0.6;
			else if(keyboard[keyoffset]==key[1])
				posoff[0]+=0.6;
			else if(keyboard[keyoffset]==key[2])
				posoff[1]+=0.6;
			else if(keyboard[keyoffset]==key[3])
				posoff[1]-=0.6;
			//else if(keyboard[keyoffset]==key[4])
				//TODO
			//else if(keyboard[keyoffset]==key[5])
				//TODO
			else if (keyboard[44]==1 && keyboard[keyoffset]==key[6]){
				//syscall(SYS_exit,0);
				//syscall(SYS_kill,ppid,15);
				quit[0]=1;
				printf("quit");
			}
		//}
		
		if(keyboard[44]==0){
			if(keyboard[keyoffset]==key[0] || keyboard[keyoffset]==key[1])
				posoff[0]=0.0;
			if(keyboard[keyoffset]==key[2] || keyboard[keyoffset]==key[3])
				posoff[1]=0.0;
			//if(keyboard[keyoffset]==key[4] || keyboard[keyoffset]==key[5])
				//TODO
		}
		
#endif
		}
	}

	int ret;
	if(!grAPI){
#ifdef USE_EGL
	fd_set fds;
	drmEventContext evctx = {
			.version = DRM_EVENT_CONTEXT_VERSION,
			.page_flip_handler = page_flip_handler,
	};
	struct gbm_bo *bo;
	struct drm_fb *fb;
	uint32_t i = 0;

	/*
	ret = init_drm();
	if (ret) {
		printf("failed to initialize DRM\n");
		goto exit;
	}
	*/
	drmModeRes *resources;
	drmModeConnector *connector = NULL;
	drmModeEncoder *encoder = NULL;
	int fr, area;

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
	for (fr = 0; fr < resources->count_connectors; fr++) {
		connector = drmModeGetConnector(drm.fd, resources->connectors[fr]);
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
	for (fr = 0, area = 0; fr < connector->count_modes; fr++) {
		drmModeModeInfo *current_mode = &connector->modes[fr];

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
	for (fr = 0; fr < resources->count_encoders; fr++) {
		encoder = drmModeGetEncoder(drm.fd, resources->encoders[fr]);
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


	FD_ZERO(&fds);
	FD_SET(0, &fds);
	FD_SET(drm.fd, &fds);

	/*
	ret = init_gbm();
	if (ret) {
		printf("failed to initialize GBM\n");
		goto exit;
	}
	*/
	gbm.dev = gbm_create_device(drm.fd);

	gbm.surface = gbm_surface_create(gbm.dev,
			drm.mode->hdisplay, drm.mode->vdisplay,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
	if (!gbm.surface) {
		printf("failed to create gbm surface\n");
		return -1;
	}
	/*
	gbm.cursor_surface = gbm_surface_create(gbm.dev,
			64,64,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_CURSOR|GBM_BO_USE_WRITE);
	if(!gbm.cursor_surface){
		printf("failed to create gbm cursor surface\n");
		return -1;
	}
	*/

	struct gbm_bo *next_bo;
		printf("trying EGL\n");
		ret = init_gl();
		if (ret) {
			printf("failed to initialize EGL\n");
			goto exit;
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
			goto exit;
		}
		int waiting_for_flip = 1;
		/*
		struct _drmModePlane cursor{

		};
		*/
		//int cursor_bo_handle = gbm_bo_get_handle(bo).s32;
		//drmModeSetPlane(drm.fd, 0, drm.crtc_id, fb->fb_id, DRM_PLANE_TYPE_CURSOR, 0,0,drm.width,drm.height,0,0,cursor_size[0],cursor_size[1]);
		//drmModeSetCursor(drm.fd,drm.crtc_id,cursor_bo_handle,cursor_size[0],cursor_size[1]);




		aspect = (float)(drm.mode->vdisplay) / (float)(drm.mode->hdisplay);
		eglSwapBuffers(gl.display, gl.surface);
		next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		fb = drm_fb_get_from_bo(next_bo);
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
		/*
		moufd=syscall(SYS_open,mousepath,0);
		if(moufd==-1){
			syscall(1,1,"Meow! Where is the mouse?\n",26);
			goto exit;
		}
		*/
	drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform1i(gl.unis[2],0);
#define g 0.098
	while(1) {
		//syscall(SYS_read,moufd,&mouse,3);
		//drmModeMoveCursor(drm.fd,drm.crtc_id,mouse[1],mouse[2]);
		//cur[0]+=mouse[1];
		//cur[1]+=mouse[2];
		//cur[0]+=(float)mouse[1];
		//cur[1]-=(float)mouse[2];
		/*
		if(mouse[0]==0x0c){
			goto exit;
			printf("stop, it's time to stop\n");
		}
		*/
		/*
		if(mouse[0]==0x9){
				vVertices[40]+=cubeoff[0];
				glBufferSubData(GL_ARRAY_BUFFER, gl.positionsoffset, vnum, &vVertices[0]);
		}
		*/
		
		/*
		switch(mouse[0]){
		case 0x09:	
		case 0x29:
			for(btn=0;btn<btnc;btn++){
				if(cur[0]<items[btn].right && cur[0]>items[btn].left && cur[1]<items[btn].bottom && cur[1]>items[btn].top){
					items[btn].function();
				}
			}
			break;
		case 0x0a:
		case 0x1a:
			break;
		case 0x0c:
		case 0x2c:
			goto exit;
		}
		*/
		

		//i++;

		/*if(mouse[0]==12){
			break;
		}*/

		/* clear the color buffer */
		//glClearColor(0xffffff);
		//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glUniform4fv(gl.unis[0], 1, posoff);
		//glUniform2fv(gl.unis[1], 1, cur);
		//write(1,&posoff,16);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, vnum/3);

		eglSwapBuffers(gl.display, gl.surface);
		next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		fb = drm_fb_get_from_bo(next_bo);
		
		drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
		/*
		 * Here you could also update drm plane layers if you want
		 * hw composition
		 */

		
		
		//ret =
		/*
		drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,
				DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		*/
		/*
		if (ret) {
			printf("failed to queue page flip: %s\n", strerror(errno));
			return -1;
		}
		*/

/*
		waiting_for_flip = 1;
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
*/
		
		

		/* release last buffer to render on again: */
		gbm_surface_release_buffer(gbm.surface, bo);
		bo = next_bo;
	}
	eglDestroySurface (gl.display, gl.surface);
	eglDestroyContext (gl.display, gl.context);
	eglTerminate (gl.display);
	if (bo) {
		drmModeRmFB(drm.fd, fb->fb_id);
		gbm_surface_release_buffer (gbm.surface, bo);
	}
	drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0,
			&drm.connector_id, 1, drm.mode);
	//drmModeFreeCrtc (crtc);
#endif
	}else{		
#ifdef USE_VULKAN
		write(1,"trying Vulkan\n",14);
		if(ret=volkInitialize()){
			goto exit;
		}
		ret=vkCreateInstance(&(VkInstanceCreateInfo){
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pApplicationInfo = &(VkApplicationInfo) {
					.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
//					.pApplicationName = "KMSPlatformer",
					.apiVersion = VK_MAKE_VERSION(1, 1, 9),
				},
				.enabledExtensionCount = 0,
//				.ppEnabledExtensionNames = 0
			}, NULL, &vkinstance1);
		if(ret!=VK_SUCCESS){
			goto exit;
		}
		

		VkPhysicalDevice physdevs;
		ret=vkEnumeratePhysicalDevices(vkinstance1,&physdevnum[0],&physdevs);
		if(physdevnum==0){
			write(1,"vkEnumeratePhysicalDevices failed\n",33);
			goto exit;
		}
		
		VkDisplayPropertiesKHR propsDisplay;
		vkGetPhysicalDeviceDisplayPropertiesKHR(physdevs, &physdevnum[0], &propsDisplay);
		if(physdevnum[0]==0){
			write(1,"vkGetPhysicalDeviceDisplayPropertiesKHR failed\n",46);
			goto exit;
		}
		
		VkDisplayModePropertiesKHR props;
		vkGetDisplayModePropertiesKHR(physdevs, propsDisplay.display, &physdevnum[0], &props);
		
		VkDisplaySurfaceCreateInfoKHR srfinfo = {
			.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR,
			.displayMode = props.displayMode,
			.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR,
			.imageExtent = props.parameters.visibleRegion
		};
		
		VkSurfaceKHR surface;
		ret = vkCreateDisplayPlaneSurfaceKHR(vkinstance1, &srfinfo, NULL, &surface);
		if(ret){
			write(1,"vkCreateDisplayPlaneSurfaceKHR failed\n",37);
			goto exit;
		}

		const char *cdextensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};


		float priority = 1.0f;
		VkDeviceQueueCreateInfo infoQueue = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueCount = 1,
			.pQueuePriorities = &priority
		};
		VkDeviceCreateInfo dcinfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &infoQueue,
			.enabledExtensionCount = sizeof(cdextensions)/sizeof(char*),
			.ppEnabledExtensionNames = cdextensions
		};
		VkDevice device;
		ret = vkCreateDevice(physdevs, &dcinfo, NULL, &device);
		if(ret){
			fprintf(stderr, "ERROR: create_device() failed.\n");
			return ret;
		}

		volkLoadDevice(device);
		VkSurfaceCapabilitiesKHR caps;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physdevs,surface,&caps);
		VkSwapchainCreateInfoKHR swccinfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = 1,
			.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
			.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = caps.currentExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE,
		};

		VkSwapchainKHR swapchain;
		ret = vkCreateSwapchainKHR(device, &swccinfo, NULL, &swapchain);
		if(ret){
			fprintf(stderr, "ERROR: create_swapchain() failed.\n");
			return ret;
		}
		
		VkQueue queue;
		vkGetDeviceQueue(device, 0, 0, &queue);

		unsigned int imn = 1;
		VkImage image;
		vkGetSwapchainImagesKHR(device, swapchain, &imn, &image);

		VkCommandPoolCreateInfo cpcinfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		};
		VkCommandPool command_pool;
		ret = vkCreateCommandPool(device, &cpcinfo, NULL, &command_pool);
		if(ret){
			fprintf(stderr, "ERROR: create_command_pool() failed.\n");
			return ret;
		}

		VkCommandBufferAllocateInfo cbainfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = command_pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		VkCommandBuffer cmd_clear;
		vkAllocateCommandBuffers(device, &cbainfo, &cmd_clear);

		VkCommandBufferBeginInfo infoBegin = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
		};
		vkBeginCommandBuffer(cmd_clear, &infoBegin);

		VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		VkClearColorValue color = {0.8984375f, 0.8984375f, 0.9765625f, 1.0f};
		vkCmdClearColorImage(cmd_clear, image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);

		vkEndCommandBuffer(cmd_clear);

		uint32_t index;
		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &index);

		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmd_clear
		};
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &index
		};
		while(1){
/*
			next_bo = gbm_surface_lock_front_buffer(gbm.surface);
			fb = drm_fb_get_from_bo(next_bo);
			
			drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
			gbm_surface_release_buffer(gbm.surface, bo);
			bo = next_bo;
*/
			vkQueuePresentKHR(queue, &presentInfo);
		}
	vkFreeCommandBuffers(device, command_pool, 1, &cmd_clear);

	vkDestroyCommandPool(device, command_pool, NULL);
	vkDestroySwapchainKHR(device, swapchain, NULL);
	vkDestroySurfaceKHR(vkinstance1, surface, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(vkinstance1, NULL);

#endif
	}
exit:
	//ttyfd=open(ttypath,0,0);
	//ioctl(ttyfd,KDSKBMODE,kbmode);
	//close(ttyfd);
	//syscall(SYS_kill,mpid,15);
	syscall(SYS_kill,kpid,15);
#ifdef SOUND
	syscall(SYS_kill,spid,15);
#endif
	//syscall(SYS_munmap,mouse,3);
	//syscall(SYS_munmap,cur,8);
	syscall(SYS_munmap,posoff,12);


#ifdef USE_EGL
	//glDetachShader(gl.program,vertex_shader);
	//glDetachShader(gl.program,fragment_shader);
	//glDeleteShader(vertex_shader);
	//glDeleteShader(fragment_shader);
	glDeleteProgram(gl.program);
	//glDeleteBuffers(&bo);
	//eglDestroySurface (gl.display, gl.surface);
#endif
	gbm_surface_destroy (gbm.surface);
	//eglDestroyContext (gl.display, gl.context);
	//eglTerminate (gl.display);
	gbm_device_destroy (gbm.dev);
	ioctl(drm.fd, DRM_IOCTL_DROP_MASTER, 0);

	//syscall(SYS_wait4,mpid,0,0,0);
	syscall(SYS_wait4,kpid,0,0,0);
#ifdef SOUND
	syscall(SYS_wait4,spid,0,0,0);
	syscall(SYS_kill,spid,9);
#endif
	//syscall(SYS_kill,mpid,9);
	syscall(SYS_kill,kpid,9);
	write(1,"nice exit\n",10);
	return ret;
}
