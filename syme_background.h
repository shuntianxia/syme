#ifndef _SYME_BACKGROUND_H
#define _SYME_BACKGROUND_H

#include "mg_common_header.h"

#ifdef _cplusplus
	extern "c" {
#endif

typedef struct {
	char *bmp_path;
	BITMAP bmp;
} background_t;

/*background_t maybe contains the member of hdc*/
typedef struct {
	void *(*init)(char *path, HDC hdc);
	int (*paint)(void *ctx,HDC hdc,HWND hwnd, RECT* clip);
	int (*destory)(void *ctx);
} bkg_ops_t;

int background_init(background_t *bkg, char *path,HDC hdc);
int background_paint(background_t *bkg, HDC hdc,HWND hwnd, RECT* clip);
int background_destory(background_t *bkg);


#ifdef _cplusplus
	}
#endif

#endif

