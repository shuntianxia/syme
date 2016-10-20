#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syme_base_item.h"


static int load_bim_bmp(base_item_t *bim, char *path, HDC hdc)
{
	bim->img_path = strdup(path);
	bim->pbmp = (PBITMAP)calloc(1,sizeof(BITMAP));
	if (!bim->pbmp) {
		CN_PRINT("no mem!\n");
		free(bim->img_path);
		return CN_ERR;
	}
	LoadBitmapFromFile(hdc,bim->pbmp,bim->img_path);

	return CN_OK;
}

static base_item_t *def_bim_create(char *text, char *path, HDC hdc)
{
	int ret = 0;
	
	if (!text && !path) {
		CN_PRINT("both cannot be empty!\n");
		return NULL;
	}
	
	base_item_t *bim = (base_item_t *)calloc(1,sizeof(base_item_t));
	if (!bim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	
	if (text) {
		bim->text = strdup(text);
	}

	if (path) {
		ret = load_bim_bmp(bim,path,hdc);
		if (CN_OK != ret) {
			if (bim->text) {
				free(bim->text);
			}
			free(bim);

			return NULL;
		}
	}

	return bim;
}

#if 0
static void *def_bim_multiple_create(char *text, char *path, HDC hdc, int tx_size, int pt_size)
{
	int ret = 0;
	
	if (!text && !path) {
		CN_PRINT("both cannot be empty!\n");
		return NULL;
	}

	base_item_t *bim = (base_item_t *)malloc(sizeof(base_item_t)*MAX(tx_size,pt_size));
	if (!bim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	
	if (text) {
		bim->text = strdup(text);
	}

	if (path) {
		ret = load_bim_bmp(bim,path,hdc);
		if (CN_OK != ret) {
			if (bim->text) {
				free(bim->text);
			}
			free(bim);

			return NULL;
		}
	}
}
#endif

/*don't need to load bitmap*/
static base_item_t *def_bim_nlp_create(char *text, PBITMAP pbmp)
{
	if (!text && !pbmp) {
		CN_PRINT("both cannot be empty!\n");
		return NULL;
	}
	
	base_item_t *bim = (base_item_t *)calloc(1,sizeof(base_item_t));
	if (!bim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}

	if (text) {
		bim->text = strdup(text);
	}
	
	if (pbmp) {
		bim->pbmp = pbmp;
	}
	
	return bim;
}

static int def_bim_draw(base_item_t *bim, HDC hdc, DWORD color)
{
	RECT rc;

	if (bim->pbmp) {
		FillBoxWithBitmap(hdc,bim->img_box.x,bim->img_box.y,bim->img_box.w,bim->img_box.h,bim->pbmp);
	}
	
	if (bim->text) {
		SetBkMode(hdc, BM_TRANSPARENT);
		SetTextColor(hdc,color);
		rc.left = bim->text_box.x;
		rc.top =  bim->text_box.y;
		rc.right = rc.left + bim->text_box.w;
		rc.bottom = rc.top + bim->text_box.h;
		DrawText (hdc, bim->text, -1, &rc, DT_LEFT);
	}
#if 0
	if (LEFT_ALIGN == item->nm_align) {
		DrawText (hdc, item->name, -1, &rc, DT_LEFT);
	} else if (RIGHT_ALIGN == item->nm_align) {
		DrawText (hdc, item->name, -1, &rc, DT_RIGHT);
	} else if (CENTER_ALIGN == item->nm_align) {
		DrawText (hdc, item->name, -1, &rc, DT_CENTER);
	}
#endif

	return CN_OK;

}

static int def_bim_destory(base_item_t *bim)
{
	if (bim->text) {
		free(bim->text);
	}

	if (bim->img_path) {
		free(bim->img_path);
	}
	
	free(bim);
	
	return CN_OK;
}

static PBITMAP def_bim_get_bitmap(base_item_t *bim)
{
	return bim->pbmp;
}

#if 0
static int def_bim_set_text_box(void *ctx, box_t tbx)
{
	base_item_t *bim = (base_item_t *)ctx;
	bim->text_box = tbx;
	
	return 0;
}
#endif
static int def_bim_set_text_box(base_item_t *bim, HDC hdc, cal_box_func func, void *data)
{
	SIZE size;
	
	GetTextExtent(hdc, bim->text, -1, &size);
	return func(&bim->img_box,&bim->text_box,size,data);
}

static int def_bim_set_img_box(base_item_t *bim, box_t ibx)
{
	bim->img_box = ibx;
}

base_item_ops_t def_bim_ops = {
	.create = def_bim_create,
	.nlp_create = def_bim_nlp_create,
	.draw = def_bim_draw,
	.destory = def_bim_destory,
	.get_bitmap = def_bim_get_bitmap,
	.set_text_box = def_bim_set_text_box,
	.set_img_box = def_bim_set_img_box,
};


