#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syme_spin_item.h"


#if 0

/*
**	txsz: the num of text arry
**	ptsz: the num of path arry
**	space: between text and image
*/
spin_item_t *spin_bim_create(char *text, int txsz, char *path, int ptsz, int space, HDC hdc)
{
	int i = 0;
	spin_item_t *sim = (spin_item_t *)malloc(sizeof(spin_item_t));
	if (!sim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	
	sim->bim.opaque = malloc(sizeof(char)*(txsz + ptsz) + sizeof(BITMAP)*ptsz);
	if (!sim->bim.opaque) {
		CN_PRINT("no mem!\n");
		return NULL;
	}

	sim->bim.text = (char *)sim->bim.opaque;
	sim->bim.img_path = sim->bim.text + txsz;
	sim->bim.pbmp = sim->bim.img_path + ptsz;

	memcpy(sim->bim.text,text,txsz);
	memcpy(sim->bim.img_path,path,ptsz);
	
	for (i=0; i<ptsz; i++) {
		LoadBitmapFromFile(hdc,&sim->bim.pbmp[i],sim->bim.img_path);
	}

	sim->idx = 0;
	sim->space = space;
	sim->spin_num = txsz;
	sim->bmp_num = ptsz;
	
	return sim;
}

spin_item_t *spin_bim_nlp_create(char *text, int txsz,  int space, PBITMAP pbmp, int bmp_num)
{
	spin_item_t *sim = (spin_item_t *)malloc(sizeof(spin_item_t));
	if (!sim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}

	sim->bim.opaque = malloc(sizeof(char) * txsz);
	if (!sim->bim.opaque) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	sim->bim.text = (char *)sim->bim.opaque;
	memcpy(sim->bim.text,text,txsz);

	sim->idx = 0;
	sim->space = space;
	sim->spin_num = txsz;
	sim->bmp_num = bmp_num;
	
	sim->bim.pbmp = pbmp;

	return sim;
}

int spin_bim_draw(spin_item_t *sim, HDC hdc, DWORD color)
{
	box_t box;
	RECT rc;

	if (sim->bim.pbmp) {
		box.x = sim->bim.text_box - sim->space;
		FillBoxWithBitmap(hdc,box.x,box.y,box.w,box.h,&sim->bim.pbmp[0]);
		box.x = sim->bim.text_box + sim->space;
		FillBoxWithBitmap(hdc,box.x,box.y,box.w,box.h,&sim->bim.pbmp[1]);
	}
	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetTextColor(hdc,color);
	rc.left = sim->bim.text_box.x;
	rc.top =  sim->bim.text_box.y;
	rc.right = rc.left + sim->bim.text_box.w;
	rc.bottom = rc.top + sim->bim.text_box.h;
	DrawText (hdc, sim->bim.text[sim->idx], -1, &rc, DT_LEFT);

	return 0;
}

int spin_bim_destory(spin_item_t *sim)
{
	free(sim->bim.opaque);
	free(sim);

	return 0;
}

PBITMAP spin_bim_get_bitmap(spin_item_t *sim)
{
	return sim->pbmp;
}

 //int (*set_text_box)(void *ctx, box_t tbx);
int spin_bim_set_text_box(spin_item_t *sim, HDC hdc,cal_box_func func,void *data)
 {
	return def_bim_set_text_box(&sim->bim,hdc,func,data);
 }
 
int spin_bim_set_img_box(spin_item_t *sim, box_t ibx)
{
	//nothing
	return 0;
}
#endif


/*
**	txsz: the num of text arry
** 	maximum number of path arrays for 2
**	space: between text and image
*/
spin_item_t *spin_bim_create(char *text[], int txsz, char *path[], int ptsz, int space, HDC hdc)
{
	int i = 0;
	char **ptr = NULL;
	
	spin_item_t *sim = (spin_item_t *)malloc(sizeof(spin_item_t));
	if (!sim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	
	ptr = (char **)malloc(sizeof(char *)*txsz + sizeof(BITMAP)*MAX_BMP_NB);
	if (!ptr) {
		CN_PRINT("no mem!\n");
		return NULL;
	}

	sim->text = ptr;
	sim->pbmp = (PBITMAP)(sim->text + txsz);

	for (i=0; i<txsz; i++) {
		sim->text[i] = strdup(text[i]);
	}

	for (i=0; i<ptsz && i<MAX_BMP_NB; i++) {
		sim->img_path[i] = strdup(path[i]);
		LoadBitmapFromFile(hdc,sim->pbmp+i,sim->img_path[i]);
	}
	
	sim->idx = 0;
	sim->space = space;
	sim->spin_num = txsz;
	//sim->bmp_num = ptsz;
	
	return sim;
}

spin_item_t *spin_bim_nlp_create(char *text[], int txsz,  int space, PBITMAP pbmp, int bmp_num)
{
	int i = 0;
	char **ptr = NULL;
	
	spin_item_t *sim = (spin_item_t *)malloc(sizeof(spin_item_t));
	if (!sim) {
		CN_PRINT("no mem!\n");
		return NULL;
	}

	ptr = (char **)malloc(sizeof(char *)*txsz + sizeof(BITMAP)*MAX_BMP_NB);
	if (!ptr) {
		CN_PRINT("no mem!\n");
		return NULL;
	}
	
	sim->text = ptr;
	sim->pbmp = (PBITMAP)(sim->text + txsz);

	for (i=0; i<txsz; i++) {
		sim->text[i] = strdup(text[i]);
	}

	memset(sim->pbmp,0,sizeof(BITMAP)*MAX_BMP_NB);
	for (i=0; i<bmp_num; i++) {
		sim->pbmp = pbmp;
	}
	
	sim->idx = 0;
	sim->space = space;
	sim->spin_num = txsz;
	//sim->bmp_num = ptsz;

	return sim;
}

int spin_bim_draw(spin_item_t *sim, HDC hdc, DWORD color)
{
	box_t box;
	RECT rc;
	SIZE size;

	box = sim->text_box;
	if (sim->pbmp) {
		box.x = sim->text_box.x - sim->space;
		FillBoxWithBitmap(hdc,box.x,box.y,box.w,box.h,&sim->pbmp[0]);
		box.x = sim->text_box.x + sim->space;
		FillBoxWithBitmap(hdc,box.x,box.y,box.w,box.h,&sim->pbmp[1]);
	}
	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetTextColor(hdc,color);
	GetTextExtent(hdc,sim->text[sim->idx],-1,&size);
	rc.left = sim->text_box.x;
	rc.top =  sim->text_box.y;
	rc.right = rc.left + size.cx;
	rc.bottom = rc.top + sim->text_box.h;
	DrawText (hdc, sim->text[sim->idx], -1, &rc, DT_CENTER);
	
	return 0;
}

int spin_bim_destory(spin_item_t *sim)
{
	int i = 0;
	for (i=0; i<sim->spin_num; i++) {
		free(sim->text[i]);
	}

	for (i=0; i<MAX_BMP_NB; i++) {
		if (sim->img_path[i] ) {
			free(sim->img_path[i] );
		}
		
		if (sim->pbmp[i].bmBits) {
			UnloadBitmap(sim->pbmp+i);
		}
	}

	free(sim->text);
	free(sim);

	return 0;
}

PBITMAP spin_bim_get_bitmap(spin_item_t *sim)
{
	return sim->pbmp;
}

 //int (*set_text_box)(void *ctx, box_t tbx);
int spin_bim_set_text_box(spin_item_t *sim, box_t box)
 {
	sim->text_box = box;
	return 0;
 }


/*

base_item_ops_t spin_bim_ops = {
	.create = spin_bim_create,
	.nlp_create = spin_bim_nlp_create,
	.draw = spin_bim_draw,
	.destory = spin_bim_destory,
	.get_bitmap = spin_bim_get_bitmap,
	.set_text_box = spin_bim_set_text_box,
	.set_img_box = spin_bim_set_img_box,
};

*/

