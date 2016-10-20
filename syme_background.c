#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "syme_background.h"

/*will be static function!!!*/
int background_init(background_t *bkg, char *path,HDC hdc)
{
	bkg->bmp_path = strdup(path);
	if (LoadBitmapFromFile(hdc,&bkg->bmp,bkg->bmp_path)) {
		CN_PRINT("%s\n",strerror(errno));
	}
	
	return 0;
}

int background_paint(background_t *bkg, HDC hdc,HWND hwnd, RECT* clip)
{
	BOOL fGetDC = FALSE;
	RECT rc;
	
	if (hdc == 0) {
		hdc = GetClientDC (hwnd);
		fGetDC = TRUE;
	}

	if (clip) {
		rc = *clip;
		ScreenToClient (hwnd, &rc.left, &rc.top);
		ScreenToClient (hwnd, &rc.right, &rc.bottom);
		IncludeClipRect (hdc, &rc);
	} else {
		GetClientRect (hwnd, &rc);
	}

	FillBoxWithBitmap (hdc, 0, 0, RECTW(rc), RECTH(rc), &bkg->bmp);

	if (fGetDC) {
		ReleaseDC (hdc);
	}
	
	return 0;
}

int background_destory(background_t *bkg)
{
	UnloadBitmap(&bkg->bmp);
	if (bkg->bmp_path) {
		free(bkg->bmp_path);
	}
}

void *def_bkg_init(char *path, HDC hdc)
{
	background_t *bkg = (background_t *)malloc(sizeof(background_t));
	if (!bkg) {
		CN_PRINT("no mem!\n");
		return NULL;
	}

	background_init(bkg,path,hdc);
	
	return (void *)bkg;
}

int def_bkg_paint(void *ctx, HDC hdc,HWND hwnd, RECT* clip)
{
	background_t *bkg = (background_t *)ctx;
	return background_paint(bkg,hdc,hwnd,clip);
}

int def_bkg_destory(void *ctx)
{
	background_t *bkg = (background_t *)ctx;
	background_destory(bkg);
	free(bkg);
	
	return 0;
}


/**********************************************************************************************************************/

bkg_ops_t def_bkg_ops = {
	.init =def_bkg_init ,
	.paint = def_bkg_paint,
	.destory = def_bkg_destory,
};


