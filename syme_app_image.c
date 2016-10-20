#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mg_common_header.h"
#include "syme_app.h"
#include "syme_vfs.h"

typedef struct {
	BITMAP bmp;

	panel_t panel;

	int screen_w;
	int screen_h;
} image_t;


static int img_file_filter(file_entry_t *fe)
{
	if (strstr(fe->name,".jpg") || strstr(fe->name,".bmp") || strstr(fe->name,"*.jpeg") 
		|| strstr(fe->name,".png") || strstr(fe->name,".gif")) {
		return 0;
	}
	
	return 1;
}

static int img_init(void *ctx, HDC hdc, HWND hwnd)
{
	
	image_t *img = (image_t *)ctx;
	
	panel_init(&img->panel);
	panel_set_cwd(&img->panel,"./res/");
	panel_dir_load(&img->panel);
	panel_filter(&img->panel,img_file_filter);

	get_screen_size(&img->screen_w,&img->screen_h);
	
	return 0;
}

static int img_draw(void *ctx, HDC hdc)
{
	int ret = 0;
	char abs_file_path[128] = {0};
	image_t *img = (image_t *)ctx;
	int x = 0 , y = 0, w = 0, h = 0;
	
	sprintf(abs_file_path,"%s%s",img->panel.cwd_vpath,img->panel.dir.list[img->panel.selected].name);
	CN_PRINT("img path:%s\n",abs_file_path);
	ret = LoadBitmapFromFile(hdc,&img->bmp,abs_file_path);
	if (0 == ret) {
		w = MIN(img->bmp.bmWidth,img->screen_w);
		h = MIN(img->bmp.bmHeight,img->screen_h);
		x = (img->screen_w - w) /2;
		y = (img->screen_h - h) / 2;
		FillBoxWithBitmap(hdc,x,y,w,h,&img->bmp);
	} else {
		TextOut(hdc,img->screen_w/2,img->screen_h/2,"Not supported format");
	}
	
	return 0;
}

static int img_key_event(void *ctx, unsigned short keycode, HWND hwnd)
{
	image_t *img = (image_t *)ctx;
	int *sel = &img->panel.selected;
	
	if (SYME_KEY_ARROW_LEFT == keycode || SYME_KEY_ARROW_RIGHT== keycode) {
		UnloadBitmap(&img->bmp);
		
		if (SYME_KEY_ARROW_LEFT == keycode) {
			*sel = (*sel + img->panel.dir.len - 1) % img->panel.dir.len ;
		} else {
			*sel = (*sel + 1) % img->panel.dir.len ;
		}

		UpdateWindow(hwnd,1);
	} else if (SYME_KEY_ESC == keycode) {
		PostMessage (hwnd, MSG_CLOSE, 0, 0);
	}
	
	return 0;
}

static int img_destory(void *ctx)
{
	image_t *img = (image_t *)ctx;

	panel_destory(&img->panel);
	UnloadBitmap(&img->bmp);
	return 0;
}


static wnd_ops_t img_wnd_ops = {
	.name = "image",
	.ctx_size = sizeof(image_t),
	.init = img_init,
	.destory = img_destory,
	.draw = img_draw,
	.key_event = img_key_event,
	.erase_background = NULL,
	.timer_event = NULL,
};

int app_image(HWND hosting)
{
	return app_create(hosting,&img_wnd_ops);
}

