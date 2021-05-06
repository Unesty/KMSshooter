#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
//#include <libdrm/drm.h>
//#include <libdrm/drm_mode.h>

#include <gbm.h>

#define GL_GLEXT_PROTOTYPES 1
//#include <GLES3/gl32.h>
//#include <GLES2/gl2ext.h>
#include <GL/glcorearb.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <assert.h>

#define PI 3.1415926535897932384626433832795
#include <math.h>

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define DYNAMIC_KEYS
#include <linux/input-event-codes.h>

//#define SOUND
#include <alsa/asoundlib.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

//char mouse[3];
char *mouse;
char moufd;
char mapfd;
char *keyboard;
char kfd;
char kapfd;
//float cur[2]={90,-90};
float *cur;
//float position[3];

char cardpath[256];

static struct {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
	EGLSurface surface;
	unsigned int program;
	unsigned int mouse;
	unsigned int time;
	unsigned int resolution;
	unsigned int textures[5];
	unsigned int vbo;
	unsigned int positionsoffset, colorsoffset, normalsoffset;
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

int vnum=14*3*4+9;
char mvfd, mffd;
char mvspath[512];
char mfspath[512];
static GLfloat vVertices[] = {
};

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
int ppid,mpid,kpid,spid=0;
#ifdef SOUND
unsigned int rate=48000;
char channels;
#endif

float aspect;
float viewsize=1.0f;
unsigned int grAPI;
char grAPIversion[2];

struct segasteon{
	int a;
	int b;
	int c;
}sas={0,0,3};

//network

//

//game stack
//float deck[52][4]={{0.7,0.2,770,11},{0,0,0,12},{0,0,30,13},{0,0,0,14},{0,0,0,15},{0,0.3,0,16},{0,0,0,17},{0.8,0.4,0,18},{0.23,0.11,0,19},{0.3,0,0,110},{0,0,0,111},{0,0,0,112},{0,0,0,113},{0,0,0,21},{0,0,0,22},{0,0,0,23},{0,0,0,24},{0,0,0,25},{0,0,0,26},{0,0,0,27},{0,0,0,28},{0,0,0,29},{0,0,0,210},{0,0,0,211},{0,0,0,212},{0,0,0,213},{0,0,0,31},{0,0,0,32},{0,0,0,33},{0,0,0,34},{0,0,0,35},{0,0,0,36},{0,0,0,37},{0,0,0,38},{0,0,0,39},{0,0,0,310},{0,0,0,311},{0,0,0,312},{0,0,0,313},{0,0,0,41},{0,0,0,42},{0,0,0,43},{0,0,0,44},{0,0,0,45},{0,0,0,46},{0,0,0,47},{0,0,0,48},{0,0,0,49},{0,0,0,410},{0,0,0,411},{0,0,0,412},{0.050,0.1,0,413}};

#define csx 5.1
#define csz 10.2
float deck[52*6][4]={{0.7-csx,0.2-csz,770,11},{0.7+csx,0.2-csz,770,11},{0.7-csx,0.2+csz,770,11},{0.7+csx,0.2-csz,770,11},{0.7-csx,0.2+csz,770,11},{0.7+csx,0.2+csz,770,11},};
unsigned int udeck;
char top=0;
float card[24];
char hand[52];
float playerspos[8][2]={{500,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
unsigned int uplayerpos;
char holding=-1;
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
			switch(conf.text[symb]){
			case '0':
				grAPI=8;//OpenGL
				conf.op++;
				break;
			case '1':
				grAPI=1;//OpenGL ES
				conf.op++;
				break;
			case '2':
				grAPI=4;//OpenGL ES 2
				conf.op++;
				break;
			case '3':
				grAPI=64;//OpenGL ES 3
				conf.op++;
				break;
			case '4':
				grAPI=0;//Vulkan
				conf.op++;
				break;
			}
			break;
		case 5:
			if(conf.text[symb]>='0'&&conf.text[symb]<='9'){
				csmb=0;
				while(csmb==0){
					switch(conf.text[symb]){
					case '0':
						grAPIversion[0]=0;
						csmb++;
						break;
					case '1':
						grAPIversion[0]=1;
						csmb++;
						break;
					case '2':
						grAPIversion[0]=2;
						csmb++;
						break;
					case '3':
						grAPIversion[0]=3;
						csmb++;
						break;
					case '4':
						grAPIversion[0]=4;
						csmb++;
						break;
					case '5':
						grAPIversion[0]=5;
						csmb++;
						break;
					case '6':
						grAPIversion[0]=6;
						csmb++;
						break;
					case '7':
						grAPIversion[0]=7;
						csmb++;
						break;
					case '8':
						grAPIversion[0]=8;
						csmb++;
						break;
					case '9':
						grAPIversion[0]=9;
						csmb++;
						break;
					}
					symb++;
				}
				while(csmb==1){
					switch(conf.text[symb]){
					case '0':
						grAPIversion[1]=0;
						csmb++;
						break;
					case '1':
						grAPIversion[1]=1;
						csmb++;
						break;
					case '2':
						grAPIversion[1]=2;
						csmb++;
						break;
					case '3':
						grAPIversion[1]=3;
						csmb++;
						break;
					case '4':
						grAPIversion[1]=4;
						csmb++;
						break;
					case '5':
						grAPIversion[1]=5;
						csmb++;
						break;
					case '6':
						grAPIversion[1]=6;
						csmb++;
						break;
					case '7':
						grAPIversion[1]=7;
						csmb++;
						break;
					case '8':
						grAPIversion[1]=8;
						csmb++;
						break;
					case '9':
						grAPIversion[1]=9;
						csmb++;
						break;
					}
					symb++;
				}
				conf.op++;
			}
			break;

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
				write(1,"vs load\n",8);
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
	int rate, channels;//, seconds;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	char* buff;
	int buff_size, loops;

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
	/*
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
	*/

}
#endif
	
	fd_set fds;
	drmEventContext evctx = {
			.version = DRM_EVENT_CONTEXT_VERSION,
			.page_flip_handler = page_flip_handler,
	};
	//char fbstick=0;

	uint32_t i = 0;
	int ret=0;

	drmModeRes *resources;
	drmModeConnector *connector = NULL;
	drmModeEncoder *encoder = NULL;
	int area;

	drm.fd = open(cardpath, O_RDWR);

	if (drm.fd < 0) {
		printf("could not open drm device\n");
		//return -1;
		goto exit;
	}

	resources = drmModeGetResources(drm.fd);
	if (!resources) {
		printf("drmModeGetResources failed: %s\n", strerror(errno));
		//return -1;
		goto exit;
	}

	/* find a connected connector: */
	char conn_count=0;
	for (i = 0; i < resources->count_connectors; i++) {
		connector = drmModeGetConnector(drm.fd, resources->connectors[i]);
		if (connector->connection == DRM_MODE_CONNECTED) {
			conn_count++;
			break;
		}
		drmModeFreeConnector(connector);
		connector = NULL;
	}
/*
	drmModeConnector **connected = malloc(conn_count*sizeof(drmModeConnector));
	for (int i=0,j=0; i<resources->count_connectors; i++) {
		drmModeConnector *conn = drmModeGetConnector(drm.fd,
		resources->connectors[i]);
		if (conn->connection == DRM_MODE_CONNECTED) {
			connected[j] = conn;
			j++;
		} else
			drmModeFreeConnector(conn);
	}
	drmModeFreeResources(resources);
	int conn_n;
	if (conn_count > 1) {
		printf("Found multiple displays:\n");
		for (int i=0; i<conn_count; i++) {
			printf("(%d) [%d]\n", i, connected[i]->connector_id);
		}
		printf("Choose one: ");
		scanf("%d", &conn_n);
	} else {
		conn_n = 0;
	}
	connector = connected[conn_n];
	//drmModeEncoder *enc = drmModeGetEncoder(drm.fd, connected[conn_n]->encoder_id);
	//drmModeCrtc *crtc = drmModeGetCrtc(drm.fd, enc->crtc_id);
	//drmModeFreeEncoder(enc);
	//drmModeCrtc *crtc = get_crtc_from_conn(drm.fd, connected[conn_n]);

	*/
	if (conn_count==0) {
		/* we could be fancy and listen for hotplug events and wait for
		 * a connector..
		 */
		printf("no connected connector!\n");
		goto exit;
		//return -1;
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
		//return -1;
		goto exit;
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
		uint32_t crtc_id;// = find_crtc_for_connector(resources, connector);

		for (i = 0; i < connector->count_encoders; i++) {
			const uint32_t encoder_id = connector->encoders[i];
			drmModeEncoder *encoder = drmModeGetEncoder(drm.fd, encoder_id);

			if (encoder) {
				const uint32_t crtc_id=-1;


				for (i = 0; i < resources->count_crtcs; i++) {
					// possible_crtcs is a bitmask as described here:
					// https://dvdhrm.wordpress.com/2012/09/13/linux-drm-mode-setting-api
					//
					const uint32_t crtc_mask = 1 << i;
					const uint32_t crtc_id = resources->crtcs[i];
					if (encoder->possible_crtcs==0 || crtc_mask==0) {
						printf("failed to initialize DRM\n");
						goto exit;
					}
				}

				drmModeFreeEncoder(encoder);
				/*
				if (crtc_id != 0) {
					return crtc_id;
				}
				*/
			}
		}

		// no match found

		if (crtc_id == 0) {
			printf("no crtc found!\n");
			printf("failed to initialize DRM\n");
			goto exit;
		}

		drm.crtc_id = crtc_id;
	}

	drm.connector_id = connector->connector_id;

	//drm initialized

	FD_ZERO(&fds);
	FD_SET(0, &fds);
	FD_SET(drm.fd, &fds);

	gbm.dev = gbm_create_device(drm.fd);

	gbm.surface = gbm_surface_create(gbm.dev,
			drm.mode->hdisplay, drm.mode->vdisplay,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
	if (!gbm.surface) {
		printf("failed to create gbm surface\n");
		goto exit;
	}

	EGLint major, minor, n;
	GLuint vertex_shader, fragment_shader;


	float vColors[] = {
	};

	float vNormals[] = {
	};
	
	float vTexCoord[52*2] = {
	};



	//load shaders
	char v_s_s[999999];//faster than malloc, free
	char f_s_s[999999];
	
	
	mvfd=syscall(SYS_open,mvspath,0);
	if(read(mvfd,&v_s_s,999999)==-1){
		close(mvfd);
		printf("%s",mvspath);
		write(1,"ERROR: no vertex shader file\n",30);
		return 4;
	}
	close(mvfd);
	//close(3);//mvfd is 32 LOL
	mffd=syscall(SYS_open,mfspath,0);
	if(read(mffd,&f_s_s,999999)==-1){
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

	if (!eglBindAPI(12450*(grAPI==8)+12448*((grAPI==1)+(grAPI==4)+(grAPI==64)))) { //OpenGL=12450  OpenGLES=12448
		printf("failed to bind api %d \n",grAPI);
		return -1;
	}

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, grAPI,
		EGL_NONE
	};
	//printf("EGL_OPENGL_API is %d\n",EGL_OPENGL_API);
	//printf("EGL_OPENGL_ES_API is %d\n",EGL_OPENGL_ES_API);
	//printf("EGL_OPENGL_BIT is %d\n",EGL_OPENGL_BIT);
	//printf("EGL_OPENGL_ES_BIT is %d\n",EGL_OPENGL_ES_BIT);
	//printf("EGL_OPENGL_ES2_BIT is %d\n",EGL_OPENGL_ES2_BIT);
	//printf("EGL_OPENGL_ES3_BIT is %d\n",EGL_OPENGL_ES3_BIT);
	if (!eglChooseConfig(gl.display, config_attribs, &gl.config, 1, &n) || n != 1) {
		printf("failed to choose config: %d\n", n);
		return -1;
	}

	EGLint context_attribs[] = {
		EGL_CONTEXT_MAJOR_VERSION_KHR, grAPIversion[0],
		EGL_CONTEXT_MINOR_VERSION_KHR, grAPIversion[1],
		EGL_NONE
	};
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

	eglSurfaceAttrib(gl.display, gl.surface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
	eglSurfaceAttrib(gl.display, gl.surface, EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER);


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

	//glBindAttribLocation(gl.program, 0, "in_position");

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

	

	gl.mouse = glGetUniformLocation(gl.program, "iMouse");
	gl.time = glGetUniformLocation(gl.program, "iTime");
	gl.resolution = glGetUniformLocation(gl.program, "iResolution");
	//udeck=glGetUniformLocation(gl.program,"deck");

	//gl.textures[0] = glGetUniformLocation(gl.program, "iChannel0");
	//glUniform1i = (gl.textures[0],0);
	
	//glGenTextures(1,gl.textures);
	//unsigned int twidth,theight,tnrChannels;
	//unsigned char* texture_data = stbi_load("textures/wn.jpg",&twidth,&theight,&tnrChannels,0);
	/*
	unsigned char* texture_data;
	for(unsigned int tw=0; tw<twidth; tw++){
		for(unsigned int th=0; th<theight; th++){
			texture_data[tw*theight*tnrChannels+th*tnrChannels]=
		}
	}
	*/

/*
	glBindTexture(GL_TEXTURE_2D, gl.textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	stbi_image_free(texture_data);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
*/

	//glBindSampler(0, gl.);

	glViewport(0, 0, drm.mode->hdisplay, drm.mode->vdisplay);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);

	//gl.positionsoffset = 0;
	//gl.colorsoffset = sizeof(vVertices);
	//gl.normalsoffset = sizeof(vVertices) + sizeof(vColors);
	glGenBuffers(1, &gl.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(deck), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(deck), &deck[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, gl.colorsoffset, sizeof(vColors), &vColors[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, gl.normalsoffset, sizeof(vNormals), &vNormals[0]);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.positionsoffset);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.normalsoffset);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)gl.colorsoffset);
	//glEnableVertexAttribArray(2);
	
