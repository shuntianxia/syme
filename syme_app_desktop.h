#ifndef _SYME_APP_DESKTOP_H
#define _SYME_APP_DESKTOP_H

#ifdef _cplusplus
extern "c" {
#endif

#include "syme_icon.h"
#include "syme_background.h"


#if 0
typedef struct {
	char path[64];
	BITMAP bmp;
} background_t;
#endif

typedef struct {
	icon_t icon;
	void (*show_icon)(HDC hdc, icon_t *icon);

	void *bkg_handle;
	bkg_ops_t *bkg_ops;
	//background_t bkg;
	//int (*load_bankgroud)( background_t *bkg, HDC hdc);
	//int (*show_bankgroud)(background_t *bkg, HDC hdc, HWND hwnd, RECT* clip);
	//int (*unload_bankgroud)(background_t *bkg);
	
	int screen_w; /*screen width*/
	int screen_h; /*scree height*/
	
	int sx; /*icon starting x*/
	int sy; /*icon starting y */
	
	int hs; /*horizontal space*/
	int vs; /*vertical space*/
} desktop_t;


#ifdef _cplusplus
}
#endif

#endif


