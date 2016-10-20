#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>  
#include <linux/fb.h>
#include <errno.h>
//#include <sys/mman.h> 

#include "mg_common_header.h"

static int m_cache_xres = 400;
static int m_cache_yres = 600;


int get_screen_size(int *xres, int *yres) 
{
	int fd=0;
	struct fb_var_screeninfo vinfo;  
	struct fb_fix_screeninfo finfo;

	/*no need to open the device each time for querying*/
	if (0 != m_cache_xres  || 0 != m_cache_yres) {
		*xres = m_cache_xres;
		*yres = m_cache_yres;
		
		return 0;
	}

	fd = open("/dev/fb0",O_RDWR);
	if (fd < 0) {
		CN_PRINT("can not open fb device:%s!\n",strerror(errno));
		return CN_ERR;
	}

	/*
	if (ioctl(fd,FBIOGET_FSCREENINFO,&finfo)) {
		printf("get fixed info err!\n");
		close(fd);
		return -1;
	}
	*/
	
	if (ioctl(fd,FBIOGET_VSCREENINFO,&vinfo)){  
		CN_PRINT("get var info err!\n");
		close(fd);
		return CN_ERR;
	}

	m_cache_xres = *xres = vinfo.xres;
	m_cache_yres = *yres = vinfo.yres;
	
	CN_PRINT("[%s:%d] xres:%d *yres:%d bits_per_pixel:%d\n",__FUNCTION__,__LINE__,*xres,*yres,vinfo.bits_per_pixel);
	close(fd);
	
	return CN_OK;
}  

int set_screen_32pixel()
{
	int ret = 0;
	int fd=0;
	struct fb_var_screeninfo vinfo;

	fd = open("/dev/fb0",O_RDWR);
	if (fd < 0) {
		CN_PRINT("can not open fb device:%s!\n",strerror(errno));
		return CN_ERR;
	}

	if (ioctl(fd,FBIOGET_VSCREENINFO,&vinfo)){  
		CN_PRINT("get var info err!\n");
		close(fd);
		return CN_ERR;
	}

	vinfo.bits_per_pixel = 32;
	vinfo.red.offset == 16;
	vinfo.green.offset == 8;
	vinfo.blue.offset == 0;
	
	ret = ioctl(fd,FBIOPUT_VSCREENINFO,&vinfo);
	close(fd);

	return ret;
}

#ifdef _TEST_

int main(int argc, char *argv[])
{	
	int xr,xy;
	get_screen_size(&xr,&xy);
	return 0;
}

#endif


