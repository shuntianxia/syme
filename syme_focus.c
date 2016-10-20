#include "mg_common_header.h"
#include "syme_focus.h"


static int get_rect(box_t box, RECT *rc)
{
	rc->left = box.x;
	rc->top = box.y;
	rc->right = box.x + box.w;
	rc->bottom = box.y + box.h;
	
	return 0;
}

static int get_next_idx(int idx, int nr, int grow_direct)
{
	if (grow_direct) {
		idx = (idx + 1)  %  nr;
	} else {
		idx = (idx + nr - 1)  %  nr;
	}

	//idx = grow_direct ? ((idx+1) % nr) : ( (idx+nr-1) % nr);
	return idx;
}


/****************************************************************************************************************/

#if 0
int focus_init(focus_t *focus)
{
	int ret = 0;
	ret = focus->init();
	ret |= focus->map(focus->nml_item,focus->fcs_item);
	
	return ret;
}
#endif

int calculate_focus_index(focus_t *focus, int nr, int grow_direct)
{
	focus->next_idx = get_next_idx(focus->idx,nr,grow_direct);
	
	return 0;
}

int invalidate_focus(focus_t *focus, HWND hwnd)
{
	RECT rc;
	box_t box;

	box = focus->get_item_box(focus->nml_item,focus->idx);
	get_rect(box,&rc);
	InvalidateRect(hwnd, &rc,TRUE);

	box = focus->get_item_box(focus->nml_item,focus->next_idx);
	get_rect(box,&rc);
	InvalidateRect(hwnd, &rc,TRUE);
	
	return 0;	
}

int simple_draw_focus(focus_t *focus, HDC hdc, int idx)
{
	int im_size = 0;
	int fcs_idx = 0;
	
	fcs_idx = focus->get_focus_item_idx(focus->nml_item,idx);
	im_size = focus->get_item_size();
	focus->adjust_focus_box(focus->nml_item + im_size * idx,focus->fcs_item+ im_size * fcs_idx);
	focus->draw_focus(hdc,focus->fcs_item,fcs_idx);

	return 0;
}

int redraw_focus(focus_t *focus, HDC hdc)
{
	int im_size = 0;
	int fcs_idx = 0;
	
	focus->draw_normal(hdc,focus->nml_item,focus->idx);

	simple_draw_focus(focus,hdc,focus->next_idx);
#if 0
	fcs_idx = focus->get_focus_item_idx(focus->nml_item,focus->next_idx);
	im_size = focus->get_item_size();
	focus->adjust_focus_box(focus->nml_item + im_size * focus->next_idx,focus->fcs_item+ im_size * fcs_idx);
	focus->draw_focus(hdc,focus->fcs_item,fcs_idx);
#endif
	
	focus->idx = focus->next_idx;
	
	return 0;
}



