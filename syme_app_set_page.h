#ifndef _SYME_APP_SET_PAGE_H
#define _SYME_APP_SET_PAGE_H

#ifdef _cplusplus
extern "c" {
#endif

#if 0
#define MAX_BIM_NB (4)

typedef struct {
	int num;
	char **text;
} sel_text_t;

typedef struct {
	int bim_num;
	base_item_ops_t *bim_ops;
	base_item_t *bim[MAX_BIM_NB];
	sel_text_t *sel_text;
	box_t box;

	int space;
	int img_w;
	int img_h;
	base_item_t *img_bim[2];

	int vs;
} set_page_t;
#endif

#include "syme_abstract_focus.h"

typedef struct {
	char *name;
	char **val;
	int nb; /*the num of val*/
	int idx; /*current index*/
	int len; /*the length of the rectangle containing the string*/
	box_t box;
	
	char *prompt;

	PBITMAP lf_bmp;
	PBITMAP rt_bmp;
	PBITMAP bk_bmp;
} child_option_t;

typedef struct {
	char *title;
	int opt_num;
	child_option_t *opt;
	
	box_t bpos; /*base position*/
	int vs;
	
//	char *bk_path;
//	char *left_path;
//	char *right_path;

	BITMAP lf_bmp;
	BITMAP rt_bmp;
	BITMAP bk_bmp;

	BITMAP sys_bmp;
	BITMAP btm_bmp;
	
	abstract_focus_t fcs;

	int partial_draw;
} set_page_t;

#ifdef _cplusplus
}
#endif

#endif


