#ifndef _SYME_APP_H
#define _SYME_APP_H

#include "mg_common_header.h"

#ifdef _cplusplus
extern "c" {
#endif

typedef struct _wnd_ops {
	char *name;
	void *ctx;
	int ctx_size;
	
	int (*init)(void *ctx, HDC hdc, HWND hwnd);
	int(*draw)(void *ctx, HDC hdc);
	int (*key_event)(void *ctx, unsigned short keycode, HWND hwnd);
	int (*timer_event)(void *ctx,int id, DWORD tick_cnt, HWND hwnd);
	int (*cmd_event)(void *ctx, int id, unsigned short cmd, HWND hwnd);
	int (*erase_background)(void *ctx, HDC hdc, HWND hwnd, RECT* clip);
	int (*destory)(void *ctx);

	struct _wnd_ops *parent_ops;
} wnd_ops_t;


#ifdef _cplusplus
	}
#endif


#endif

