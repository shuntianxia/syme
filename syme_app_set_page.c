#include "mg_common_header.h"
#include "syme_base_item.h"
#include "syme_app_set_page.h"


#define RT_MARGIN (20)
#define LF_MARGIN (20)

#define PROMPT_SPACE (300)


static int draw_child_opt(child_option_t *opt,HDC hdc, DWORD color)
{
	RECT rc;
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	int rt_margin = RT_MARGIN;
	int lf_margin = LF_MARGIN;
		
	if (opt->bk_bmp) {
		x = opt->box.x;
		y = opt->box.y;
		w = opt->box.w;
		h = opt->box.h;
		FillBoxWithBitmap(hdc,x,y,w,h,opt->bk_bmp);
	}

	if (opt->nb > 1) {
		if (opt->lf_bmp) {
			w = opt->lf_bmp->bmWidth;
			h = opt->lf_bmp->bmHeight;
			x = opt->box.x + opt->box.w - opt->len - rt_margin;
			y = opt->box.y + (opt->box.h - h) / 2;
			FillBoxWithBitmap(hdc,x,y,w,h,opt->lf_bmp);
		}
	}
	
	if (opt->rt_bmp) {
		w = opt->rt_bmp->bmWidth;
		h = opt->rt_bmp->bmHeight;
		x = opt->box.x + opt->box.w - w - rt_margin;
		y = opt->box.y + (opt->box.h - h) / 2;
		FillBoxWithBitmap(hdc,x,y,w,h,opt->rt_bmp);
	}
	
	SetBkMode (hdc, BM_TRANSPARENT);
	SetTextColor(hdc,color);
	rc.left = opt->box.x + lf_margin;
	rc.top = opt->box.y;
	rc.right = opt->box.x + opt->box.w;
	rc.bottom = rc.top + opt->box.h;
	DrawText(hdc,opt->name,-1,&rc,DT_SINGLELINE |DT_LEFT | DT_VCENTER);
	
	if (opt->val) {
		rc.left = opt->box.x + opt->box.w - opt->len - rt_margin;
		rc.top = opt->box.y;
		rc.right = opt->box.x + opt->box.w  - rt_margin;
		rc.bottom = rc.top + opt->box.h;
		DrawText(hdc,opt->val[opt->idx],-1,&rc,DT_SINGLELINE |DT_CENTER | DT_VCENTER);
	}
	
	return 0;
}


/*when normal and focus vary in size, requires greater*/
static void *get_effective_item(void *ctx, int idx)
{
	set_page_t *page = (set_page_t *)ctx;
	return (void *)(&page->opt[idx]);
}
static void *get_normal_item(void *ctx, int idx)
{
	set_page_t *page = (set_page_t *)ctx;
	return (void *)(&page->opt[idx]);
}
static void *get_focus_item(void *ctx,int idx)
{
	set_page_t *page = (set_page_t *)ctx;
	return (void *)(&page->opt[idx]);
}

static int draw_normal(void *ctx, void *nml_item, HDC hdc)
{
	child_option_t *opt = (child_option_t *)nml_item;
	set_page_t *page = (set_page_t *)ctx;

	opt->bk_bmp = NULL;
	opt->lf_bmp = NULL;
	opt->rt_bmp = NULL;
	draw_child_opt(opt,hdc,COLOR_black);

	return 0;
}
static int draw_focus(void *ctx, void *fcs_item, HDC hdc)
{
	child_option_t *opt = (child_option_t *)fcs_item;
	set_page_t *page = (set_page_t *)ctx;

	opt->bk_bmp = &page->bk_bmp;
	opt->lf_bmp = &page->lf_bmp;
	opt->rt_bmp = &page->rt_bmp;

	draw_child_opt(opt,hdc,COLOR_yellow);
	return 0;
}

static int get_next_idx(void *ctx, int direction)
{
	int nb = 0;
	int idx = 0;
	set_page_t *page = (set_page_t *)ctx;
	idx = page->fcs.idx ;
	
	if (direction) {
		idx = (idx + 1)  %  page->opt_num;
	} else {
		idx = (idx + page->opt_num - 1)  %  page->opt_num;
	}
	
	return idx;
}

static box_t get_item_box(void *item)
{
	child_option_t *opt = (child_option_t *)item;
	return opt->box;
}


int set_page_var_init(set_page_t *page, child_option_t *opt, int num)
{
	page->opt = opt;
	page->opt_num = num;

	page->partial_draw = 0;
	page->fcs.idx = 0;
	page->fcs.next_idx = 0;
	
	//set_page_fixed_init(page);
	
	return 0;
}

