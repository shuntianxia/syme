#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mg_common_header.h"
#include "syme_icon.h"
#include "cdlb_string.h"


#define ITEM_CFG_PATH CFG_PATH"item_cfg"
#define FCS_ITEM_CFG_PATH CFG_PATH"fcs_item_cfg"

#define ITEM_DFT_SIZE (30)
#define ITEM_TEXT_RATIO (3)


static void draw_item(HDC hdc, dsk_item_t *item, DWORD color)
{
	RECT rc;
	SIZE size; 

	CN_PRINT("name:%s\n",item->name);
	SelectFont(hdc,GetSystemFont (SYSLOGFONT_WCHAR_DEF));
	
	if (item->bmp.bmBits) {
		FillBoxWithBitmap(hdc,item->box.x,item->box.y,item->box.w,item->box.h,&item->bmp);
	}

	/*just copy!!!*/
	GetTextExtent(hdc, item->name, -1, &size);
	rc.top     = item->box.y + item->box.h;
	rc.left    = (item->box.x + item->box.w + item->box.x- size.cx) / 2;
	rc.bottom  = rc.top + size.cy;
	rc.right   = rc.left + size.cx;

	item->name_box.x = rc.left;
	item->name_box.y = rc.top;
	item->name_box.w = rc.right - rc.left;
	item->name_box.h = rc.bottom - rc.top;
	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetTextColor(hdc,color);
	if (LEFT_ALIGN == item->nm_align) {
		DrawText (hdc, item->name, -1, &rc, DT_LEFT);
	} else if (RIGHT_ALIGN == item->nm_align) {
		DrawText (hdc, item->name, -1, &rc, DT_RIGHT);
	} else if (CENTER_ALIGN == item->nm_align) {
		DrawText (hdc, item->name, -1, &rc, DT_CENTER);
	}
}

static void show_item(HDC hdc, dsk_item_t *item)
{
	draw_item(hdc,item,COLOR_black);
}

static int is_strip_ch(char ch)
{
	if (' ' ==ch || '\n' == ch || '\r' == ch) {
		return 1;
	}

	return 0;
}

static int load_item_config(dsk_item_t *head, int max, char *cfg)
{
	int i = 0;
	int nr = 0;
	int count = 0;
	int len = 0;
	int width = 0;
	int height = 0;
	FILE *fp = NULL;
	char *ptr = NULL;
	char *buf = NULL;
	char root_path[64] = {0};
	
	fp = fopen(cfg,"r");
	if (!fp) {
		CN_PRINT("can't open config:%s\n",cfg);
		return CN_ERR;
	}
	
	len = sizeof(buf);
	while ((-1 != getline(&buf,&len,fp)) && (nr < max)) {
		/*skip comments*/
		if ('#' == buf[0]) { 
			continue;
		}

		ptr = strip_ch(buf,is_strip_ch);
		if (strstr(ptr,"root_path")) {
			strcpy(root_path,ptr+10);
		} else if (strstr(ptr,"bmp_name")) {
			strcpy(head[nr].bmp_path,root_path);
			strcat(head[nr].bmp_path,ptr+9);
		} else if (strstr(ptr,"nr")) {
			nr = atoi(ptr+3);
			count++;
		} else if (strstr(buf,"name")) {
			strcpy(head[nr].name,ptr+5);
		} else if (strstr(buf,"item_width")) {
			width = atoi(ptr+11);
		} else if (strstr(buf,"item_height")) {
			height = atoi(ptr+12);
		}

		len = sizeof(buf);
	}

	for (i=0; i<count; i++) {
		head[i].box.w = width;
		head[i].box.h = height;
	}
	
	free(buf);
	close(fp);

	CN_PRINT("nr:%d\n",count);

	return count;
}

static int load_dsk_item_config(icon_t *icon, char *cfg)
{
	icon->item = (dsk_item_t *)malloc(ITEM_DFT_SIZE * sizeof(dsk_item_t));
	if (!icon->item) {
		CN_PRINT("no mem for dsk item \n");
		return CN_ERR;
	}
	memset(icon->item,0,ITEM_DFT_SIZE * sizeof(dsk_item_t));
	
	icon->nr = load_item_config(icon->item,ITEM_DFT_SIZE,cfg);
	
	return 0;
}

static void load_all_item_bitmap(HDC hdc, dsk_item_t *item, int num)
{
	int i = 0;
	for (i=0; i<num; i++) {
		CN_PRINT("path:%s\n",item[i].bmp_path);
		if (0 != LoadBitmapFromFile(hdc,&item[i].bmp,item[i].bmp_path)) {
			CN_PRINT("%s\n",strerror(errno));
		}
	}
}


