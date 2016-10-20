#ifndef _SYME_FILEMGR_H
#define _SYME_FILEMGR_H

#ifdef _cplusplus
extern "c" {
#endif

#include "mg_common_header.h"
#include "syme_vfs.h"


typedef struct {
	char *name;
	
	char *path_prefix;
	HWND path_hwnd;
	HWND list_hwnd;
	HWND info_hwnd;

	//char *bmp_path;
	//BITMAP bmp;
	char *file_bpath;
	char *folder_bpath;
	BITMAP file_bmp;
	BITMAP folder_bmp;
	
	char *lst_bmp_path;
	BITMAP lst_bmp;

	char *lst_file_bpath;
	char *lst_folder_bpath;
	BITMAP lst_file_bmp;
	BITMAP lst_folder_bmp;

	panel_t panel;
	WNDPROC old_wndproc;
} file_mgr_t;



#ifdef _cplusplus
	}
#endif

#endif

