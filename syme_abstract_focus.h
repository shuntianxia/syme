#ifndef _SYME_ABSTRACT_FOCUS_H
#define _SYME_ABSTRACT_FOCUS_H

#ifdef _cplusplus
extern "c" {
#endif

#include "mg_common_header.h"

typedef struct {
	int idx;
	int next_idx;
	void *ctx;
	
	void *(*get_effective_item)(void *ctx, int idx); /*when normal and focus vary in size, requires greater*/
	void *(*get_normal_item)(void *ctx, int idx);
	void *(*get_focus_item)(void *ctx,int idx);
	int (*draw_normal)(void *ctx, void *nml_item, HDC hdc);
	int (*draw_focus)(void *ctx, void *fcs_item, HDC hdc);
	int (*get_next_idx)(void *ctx, int direction);

	box_t (*get_item_box)(void *item);
	//int (*adjust_focus_box)(void *nml_item, void *fcs_item);
} abstract_focus_t;


int invalidate_abstr_focus(abstract_focus_t *focus, HWND hwnd);
int simple_draw_abstr_focus(abstract_focus_t *focus, HDC hdc, int idx);
int redraw_abstr_focus(abstract_focus_t *focus, HDC hdc);


#ifdef _cplusplus
}
#endif

#endif



