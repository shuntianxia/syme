#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mg_common_header.h"
#include "syme_app_set.h"
#include "syme_abstract_focus.h"
#include "syme_background.h"
#include "syme_app.h"

#include "syme_app_set_power.h"

#define SUB_ID_POWER (1)
#define SUB_ID_COMMON (2)
#define SUB_ID_VALUME (3)



extern base_item_ops_t def_bim_ops;


static int load_def_set_config(set_t *set, char *cfg)
{
	int xres = 0;
	int yres = 0;

	get_screen_size(&xres,&yres);
	
	set->sx = 50;
	set->sy = 50;

	set->hs = 0;
	set->vs = 16;
	set->rx = 40;

	set->bmp_path = "res/SET_COMMON_LOSE.png";
	set->fcs_path = "res/SET_COMMON_FOCUS.png";

	/*¾ÓÖÐ*/
	set->box.w = 118;
	set->box.h = 24;
	set->box.x = (xres -set->box.w)  / 2;
	set->box.y = (yres - set->box.h * SET_ITEM_NUM - set->vs * (SET_ITEM_NUM -1) )  / 2;
	
	return CN_OK;
}

static int cal_text_box(const box_t *ibx, box_t *tbx, SIZE size, void *data)
{
	int rx = (int)data;
	
	tbx->x = ibx->x + rx;
	tbx->y = ibx->y + ibx->h / 2 - size.cy / 2;
	tbx->w = size.cx;
	tbx->h = size.cy;
	
	return 0;
}

/*abstr focus operate*/
static void *get_effective_item(void *ctx, int idx)
{
	set_t *set = NULL;

	set = (set_t *)ctx;
	
	return (void *)(set->bim_ary[idx]);
}
static void *get_normal_item(void *ctx, int idx)
{
	set_t *set = NULL;
	set = (set_t *)ctx;
	
	return (void *)(set->bim_ary[idx]);
}

static void *get_focus_item(void *ctx,int idx)
{
	set_t *set = NULL;
	base_item_t *bim = NULL;
	
	set = (set_t *)ctx;
	bim = set->bim_ary[idx];
	
	set->bim_focus->text = bim->text;
	set->bim_focus->text_box = bim->text_box;
	set->bim_focus->img_box.x = bim->img_box.x;
	set->bim_focus->img_box.y = bim->img_box.y;
	
	return (void *)(set->bim_focus);
}
static box_t get_item_box(void *item)
{
	base_item_t *bim = NULL;
	
	bim = (base_item_t *)item;
	return bim->img_box;
}
static int draw_normal(void *ctx, void *nml_item, HDC hdc)
{
	base_item_t *bim = NULL;
	set_t *set = NULL;
	
	set = (set_t *)ctx;
	bim = (base_item_t *)nml_item;
	set->bim_ops->draw(bim,hdc,COLOR_black);
}

static int draw_focus(void *ctx, void *fcs_item, HDC hdc)
{
	base_item_t *bim = NULL;
	set_t *set = NULL;
	
	set = (set_t *)ctx;
	bim = (base_item_t *)fcs_item;
	set->bim_ops->draw(bim,hdc,COLOR_yellow);
	
	return 0;
}

static int get_next_idx(void *ctx, int direction)
{
	int idx = 0;
	set_t *set = NULL;
	
	set = (set_t *)ctx;
	idx = set->fcs.idx ;
	
	if (direction) {
		idx = (idx + 1)  %  SET_ITEM_NUM;
	} else {
		idx = (idx + SET_ITEM_NUM - 1)  %  SET_ITEM_NUM;
	}

	//idx = direction ? ((idx+1) % SET_ITEM_NUM) : ( (idx+SET_ITEM_NUM-1) % SET_ITEM_NUM);
	return idx;
}

/************************************************************************************************************/
static power_set_t ps;


static int set_init(void *ctx, HDC hdc, HWND hwnd)
{
	int i = 0;
	int size = 0;
	PBITMAP pbmp = NULL;
	set_t *set = (set_t *)ctx;
	char *text[SET_ITEM_NUM] = {"common","power","volume","file"};
	
	load_def_set_config(set,"configs/set_cfg");
	
	set->bim_ops = &def_bim_ops;
	set->bim_ary[0] = set->bim_ops->create(text[0],set->bmp_path,hdc);

	pbmp = set->bim_ops->get_bitmap(set->bim_ary[0]);
	size = ARRY_SIZE(text);
	for (i=1; i<size; i++) {
		set->bim_ary[i] = set->bim_ops->nlp_create(text[i],pbmp);
	}

	set->bim_focus = set->bim_ops->create(NULL,set->fcs_path,hdc);
	
	//set->fcs.adjust_focus_box = adjust_focus_box;
	set->fcs.get_effective_item = get_effective_item;
	set->fcs.get_focus_item = get_focus_item; 
	set->fcs.get_normal_item = get_normal_item; 
	set->fcs.draw_focus = draw_focus;
	set->fcs.draw_normal = draw_normal;
	set->fcs.get_item_box = get_item_box;
	set->fcs.get_next_idx = get_next_idx;
	set->partial_draw = 0;
	set->fcs.idx = 0;
	set->fcs.next_idx = 0;
	set->fcs.ctx = (void *)set;
	
	//power_set_init(&ps,hdc);
	//int ret = 0;
	//BITMAP bmp;
	//LoadBitmapFromFile(hdc,&bmp,"res/set_item_f.png");

	set->sub_page = 0;
	set_page_fixed_init(&set->page);
	
	return CN_OK;
}

