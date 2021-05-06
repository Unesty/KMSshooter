#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	printf("F_SETFD %d\n",F_SETFD);
	printf("FD_CLOEXEC %d\n",FD_CLOEXEC);
	printf("DRM_IOCTL_GET_CAP %d\n",DRM_IOCTL_GET_CAP);
	printf("DRM_IOCTL_MODE_GETRESOURCES %d\n",DRM_IOCTL_MODE_GETRESOURCES);
	printf("DRM_IOCTL_MODE_GETENCODER %d\n",DRM_IOCTL_MODE_GETENCODER);
	printf("DRM_IOCTL_MODE_CREATE_DUMB %d\n",DRM_IOCTL_MODE_CREATE_DUMB);
	printf("DRM_IOCTL_MODE_ADDFB %d\n",DRM_IOCTL_MODE_ADDFB);
	printf("DRM_IOCTL_MODE_MAP_DUMB %d\n",DRM_IOCTL_MODE_MAP_DUMB);
	printf("DRM_IOCTL_MODE_GETCONNECTOR %d\n",DRM_IOCTL_MODE_GETCONNECTOR);
	printf("DRM_IOCTL_MODE_GETCRTC %d\n",DRM_IOCTL_MODE_GETCRTC);
	printf("DRM_IOCTL_MODE_SETCRTC %d\n",DRM_IOCTL_MODE_SETCRTC);
}