int set_page_fixed_init(set_page_t *page)
{
	int screen_w = 0;
	int screen_h = 0;
	get_screen_size(&screen_w,&screen_h);
	
	/*put it here just for a while*/
	page->vs = 20;
	page->bpos.w = 300;
	page->bpos.h = 24;
	page->bpos.x = (screen_w -page->bpos.w) / 2;
	page->bpos.y = 15;
	LoadBitmapFromFile(HDC_SCREEN,&page->bk_bmp,"res/set_item_f.png");
	LoadBitmapFromFile(HDC_SCREEN,&page->lf_bmp,"res/set_butt_lf.png");
	LoadBitmapFromFile(HDC_SCREEN,&page->rt_bmp,"res/set_butt_rf.png");
	LoadBitmapFromFile(HDC_SCREEN,&page->sys_bmp,"res/set_bsysset.png");
	LoadBitmapFromFile(HDC_SCREEN,&page->btm_bmp,"res/set_bottom.png");
	
	page->fcs.get_effective_item = get_effective_item;
	page->fcs.get_focus_item = get_focus_item; 
	page->fcs.get_normal_item = get_normal_item; 
	page->fcs.draw_focus = draw_focus;
	page->fcs.draw_normal = draw_normal;
	page->fcs.get_item_box = get_item_box;
	page->fcs.get_next_idx = get_next_idx;
	page->fcs.ctx = (void *)page;
	
	return 0;
}


int set_page_destory(set_page_t *page)
{
	UnloadBitmap(&page->bk_bmp);
	UnloadBitmap(&page->lf_bmp);
	UnloadBitmap(&page->rt_bmp);
	UnloadBitmap(&page->sys_bmp);
	UnloadBitmap(&page->btm_bmp);
	
	return 0;
}

int set_page_draw(set_page_t *page, HDC hdc)
{
	int i = 0;
	box_t box =  page->bpos;
	SIZE size;
	int screen_w = 0;
	int screen_h = 0;
	get_screen_size(&screen_w,&screen_h);

	if (page->partial_draw) {
		redraw_abstr_focus(&page->fcs,hdc);
		page->partial_draw = 0;
	} else {
		for (i=0; i<page->opt_num; i++) {
			box.y += i * page->vs + box.h;
			page->opt[i].box = box;
			page->opt[i].len = 100;
			if (0 == i) {
				simple_draw_abstr_focus(&page->fcs,hdc,0);
			} else {
				draw_normal((void *)page,&page->opt[i],hdc); /*just for reading*/
			}
		}
	}

	FillBoxWithBitmap(hdc,25,page->bpos.y+ page->bpos.h,73,73,&page->sys_bmp);
	FillBoxWithBitmap(hdc,0,screen_h-37,screen_w,40,&page->btm_bmp);
	
	if (page->opt[page->fcs.idx].prompt) {
		SetTextColor(hdc,COLOR_yellow);
		TextOut(hdc,page->bpos.x+LF_MARGIN,page->bpos.y + PROMPT_SPACE,page->opt[page->fcs.idx].prompt);
	}

	if (page->title) {
		GetTextExtent(hdc,page->title,-1,&size);
		TextOut(hdc,(screen_w - size.cx)/2,screen_h-40 + size.cy/2 ,page->title);
	}
	
	return 0;
}

int set_page_key_event(set_page_t *page, unsigned short keycode, HWND hwnd)
{
	RECT rc;
	
	if (SYME_KEY_ARROW_UP == keycode || SYME_KEY_ARROW_DOWN == keycode) {
		calculate_abstr_focus_index(&page->fcs,SYME_KEY_ARROW_UP==keycode ? 0 : 1);
		invalidate_abstr_focus(&page->fcs,hwnd);
		page->partial_draw = 1;
	} else if (SYME_KEY_ARROW_LEFT == keycode || SYME_KEY_ARROW_RIGHT == keycode) {
		child_option_t *opt = &page->opt[page->fcs.idx];
		if (SYME_KEY_ARROW_RIGHT) {
			opt->idx = (opt->idx + 1)  %  opt->nb;
		} else {
			opt->idx = (opt->idx + opt->nb - 1)  %  opt->nb;
		}
		
		//invalidate_abstr_focus(&page->fcs,hwnd);
		page->partial_draw = 1;
		rc.left = opt->box.x;
		rc.top = opt->box.y;
		rc.right = rc.left + opt->box.w;
		rc.bottom = rc.top + opt->box.h;
		InvalidateRect(hwnd,&rc,1);
		page->partial_draw = 1;
	}

	rc.left = page->bpos.x + LF_MARGIN;
	rc.top = page->bpos.y +  PROMPT_SPACE;
	rc.right = rc.left +page->bpos.w;
	rc.bottom = rc.top + page->bpos.h;
	InvalidateRect(hwnd,&rc,1);
	
	return 0;	
}