static int set_root_draw(set_t *set, HDC hdc)
{
	int i = 0;
	box_t box;
	
	if (set->partial_draw) {
		redraw_abstr_focus(&set->fcs,hdc);
		set->partial_draw = 0;
	} else {
		box = set->box;
		set->bim_ops->set_img_box(set->bim_focus,box);
		for (i=0; i<SET_ITEM_NUM; i++) {
			set->bim_ops->set_img_box(set->bim_ary[i],box);
			set->bim_ops->set_text_box(set->bim_ary[i],hdc,cal_text_box,(void *)set->rx);
			if (set->fcs.idx == i) {
				simple_draw_abstr_focus(&set->fcs,hdc,set->fcs.idx);
			} else {
				//set->bim_ops->draw(set->bim_ary[i],hdc,COLOR_black);
				draw_normal((void *)set,set->bim_ary[i],hdc); /*just for reading*/
			}
			box.y += box.h + set->vs;
		}
	}
}

static int set_child_draw(set_t *set, HDC hdc)
{
	int ret = 0;
	if (SUB_ID_POWER == set->sub_id) {
		ret = power_set_draw(&set->ps,hdc);
	} else if (SUB_ID_COMMON == set->sub_id) {
		ret = common_set_draw(&set->cs,hdc);
	} else if (SUB_ID_VALUME == set->sub_id) {
		// todo
	}
	
	return ret;
}

/*a familiar feeling*/
static int set_draw(void *ctx, HDC hdc)
{
	set_t *set = (set_t *)ctx;
	
	if (set->sub_page) {
		set_child_draw(set,hdc);
	} else {
		set_root_draw(set,hdc);
	}

	//power_set_draw(&ps,hdc);
	return 0;
}

static int set_root_key_event(set_t *set, unsigned short keycode, HWND hwnd)
{
	if (SYME_KEY_ARROW_UP == keycode || SYME_KEY_ARROW_DOWN == keycode) {
		calculate_abstr_focus_index(&set->fcs,SYME_KEY_ARROW_UP==keycode ? 0 : 1);
		invalidate_abstr_focus(&set->fcs,hwnd);
		set->partial_draw = 1;
	}  else if (SYME_KEY_ENTER == keycode) {
		if (0 == set->fcs.idx) { //common setting
			//show_product_info(hwnd);
			//
			common_set_init(&set->cs,&set->page);
			set->sub_id = SUB_ID_COMMON;
			set->sub_page = 1;
		} else if (1 == set->fcs.idx) {
			power_set_init(&set->ps,&set->page);
			set->sub_id = SUB_ID_POWER;
			set->sub_page = 1;
		} else if (3 == set->fcs.idx) {
			app_file_mgr(hwnd);
		} else if (2 ==set->fcs.idx) {
			show_volume_dlg(hwnd);
		}
		
		UpdateWindow(hwnd,1);
	} else if (SYME_KEY_ESC == keycode) {
		PostMessage (hwnd, MSG_CLOSE, 0, 0);
	}
	
	return 0;
}

static int set_child_key_event(set_t *set, unsigned short keycode, HWND hwnd)
{
	int ret = 0;
	if (SUB_ID_POWER == set->sub_id) {
		ret = power_set_key_event(&set->ps,keycode,hwnd);
	} else if (SUB_ID_COMMON == set->sub_id) {
		ret = common_set_key_event(&set->cs,keycode,hwnd);
	} else if (SUB_ID_VALUME == set->sub_id) {
		// todo
	}

	return ret;
}

static int set_key_event(void *ctx, unsigned short keycode, HWND hwnd)
{
	set_t *set = (set_t *)ctx;

	if (set->sub_page) {
		if (SYME_KEY_ESC == keycode) { /*exit*/
			set->sub_page = 0;
			UpdateWindow(hwnd,1);
		} else { /*step into*/
			set_child_key_event(set,keycode,hwnd);
		}
	} else {
		set_root_key_event(set,keycode,hwnd);
	}
	
	//power_set_key_event(&ps,keycode,hwnd);
	return 0;
}

int set_erase_background(void *ctx, HDC hdc, HWND hwnd, RECT* clip)
{
	typedef int (*erase_func)(void *ctx, HDC hdc, HWND hwnd, RECT* clip);
	
	wnd_ops_t *parent_ops = NULL;
	erase_func func_addr = set_erase_background;
	
	wnd_ops_t *wnd_ops = (wnd_ops_t *)GetWindowAdditionalData(hwnd);
	/*child call it*/
	if (wnd_ops->erase_background != func_addr) {
		parent_ops = wnd_ops->parent_ops->parent_ops; /*need to fix bug*/
	} else {
		parent_ops = wnd_ops->parent_ops;
	}
	
	
	return parent_ops->erase_background(parent_ops->ctx,hdc,hwnd,clip); 
}

static int set_destory(void *ctx)
{
	int i = 0;
	set_t *set = (set_t *)ctx;
	
	for (i=0; i<SET_ITEM_NUM; i++) {
		set->bim_ops->destory(set->bim_ary[i]);
	}

	set_page_destory(&set->page);
}


static wnd_ops_t set_wnd_ops = {
	.name = "set",
	.ctx_size = sizeof(set_t),
	.init = set_init,
	.destory = set_destory,
	.draw = set_draw,
	.key_event = set_key_event,
	.erase_background = set_erase_background,
	.timer_event = NULL,
};


int app_set(HWND hosting)
{
	return app_create(hosting,&set_wnd_ops);
}

