#include "unistd.h"
#include "stdio.h"
#include "/usr/include/libdrm/drm.h"
#include "/usr/include/xf86drm.h"
#include "/usr/include/xf86drmMode.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/ioctl.h"
void _start(){
/*
putchar('\n');
printf("%s","DRM_IOCTL_GET_CAP=");
printf("%d",DRM_IOCTL_GET_CAP);
putchar('\n');
write(1, DRM_IOCTL_GET_CAP,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_GETRESOURCES");
printf("%d",DRM_IOCTL_MODE_GETRESOURCES);
putchar('\n');
write(1, DRM_IOCTL_MODE_GETRESOURCES,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_GETCONNECTOR");
printf("%d",DRM_IOCTL_MODE_GETCONNECTOR);
putchar('\n');
write(1, DRM_IOCTL_MODE_GETCONNECTOR,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_GETENCODER");
printf("%d",DRM_IOCTL_MODE_GETENCODER);
putchar('\n');
write(1, DRM_IOCTL_MODE_GETENCODER,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_CREATE_DUMB");
printf("%d",DRM_IOCTL_MODE_CREATE_DUMB);
putchar('\n');
write(1, DRM_IOCTL_MODE_CREATE_DUMB,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_ADDFB");
printf("%d",DRM_IOCTL_MODE_ADDFB);
putchar('\n');
write(1, DRM_IOCTL_MODE_ADDFB,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_MAP_DUMB");
printf("%d",DRM_IOCTL_MODE_MAP_DUMB);
putchar('\n');
write(1, DRM_IOCTL_MODE_MAP_DUMB,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_GETCRTC");
printf("%d",DRM_IOCTL_MODE_GETCRTC);
putchar('\n');
write(1, DRM_IOCTL_MODE_GETCRTC,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_SETCRTC");
printf("%d",DRM_IOCTL_MODE_SETCRTC);
putchar('\n');
write(1, DRM_IOCTL_MODE_SETCRTC,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_RMFB");
printf("%d",DRM_IOCTL_MODE_RMFB);
putchar('\n');
write(1, DRM_IOCTL_MODE_RMFB,12);
putchar('\n');
printf("%s","DRM_IOCTL_MODE_DESTROY_DUMB");
printf("%d",DRM_IOCTL_MODE_DESTROY_DUMB);
putchar('\n');
write(1, DRM_IOCTL_MODE_DESTROY_DUMB,12);
putchar('\n');
*/
struct drm_mode_create_dumb creq;
struct drm_mode_destroy_dumb dreq;
struct drm_mode_map_dumb mreq;
long unsigned int has_cap;
open("/dev/dri/card0", O_RDWR|O_CLOEXEC);
ioctl(3, DRM_IOCTL_GET_CAP, &);
ioctl(3, DRM_IOCTL_MODE_GETRESOURCES, );
ioctl(3, DRM_IOCTL_MODE_GETRESOURCES, );
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, );

/*
write(2, "using card '/dev/dri/card0'\n", 28) = 28;
openat(AT_FDCWD, "/dev/dri/card0", O_RDWR|O_CLOEXEC) = 3;
ioctl(3, DRM_IOCTL_GET_CAP, 0x7ffedbaa1460) = 0;
ioctl(3, DRM_IOCTL_MODE_GETRESOURCES, 0x7ffedbaa1400) = 0;
brk(NULL)                               = 0x55fb397a9000;
brk(0x55fb397ca000)                     = 0x55fb397ca000;
ioctl(3, DRM_IOCTL_MODE_GETRESOURCES, 0x7ffedbaa1400) = 0;
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0;
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0;
write(2, "mode for connector 64 is 1366x768"..., 34) = 34
ioctl(3, DRM_IOCTL_MODE_GETENCODER, 0x7ffedbaa13d0) = 0
ioctl(3, DRM_IOCTL_MODE_CREATE_DUMB, 0x7ffedbaa1410) = 0
ioctl(3, DRM_IOCTL_MODE_ADDFB, 0x7ffedbaa1390) = 0
ioctl(3, DRM_IOCTL_MODE_MAP_DUMB, 0x7ffedbaa1400) = 0
mmap(NULL, 4227072, PROT_READ|PROT_WRITE, MAP_SHARED, 3, 0x100000000) = 0x7fc8908d4000
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0
write(2, "ignoring unused connector 71\n", 29) = 29
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0
write(2, "ignoring unused connector 74\n", 29) = 29
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0
ioctl(3, DRM_IOCTL_MODE_GETCONNECTOR, 0x7ffedbaa13b0) = 0
write(2, "ignoring unused connector 80\n", 29) = 29
ioctl(3, DRM_IOCTL_MODE_GETCRTC, 0x7ffedbaa1450) = 0
ioctl(3, DRM_IOCTL_MODE_SETCRTC, 0x7ffedbaa1430) = 0
ioctl(3, DRM_IOCTL_MODE_SETCRTC, 0x7ffedbaa13f0) = 0
munmap(0x7fc8908d4000, 4227072)         = 0
ioctl(3, DRM_IOCTL_MODE_RMFB, 0x7ffedbaa147c) = 0
ioctl(3, DRM_IOCTL_MODE_DESTROY_DUMB, 0x7ffedbaa14ac) = 0
*/
}
