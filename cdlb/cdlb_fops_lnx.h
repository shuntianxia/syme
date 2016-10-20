#ifndef _CDLB_FOPS_LNX_H
#define _CDLB_FOPS_LNX_H

#ifdef _cplusplus
	extern "c" {
#endif

int is_dir_mount(char *path);
int get_device_mnt_path(char *key, char *mnt_path);


#ifdef _cplusplus
	}
#endif

#endif