/*focus operation*/
static box_t get_item_box(void *item, int idx)
{
	box_t box;
	
	dsk_item_t *im = (dsk_item_t *)item;

	
	//CN_PRINT("im box[x:%d y:%d w:%d h:%d]\n",im[idx].box.x,im[idx].box.y,im[idx].box.w,im[idx].box.h);
#if 0
	box.x = im[idx].box.x;
	box.y = im[idx].box.y;
	box.h = im[idx].box.h + im[idx].name_box.h;
	box.w = (im[idx].box.w + im[idx].name_box.w) / 2;

	/*adjust it */
	if (box.w < im[idx].box.w) {
		box.w = im[idx].box.w;
	}
	
	if (box.x  >  im[idx].name_box.x) {
		box.x =  im[idx].name_box.x;
	}
#endif
	
	box.x = MIN(im[idx].box.x,im[idx].name_box.x);
	box.y = im[idx].box.y;

	box.w = MAX(im[idx].box.x+im[idx].box.w,im[idx].name_box.x+im[idx].name_box.w) - box.x;
	box.h = im[idx].box.h + im[idx].name_box.h;
	
	//CN_PRINT("box [x:%d y:%d w:%d h:%d]\n",box.x,box.y,box.w,box.h);
	
	return box;
}

static int draw_nomal(HDC hdc, void *item, int idx)
{
	dsk_item_t *im = (dsk_item_t *)item;

	//CN_PRINT("name:%s idx:%d box:%p\n",im[idx].name,idx,&im[idx].box);
	
	show_item(hdc,&im[idx]);
	return 0;
}

static int get_focus_item_idx(void *item, int idx)
{
	dsk_item_t *im = (dsk_item_t *)item;
	return (int)im[idx].opaque;
}

static int draw_focus(HDC hdc, void *fcs_item, int idx)
{
	dsk_item_t *im = (dsk_item_t *)fcs_item;
	
	//CN_PRINT("name:%s idx:%d box:%p\n",im[idx].name,idx,&im[idx].box);
	
	draw_item(hdc,&im[idx],COLOR_red);
	return 0;
}

static int get_item_size()
{
	return sizeof(dsk_item_t);
}

static int adjust_focus_box(void *nml_item, void *fcs_item)
{
	box_t box;
	dsk_item_t *nml= (dsk_item_t *)nml_item;
	dsk_item_t *fcs= (dsk_item_t *)fcs_item;

	box = nml->box;
	fcs->box = box;
	
	return 0;
}

static int focus_init(icon_t *icon, HDC hdc)
{	
	icon->fcs_item = (dsk_item_t *)malloc(ITEM_DFT_SIZE * sizeof(dsk_item_t));
	if (!icon->fcs_item) {
		CN_PRINT("no mem for dsk item \n");
		return CN_ERR;
	}
	memset(icon->fcs_item,0,ITEM_DFT_SIZE * sizeof(dsk_item_t));
	
	icon->fcs_nr = load_item_config(icon->fcs_item,ITEM_DFT_SIZE,FCS_ITEM_CFG_PATH);
	load_all_item_bitmap(hdc,icon->fcs_item,icon->fcs_nr);

	icon->focus.idx = 0;
	icon->focus.next_idx = 0;
	icon->focus.nml_item = icon->item;
	icon->focus.fcs_item = icon->fcs_item;
	icon->focus.draw_focus = draw_focus;
	icon->focus.draw_normal = draw_nomal;
	icon->focus.get_focus_item_idx = get_focus_item_idx;
	icon->focus.get_item_box = get_item_box;
	icon->focus.get_item_size = get_item_size;
	icon->focus.adjust_focus_box = adjust_focus_box; 
	
	map(icon->item,icon->nr,icon->fcs_item,icon->fcs_nr);
	
	return 0;	
}

/*mapping it by name violence look*/
int map(dsk_item_t *nml_item, int nml_num, dsk_item_t *fcs_item, int fcs_num)
{
	int i = 0;
	int j = 0;
	
	for (i=0; i<nml_num; i++) {
		for (j=0; j<fcs_num; j++) {
			if (0 == strcmp(nml_item[i].name,fcs_item[j].name)) {
				nml_item[i].opaque = (void *)j;
				break;
			}
		}
	}

	return 0;
}

/***************************************************************************************************************/

int icon_init(icon_t *icon, HDC hdc)
{	
	load_dsk_item_config(icon,ITEM_CFG_PATH);
	load_all_item_bitmap(hdc,icon->item,icon->nr);

	focus_init(icon,hdc);
	
	icon->show_item = show_item;
	icon->partial_draw = 0;
	
	return 0;
}

int icon_exit(icon_t *icon)
{
	int i = 0;
	for (i=0; i<icon->nr; i++) {
		UnloadBitmap(&icon->item[i].bmp);
	}
}


