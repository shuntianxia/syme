#ifndef _SYME_VFS_H
#define _SYME_VFS_H

#ifdef _cplusplus
	extern "c" {
#endif

#include<sys/types.h>
#include<sys/stat.h>

typedef struct {
	int nmlen;
	char *name;
	struct stat st;
} file_entry_t;

typedef struct {
	file_entry_t *list;
	int size; /*alloc size*/
	int len; /*in use*/
} dir_list_t;

typedef struct {
	dir_list_t dir;

	char *cwd_vpath;
	char *lwd_vpath;

	int selected;
	int dirty;
} panel_t;


typedef int (*file_filter_func)(file_entry_t *file_entry);
char *panel_get_text(void *pnl, int idx);


int panel_dir_recursive_load(dir_list_t *dl, char *dir, int depth, file_filter_func filter);


#ifdef _cplusplus
	}
#endif

#endif


