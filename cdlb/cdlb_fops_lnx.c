#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <mntent.h>


int is_dir_mount(char *path)
{
	int ret = 0;
	FILE *mntfile = NULL;  
	struct mntent *mntent;
	char *filename = "/proc/mounts";
	
	mntfile = setmntent(filename, "r");
	if (!mntfile) {
		printf("Failed to read mtab file, error [%s]\n",strerror(errno));
		return -1;
	}

	while(mntent = getmntent(mntfile)) {
		printf("%s, %s, %s, %s\n",mntent->mnt_dir,mntent->mnt_fsname,mntent->mnt_type,mntent->mnt_opts);
		if (0 == strcmp(mntent->mnt_dir,path)) {
			ret = 1;
			break;
		}
	}
	
	endmntent(mntfile);
	
	return ret;
}


int get_device_mnt_path(char *key, char *mnt_path)
{
	FILE *mntfile = NULL;  
	struct mntent *mntent;
	char *filename = "/proc/mounts";
	
	mntfile = setmntent(filename, "r");
	if (!mntfile) {
		printf("Failed to read mtab file, error [%s]\n",strerror(errno));
		return -1;
	}
	
	while(mntent = getmntent(mntfile)) {
		printf("%s, %s, %s, %s\n",mntent->mnt_dir,mntent->mnt_fsname,mntent->mnt_type,mntent->mnt_opts);
		if (strstr(mntent->mnt_fsname,key)) {
			strcpy(mnt_path,mntent->mnt_dir);
			break;
		}
	}
	
	endmntent(mntfile);
	
	return 0;
}



