#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "syme_vfs.h"
#include "mg_common_header.h"
#include "cdlb_sort.h"

#define MAX_LIST_SIZE (128)

static int vfs_opendir(char *name)
{
	DIR *dir = opendir(name);
	if (!dir) {
		CN_PRINT("err:%s path:%s\n",strerror(errno),name);
		return CN_ERR;
	}

	return (int)dir;
}

static struct dirent *vfs_readdir(int h)
{
	return readdir((DIR *)h);
}

static int vfs_closedir(int h)
{
	return closedir((DIR *)h);	
}

static int vfs_stat(char *path, struct stat *buf)
{
	return stat(path,buf);
}


static void *file_entry_get_elem(void *arry, int offset)
{
	file_entry_t *list = arry;
	return (void *)(list+offset);
}

static int file_entry_get_key(void *elem)
{
	file_entry_t *entry = elem;
	return (int)entry->name;
}

static int file_entry_compare(int key1, int key2)
{
	return strcmp((char *)key1, (char *)key2);
}

static void file_entry_swap(void *a, void *b)
{
	file_entry_t entry;
	entry = *(file_entry_t *)a;
	*(file_entry_t *)a = *(file_entry_t *)b;
	*(file_entry_t *)b = entry;
}

static char *new_file_path(char *dir, char *name)
{
	char *fmt = NULL;
	char *str = NULL;
	
	str = (char *)malloc(strlen(dir) + strlen(name) + 2);
	if (!str) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	
	if ('/' != dir+strlen(dir)) {
		fmt = "%s/%s";
	} else {
		fmt = "%s%s";
	}
	sprintf(str,fmt,dir,name);
	
	return str;
}


/*****************************************************************************/

int panel_init(panel_t *panel)
{
	memset(panel,0,sizeof(panel_t));
	
	panel->dir.list = (file_entry_t *) calloc(1,sizeof(file_entry_t) * MAX_LIST_SIZE);
	if (!panel->dir.list) {
		CN_PRINT("no mem!\n");
		return CN_ERR;
	}
	panel->dir.size = MAX_LIST_SIZE;
	panel->dir.len = 0;
	
	return 0;
}

int panel_destory(panel_t *panel)
{
	if (panel->dir.list) {
		free(panel->dir.list);
		panel->dir.list = NULL;
	}

	if (panel->cwd_vpath) {
		free(panel->cwd_vpath);
		panel->cwd_vpath = NULL;
	}
	
	return 0;
}

int panel_set_cwd(panel_t *panel, char *path)
{
	if (panel->lwd_vpath) {
		free(panel->lwd_vpath);
	}
	panel->lwd_vpath = panel->cwd_vpath;
	panel->cwd_vpath = strdup(path);

	return 0;
}

#if 0
struct dirent
{
	long d_ino; /* inode number 索引节点号 */
	off_t d_off; /* offset to this dirent 在目录文件中的偏移 */
	unsigned short d_reclen;/* length of this d_name 文件名长 */
	unsigned char d_type; /* the type of d_name 文件类型 */
	char d_name [NAME_MAX+1];/* file name (null-terminated) 文件名，最长255字符 */
}
#endif
int panel_dir_load(panel_t *panel)
{
	struct dirent * dirent = NULL;
	int handle = 0;
	int i = 0;

	if (!panel->cwd_vpath) {
		CN_PRINT("null cwd_vpath\n");
		return CN_ERR;
	}
	
	panel_dir_clear(panel);
	handle = vfs_opendir(panel->cwd_vpath);
	if (CN_ERR == handle) {
		return CN_ERR;
	}

	while (dirent = vfs_readdir(handle)) {
		panel->dir.list[i].name = strdup(dirent->d_name);
		panel->dir.list[i].nmlen = dirent->d_reclen;
		vfs_stat(dirent->d_name,&panel->dir.list[i].st);
		
		CN_PRINT("%s\n",panel->dir.list[i].name);
		
		i++;
		if (i >= panel->dir.size) {
			break; //will be fixed!!!
		}
	}
	panel->dir.len = i;
	panel->dirty  = 1;

	vfs_closedir(handle);
	
	quick_sort_t qs;
	qs.arry = panel->dir.list;
	qs.compare = file_entry_compare;
	qs.get_key = file_entry_get_key;
	qs.swap = file_entry_swap;
	qs.get_elem = file_entry_get_elem;
	quick_sort(&qs,0,panel->dir.len-1);
	
	return 0;
}

char *panel_get_text_ex(panel_t *panel, int idx)
{
	if (idx > panel->dir.len) {
		CN_PRINT("out of len!\n");
		return NULL;
	}
	
	return panel->dir.list[idx].name;
}

char *panel_get_text(void *pnl, int idx)
{
	panel_t *panel = (panel_t *)pnl;
	return panel_get_text_ex(panel,idx);
}

int panel_dir_clear(panel_t *panel)
{
	if (panel->dir.list) {
		memset(panel->dir.list,0,panel->dir.size * sizeof(file_entry_t));
	}
	panel->selected = 0;
}

int panel_filter(panel_t *panel, file_filter_func filter)
{
	int i = 0;

	file_entry_t *ptr = NULL;
	file_entry_t *pre = NULL;
	
	pre = ptr = panel->dir.list;
	while (ptr && pre && (ptr < panel->dir.list+panel->dir.len)) {
		if (filter(ptr)) {
			ptr ++;
			continue;
		}
		
		*pre++ = *ptr++;
		i++;
	}

	panel->dir.len = i;
	
	return 0;
}



/*
*
*	depth : maximum number of recursive (-1 infinity)
*	filter	: filter files
*
*/
int panel_dir_recursive_load(dir_list_t *dl, char *dir, int depth, file_filter_func filter)
{
#define D_TYPE_DIR (4)
#define D_TYPE_FILE (8)

	int *pos = &dl->len;
	int handle = 0;
	struct dirent * dirent = NULL;
	
	handle = vfs_opendir(dir);
	if (CN_ERR == handle) {
		return CN_ERR;
	}

	while (dirent = vfs_readdir(handle)) {
		if (*pos >= dl->size) {
			break; //will be fixed!!!
		}
		
		if (D_TYPE_FILE == dirent->d_type) {
			dl->list[*pos].name = new_file_path(dir, dirent->d_name);
			dl->list[*pos].nmlen = dirent->d_reclen;
			vfs_stat(dl->list[*pos].name,&dl->list[*pos].st);

			if (filter && filter(&dl->list[*pos])) {
				free(dl->list[*pos].name);
				dl->list[*pos].name = NULL;
				continue;
			}
			CN_PRINT("file:%s\n",dl->list[*pos].name);
			
			(*pos)++;
		} else if ('.' != dirent->d_name[0] && D_TYPE_DIR == dirent->d_type) {
			if ((depth <= 0) && (-1 != depth)) {
				continue;
			}
			
			char *path = new_file_path(dir, dirent->d_name);
			//CN_PRINT("path:%s\n",path);
			panel_dir_recursive_load(dl,path,(-1 == depth) ? depth : (depth-1),filter);
			free(path);
		}
		
		//CN_PRINT("%s\n",panel->dir.list[i].name);
	}
	
	vfs_closedir(handle);
	
	return 0;	
}


