#include "syme_abstract_focus.h"


static int get_rect(box_t box, RECT *rc)
{
	rc->left = box.x;
	rc->top = box.y;
	rc->right = box.x + box.w;
	rc->bottom = box.y + box.h;
	
	return 0;
}

int calculate_abstr_focus_index(abstract_focus_t *focus, int direction)
{
	focus->next_idx = focus->get_next_idx(focus->ctx,direction);
	
	return 0;
}


int invalidate_abstr_focus(abstract_focus_t *focus, HWND hwnd)
{
	RECT rc;
	box_t box;
	void *item = NULL;
	
	//CN_PRINT("in\n");

	item = focus->get_effective_item(focus->ctx,focus->idx); 
	box = focus->get_item_box(item);
	get_rect(box,&rc);
	InvalidateRect(hwnd, &rc,TRUE);

	item = focus->get_effective_item(focus->ctx,focus->next_idx);
	box = focus->get_item_box(item);
	get_rect(box,&rc);
	InvalidateRect(hwnd, &rc,TRUE);

	//CN_PRINT("out\n");
	
	return 0;	
}

int simple_draw_abstr_focus(abstract_focus_t *focus, HDC hdc, int idx)
{
	//void *item = NULL;
	void *fcs_item = NULL;
	
	//CN_PRINT("in\n");
	//item = focus->get_focus_item(focus->ctx,idx);
	fcs_item = focus->get_focus_item(focus->ctx,idx);
	//focus->adjust_focus_box(item,fcs_item);
	focus->draw_focus(focus->ctx,fcs_item,hdc);

	//CN_PRINT("out\n");
	
	return 0;
}

int redraw_abstr_focus(abstract_focus_t *focus, HDC hdc)
{
	void *item = NULL;

	//CN_PRINT("in\n");
	item = focus->get_normal_item(focus->ctx,focus->idx);
	focus->draw_normal(focus->ctx,item,hdc);
	
	simple_draw_abstr_focus(focus,hdc,focus->next_idx);
	focus->idx = focus->next_idx;

	//CN_PRINT("out\n");
	
	return 0;
}




