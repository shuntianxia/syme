#ifndef _SYME_FOCUS_H
#define _SYME_FOCUS_H

#ifdef _cplusplus
extern "c" {
#endif

typedef struct {
	//HWND hwnd;
	int idx; /*current focus index*/
	int next_idx; /*next focus index*/
	
	void *nml_item; /*normal member*/
	box_t (*get_item_box)(void *nml_item, int idx);
	int (*draw_normal)(HDC hdc, void *nml_item, int idx);
	int (*get_focus_item_idx)(void *nml_item, int idx);

	void *fcs_item; /*focus member*/
	int (*draw_focus)(HDC hdc, void *fcs_item, int idx);

	int (*get_item_size)();
	int (*adjust_focus_box)(void *nml_item, void *fcs_item);
	
	//int (*init)();
	//int (*map)(void *nml_item, void *fcs_item);
} focus_t;

int invalidate_focus(focus_t *focus, HWND hwnd);
int simple_draw_focus(focus_t *focus, HDC hdc, int idx);
int redraw_focus(focus_t *focus, HDC hdc);


#ifdef _cplusplus
}
#endif

#endif

