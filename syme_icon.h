#ifndef _SYME_ICON_H
#define _SYME_ICON_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "syme_focus.h"

#define MAX_PATH_SIZE (32)

typedef enum {
	LEFT_ALIGN,
	RIGHT_ALIGN,
	CENTER_ALIGN,
} align_e;

typedef struct {
	box_t box;
	box_t name_box;
	align_e nm_align; /*relative to the image*/
	char name[MAX_PATH_SIZE];
	char bmp_path[MAX_PATH_SIZE];
	BITMAP bmp;
	
	void *opaque;
} dsk_item_t;

typedef struct {
	int partial_draw;
	int nr;
	dsk_item_t *item;
	void (*show_item)(HDC hdc, dsk_item_t *item);

	int fcs_nr;
	dsk_item_t *fcs_item;
	focus_t focus;
} icon_t;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