	//unsigned int vao;
	//glGenVertexArrays(1,&vao);
	//glBindVertexArray(vao);

	//	printf("failed to initialize EGL\n");

	/* clear the color buffer */
	//glClearColor(0.5, 0.5, 0.5, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(gl.display, gl.surface);
	struct gbm_bo *bo;
	bo = gbm_surface_lock_front_buffer(gbm.surface);
	struct drm_fb *fb;
	//struct drm_fb *fb = gbm_bo_get_user_data(bo);
	fb = drm_fb_get_from_bo(bo);
	//struct drm_fb *fbs[2];
	//fbs[0]=fb;

	uint32_t width, height, stride, handle;

	if (!fb){
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
		}

		gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);

	}

	/* set mode: */
	write(1,"\nllol\n",6);
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
		fbs[1]=fb;

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
	mouse = mmap(0,3,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//posoff = mmap(0,12,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,kapfd,0);
	cur = mmap(0,8,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//modelview = mmap(0,16*4,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//int ppid = getpid();
	//char *quit = mmap(0,1,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,mapfd,0);
	//quit[0]=0;
	mpid=syscall(SYS_clone,0,0);
	if(mpid==0){
		moufd=syscall(SYS_open,mousepath,0);
		if(moufd==-1){
			syscall(SYS_munmap,cur,3);
			syscall(1,1,"Meow! Where is the mouse?\n",26);
			return 0;
		}
		close(0);
		close(1);
		close(2);
		close(3);
		close(4);
		//#ifdef __X86_64
		asm("movq $5,%rdi; mov mouse(%rip),%rsi; mov $3,%rdx;");
		//#elif
		//#endif
		mouse_loop:
			//#ifdef __X86_64
			asm("xor %rax,%rax; syscall; movq mouse(%rip), %r8; movq cur(%rip), %r9; pxor %xmm0, %xmm0; pxor %xmm1, %xmm1; movsbl 1(%r8), %ecx; cvtsi2ssl %ecx, %xmm0; addss (%r9), %xmm0; movss %xmm0, (%r9); movsbl 2(%r8), %r8d; movss 4(%r9), %xmm0; cvtsi2ssl %r8d, %xmm1; subss %xmm1, %xmm0; movss %xmm0, 4(%r9)");
			//#elif
			//read(moufd,mouse,3);
			//cur[0]+=mouse[1];
			//cur[1]-=mouse[2];
			
			//if(mouse[0]==0x09){
				//write(1,&cur[0],1);
			//}
			//else if(mouse[0]==0x0c){
			//	quit[0]=1;
				//syscall(SYS_kill,15,ppid);
			//	return 0;
			//}
			//#endif
			
		goto mouse_loop;
	}
	//keyboard process
	/*
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
			break;
			  }
		case 0x1d:{
			break;
			  }
		case 0x1c:{
			break;
			  }
		case 0x23:{
			break;
			  }
		case 0x24:{
			break;
			  }
		case 0x15:{
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
			else if(keyboard[keyoffset]==key[1])
			else if(keyboard[keyoffset]==key[2])
			else if(keyboard[keyoffset]==key[3])
			else if(keyboard[keyoffset]==key[4])
			else if(keyboard[keyoffset]==key[5])
			else if (keyboard[44]==1 && keyboard[keyoffset]==key[6]){
				//syscall(SYS_exit,0);
				//syscall(SYS_kill,ppid,15);
				quit[0]=1;
				printf("quit");
			}
		//}
		
		if(keyboard[44]==0){
			if(keyboard[keyoffset]==key[0] || keyboard[keyoffset]==key[1])
			if(keyboard[keyoffset]==key[2] || keyboard[keyoffset]==key[3])
			if(keyboard[keyoffset]==key[4] || keyboard[keyoffset]==key[5])
		}
		
#endif
		}
	}
*/



	aspect = (GLfloat)(drm.mode->vdisplay) / (GLfloat)(drm.mode->hdisplay);
		//eglSwapBuffers(gl.display, gl.surface);
		//next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		//fb = drm_fb_get_from_bo(next_bo);
		//fbs[1]=fb;

		//ret = drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id, DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		//if (ret) {
		//	printf("failed to queue page flip: %s\n", strerror(errno));
		//	return -1;
		//}

	/*
		moufd=syscall(SYS_open,mousepath,0);
		if(moufd==-1){
			syscall(1,1,"Meow! Where is the mouse?\n",26);
			goto exit;
		}
		*/
	//drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
	//glClear(GL_COLOR_BUFFER_BIT);
		write(1,"\nlol2\n",6);
	//unsigned long stm=0;
	gbm_surface_release_buffer(gbm.surface, bo);
	//struct timeval time;
	//gettimeofday(time,NULL);
	float time=0;
	float resolution[2]={drm.mode->vdisplay, drm.mode->hdisplay};
	glUniform2fv(gl.resolution, 1, resolution);
	
	//game init
	int bhj=0;
	//char cvt=0;
	for(bhj=0; bhj<52*6;bhj+=6){
		deck[bhj][0]=-csx+.5;
		deck[bhj][1]=-csz+.5;
		deck[bhj][2]=0;
		deck[bhj][3]=(float)(((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*100+bhj-((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*13*6);
		deck[bhj+1][0]=+csx+.5;
		deck[bhj+1][1]=-csz+.5;
		deck[bhj+1][2]=0;
		deck[bhj+1][3]=(float)(((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*100+bhj-((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*13*6);
		deck[bhj+2][0]=-csx+.5;
		deck[bhj+2][1]=+csz+.5;
		deck[bhj+2][2]=0;
		deck[bhj+2][3]=(float)(((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*100+bhj-((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*13*6);
		deck[bhj+3][0]=+csx+.5;
		deck[bhj+3][1]=-csz+.5;
		deck[bhj+3][2]=0;
		deck[bhj+3][3]=(float)(((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*100+bhj-((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*13*6);
		deck[bhj+4][0]=-csx+.5;
		deck[bhj+4][1]=+csz+.5;
		deck[bhj+4][2]=0;
		deck[bhj+4][3]=(float)(((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*100+bhj-((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*13*6);
		deck[bhj+4][0]=+csx+.5;
		deck[bhj+4][1]=+csz+.5;
		deck[bhj+4][2]=0;
		deck[bhj+4][3]=(float)(((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*100+bhj-((bhj>12*6)+(bhj>25*6)+(bhj>38*6))*13*6);
	}
	printf("start loop\n");
	/*
	int deck2[52];
	char rndnum;
#define g 9.8
shuffle:
	for(bhj=0; bhj<52;bhj++){
		deck2[bhj]=deck[bhj][3];
		deck[bhj][3]=0;
	}
	for(bhj=0; bhj<52;bhj++){
		rndnum=random(top);
		if(rndnum==top){
			top--;
		}
		while(deck2[rndnum]==0){
			rndnum++;
			if(rndnum>top){
				rndnum=0;
			}
		}
		deck[bhj][3]=deck2[rndnum];
		deck2[rndnum]=0;
	}
gather:
	for(bhj=0; bhj<52;bhj++){
		deck[bhj][0]=700;
		deck[bhj][1]=700;
		deck[bhj][2]=0;
	}
	*/
	top=51;	

	close(0);
	close(1);
	close(2);
glloop:
		//read(moufd,&mouse,3);
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
			//glBufferSubData(GL_ARRAY_BUFFER, gl.positionsoffset, vnum, &vVertices[0]);
		}
		*/
/*
		switch(mouse[0]){
		case 0x09:
		case 0x19:
		case 0x29:
		case 0x39:
			stm+=10000;
			break;
		case 0x0a:
		case 0x1a:
		case 0x2a:
		case 0x3a:
			stm-=10000*(stm>=10000);
			stm*=(stm>10000);
			break;
		case 0x0c:
		case 0x1c:
		case 0x2c:
		case 0x3c:
			goto exit;
		}
*/
		

		//i++;

		/*if(mouse[0]==12){
			break;
		}*/

		/* clear the color buffer */
		//glClearColor(0,0,1,0);
		//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//float aspect = (GLfloat)(drm.mode->vdisplay) / (GLfloat)(drm.mode->hdisplay);
		
		//game loop
		if(holding==-1){
			switch(mouse[0]){
			case 0x09:
			case 0x19:
			case 0x29:
			case 0x39:
				for(holding=top*6;holding>=0;holding-=6){
					if(cur[0]<deck[holding][0]+40 && cur[0]>deck[holding][0]-40 && cur[1]<deck[holding][1]+80 && cur[1]>deck[holding][1]-80){
						card[0]=deck[holding-5][0];
						card[1]=deck[holding-5][1];
						card[2]=deck[holding-5][2];
						card[3]=deck[holding-5][3];
						card[4]=deck[holding-4][0];
						card[5]=deck[holding-4][1];
						card[6]=deck[holding-4][2];
						card[7]=deck[holding-4][3];
						card[8]=deck[holding-3][0];
						card[9]=deck[holding-3][1];
						card[10]=deck[holding-3][2];
						card[11]=deck[holding-3][3];
						card[12]=deck[holding-2][0];
						card[13]=deck[holding-2][1];
						card[14]=deck[holding-2][2];
						card[15]=deck[holding-2][3];
						card[16]=deck[holding-1][0];
						card[17]=deck[holding-1][1];
						card[18]=deck[holding-1][2];
						card[19]=deck[holding-1][3];
						card[20]=deck[holding][0];
						card[21]=deck[holding][1];
						card[22]=deck[holding][2];
						card[23]=deck[holding][3];
						for(char jhn=holding;jhn<top;jhn++){
							deck[jhn-5][0]=deck[jhn+1][0];
							deck[jhn-5][1]=deck[jhn+1][1];
							deck[jhn-5][2]=deck[jhn+1][2];
							deck[jhn-5][3]=deck[jhn+1][3];
							deck[jhn-4][0]=deck[jhn+2][0];
							deck[jhn-4][1]=deck[jhn+2][1];
							deck[jhn-4][2]=deck[jhn+2][2];
							deck[jhn-4][3]=deck[jhn+2][3];
							deck[jhn-3][0]=deck[jhn+3][0];
							deck[jhn-3][1]=deck[jhn+3][1];
							deck[jhn-3][2]=deck[jhn+3][2];
							deck[jhn-3][3]=deck[jhn+3][3];
							deck[jhn-2][0]=deck[jhn+4][0];
							deck[jhn-2][1]=deck[jhn+4][1];
							deck[jhn-2][2]=deck[jhn+4][2];
							deck[jhn-2][3]=deck[jhn+4][3];
							deck[jhn-1][0]=deck[jhn+5][0];
							deck[jhn-1][1]=deck[jhn+5][1];
							deck[jhn-1][2]=deck[jhn+5][2];
							deck[jhn-1][3]=deck[jhn+5][3];
							deck[jhn][0]=deck[jhn+6][0];
							deck[jhn][1]=deck[jhn+6][1];
							deck[jhn][2]=deck[jhn+6][2];
							deck[jhn][3]=deck[jhn+6][3];
						}
						deck[top*6-5][0]=card[0];
						deck[top*6-5][1]=card[1];
						deck[top*6-5][2]=card[2];
						deck[top*6-5][3]=card[3];
						deck[top*6-4][0]=card[4];
						deck[top*6-4][1]=card[5];
						deck[top*6-4][2]=card[6];
						deck[top*6-4][3]=card[7];
						deck[top*6-3][0]=card[8];
						deck[top*6-3][1]=card[9];
						deck[top*6-3][2]=card[10];
						deck[top*6-3][3]=card[11];
						deck[top*6-2][0]=card[12];
						deck[top*6-2][1]=card[13];
						deck[top*6-2][2]=card[14];
						deck[top*6-2][3]=card[15];
						deck[top*6-1][0]=card[16];
						deck[top*6-1][1]=card[17];
						deck[top*6-1][2]=card[18];
						deck[top*6-1][3]=card[19];
						deck[top*6][0]=card[20];
						deck[top*6][1]=card[21];
						deck[top*6][2]=card[22];
						deck[top*6][3]=card[23];
						break;
					}
				}
			}
		}else{	
			switch(mouse[0]){
			case 0x09:
			case 0x19:
			case 0x29:
			case 0x39:
				deck[top*6-5][0]=cur[0]-csx;
				deck[top*6-5][1]=cur[1]-csz;
				deck[top*6-4][0]=cur[0]+csx;
				deck[top*6-4][1]=cur[1]-csz;
				deck[top*6-3][0]=cur[0]-csx;
				deck[top*6-3][1]=cur[1]+csz;
				deck[top*6-2][0]=cur[0]+csx;
				deck[top*6-2][1]=cur[1]-csz;
				deck[top*6-1][0]=cur[0]-csx;
				deck[top*6-1][1]=cur[1]+csz;
				deck[top*6][0]=cur[0]+csx;
				deck[top*6][1]=cur[1]+csz;
				glBufferSubData(GL_ARRAY_BUFFER, 0, 52*6, &deck[0]);
				break;
			case 0x00:
			case 0x08:
			case 0x18:
			case 0x28:
			case 0x38:
				holding=-1;
			}
		}
		//
		//glUniform4iv(udeck,52,deck);
		time+=0.01;
		glUniform2fv(gl.mouse, 1, cur);
		glUniform1f(gl.time, time);

		glDrawArrays(GL_TRIANGLES, 0, 52*6);

		eglSwapBuffers(gl.display, gl.surface);
		//next_bo = gbm_surface_lock_front_buffer(gbm.surface);
		//fb = drm_fb_get_from_bo(next_bo);
		
		//drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,0,0);
//				DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		//fbstick=1-fbstick;
		//usleep(1000);
		/*
		 * Here you could also update drm plane layers if you want
		 * hw composition
		 */

		
		//ret =
		//drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id, DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
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
		//		break;
			}
			drmHandleEvent(drm.fd, &evctx);
		}
		*/
		
		

		/* release last buffer to render on again: */
		//gbm_surface_release_buffer(gbm.surface, bo);
		//bo = next_bo;
	goto glloop;
exit:
	syscall(SYS_kill,mpid,15);
	syscall(SYS_kill,kpid,15);
#ifdef SOUND
	syscall(SYS_kill,spid,15);
#endif
	syscall(SYS_munmap,mouse,3);
	syscall(SYS_munmap,cur,8);
	drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0,
			&drm.connector_id, 1, drm.mode);
	//drmModeFreeCrtc(drm.crtc_id);

	if (bo) {
		drmModeRmFB(drm.fd, fb->fb_id);
		gbm_surface_release_buffer (gbm.surface, bo);
	}

	//glDetachShader(gl.program,vertex_shader);
	//glDetachShader(gl.program,fragment_shader);
	//glDeleteShader(vertex_shader);
	//glDeleteShader(fragment_shader);
	glDeleteProgram(gl.program);
	//glDeleteBuffers(&bo);
	eglDestroySurface (gl.display, gl.surface);
	gbm_surface_destroy (gbm.surface);
	eglDestroyContext (gl.display, gl.context);
	eglTerminate (gl.display);
	gbm_device_destroy (gbm.dev);
	ioctl(drm.fd, DRM_IOCTL_DROP_MASTER, 0);
	close(drm.fd);

	syscall(SYS_wait4,mpid,0,0,0);
	//syscall(SYS_wait4,kpid,0,0,0);
#ifdef SOUND
	syscall(SYS_wait4,spid,0,0,0);
	syscall(SYS_kill,spid,1);
#endif
	syscall(SYS_kill,mpid,1);
	//syscall(SYS_kill,kpid,1);
	write(1,"nice exit\n",10);
	return ret;
}
