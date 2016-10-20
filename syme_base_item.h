#ifndef _SYME_BASE_ITEM_H
#define _SYME_BASE_ITEM_H

#ifdef _cplusplus
	extern "c" {
#endif

#include "mg_common_header.h"


typedef int (*cal_box_func)(const box_t *ibx, box_t *tbx, SIZE size, void *data);

typedef struct {
	char *text;
	box_t text_box;
	
	char *img_path;
	box_t img_box;
	PBITMAP pbmp;

	//align_e nm_align; /*relative to the image*/

	void *opaque;
} base_item_t;


typedef struct {
	base_item_t *(*create)(char *text, char *path, HDC hdc);
	base_item_t *(*nlp_create)(char *text, PBITMAP pbmp);
	int (*draw)(base_item_t *bim, HDC hdc, DWORD color);
	int (*destory)(base_item_t *bim);
	PBITMAP (*get_bitmap)(base_item_t *bim);
	 //int (*set_text_box)(void *ctx, box_t tbx);
	int (*set_text_box)(base_item_t *bim, HDC hdc,cal_box_func func,void *data);
	int (*set_img_box)(base_item_t *bim, box_t ibx);
} base_item_ops_t;



#ifdef _cplusplus
	}
#endif



#endif


