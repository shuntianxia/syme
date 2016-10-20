#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mg_common_header.h"
#include "syme_app_desktop.h"
#include "syme_app.h"
#include "syme_vfs.h"

#define DKP_CFG_PATH CFG_PATH"desktop_cfg"

extern bkg_ops_t def_bkg_ops;


int audio_file_filter(file_entry_t *fe);


static void load_default_config(desktop_t *dkp, char *cfg)
{
	int xres = 0;
	int yres = 0;
	
	dkp->sx = 30;
	dkp->sy = 30;
	
	dkp->vs = 40;
	dkp->hs = 40;

	get_screen_size(&xres,&yres);
	dkp->screen_w = xres;
	dkp->screen_h = yres;
	printf("screen_w = %d, screen_h = %d\n", dkp->screen_w, dkp->screen_h);
	//set_screen_32pixel();

	//strcpy(dkp->bkg.bmp_path,"res/bkgnd.jpg");
}

static int calculate_coordinate(desktop_t *dkp, int idx,  int *x, int *y)
{
	int max_column = 0;

	box_t *ptr = &dkp->icon.item[idx].box;
	
	max_column = dkp->screen_w / ( ptr->w + dkp->hs);
	
	*x = (idx % max_column) * (ptr->w + dkp->hs) + dkp->sx;
	*y =  (idx /max_column) * (ptr->h + dkp->vs) + dkp->sy;
	
	return 0;
}

static void show_icon(HDC hdc, icon_t *icon)
{
	int i = 0;
	box_t *ptr = NULL;
	int fcs_idx = 0;
	
	//assert(icon);
	
	desktop_t *dkp = container_of(icon,desktop_t,icon);
	if (icon->partial_draw) {
		redraw_focus(&icon->focus,hdc);
		icon->partial_draw = 0;
	} else {
		for (i=0;i<icon->nr; i++) {
			ptr = &icon->item[i].box;
			calculate_coordinate(dkp,i,&ptr->x,&ptr->y);
			//CN_PRINT("x:%d y:%d\n",ptr->x,ptr->y);
			if (icon->focus.idx == i) {
				simple_draw_focus(&icon->focus,hdc,icon->focus.idx); /*default focus*/
				/*repair text for the first time does not repaint*/
				fcs_idx = icon->focus.get_focus_item_idx(icon->focus.nml_item,0);
				icon->item[0].name_box = icon->fcs_item[fcs_idx].name_box;
			} else {
				icon->show_item(hdc,&icon->item[i]);
			}
		}
	}
}


/****************************************************************************************************************/

static int desktop_init(void *ctx, HDC hdc, HWND hwnd)
{	
	desktop_t *dkp = (desktop_t *)ctx;
	
	load_default_config(dkp,DKP_CFG_PATH);
	icon_init(&dkp->icon,hdc);

	dkp->show_icon = show_icon;
	//dkp->load_bankgroud = background_init;
	//dkp->show_bankgroud = background_paint;
	//dkp->unload_bankgroud = background_destory;
	dkp->bkg_ops = &def_bkg_ops;
	dkp->bkg_handle = dkp->bkg_ops->init("res/home.png",hdc);
	
	//dkp->load_bankgroud(&dkp->bkg,hdc);
	
	return 0;
}

static int desktop_destory(void *ctx)
{
	desktop_t *dkp = (desktop_t *)ctx;
	
	icon_exit(&dkp->icon);
	//dkp->unload_bankgroud(&dkp->bkg);
	dkp->bkg_ops->destory(dkp->bkg_handle);
}

static int desktop_draw(void *ctx, HDC hdc)
{
	desktop_t *dkp = (desktop_t *)ctx;
	dkp->show_icon(hdc,&dkp->icon);
	
	return 0;
}

static int entry_app(icon_t *icon, int idx, HWND hwnd)
{
	printf("name is %s\n", icon->item[idx].name);
	if (0 == strcmp(icon->item[idx].name,"SETTING")) {
		app_set(hwnd);
	} else if (0 == strcmp(icon->item[idx].name,"MUSIC")) {
		char mnt_path[128] = {0};
		panel_t panel;
		#ifdef _SUNXI_C500
			get_device_mnt_path("mmc",mnt_path);
		#else
			sprintf(mnt_path,"%s","/home/work");
		#endif
		panel_init(&panel);
		//panel_set_cwd(&panel,mnt_path);
		//panel_dir_load(&panel);
		//panel_filter(&panel,audio_file_filter);
		panel_dir_recursive_load(&panel.dir,mnt_path,-1,audio_file_filter);
		
		if ('\0' != mnt_path[0]  && panel.dir.len) {
			app_music(hwnd);
		}
		
		panel_destory(&panel);
	} else if (0 == strcmp(icon->item[idx].name,"PICTURE") ){
		//todo
		app_image(hwnd);
	} else if (0 == strcmp(icon->item[idx].name,"FILE") ){
		//todo
		app_file_mgr(hwnd);
	}	
	return 0;
}

static int desktop_key_event(void *ctx, unsigned short keycode, HWND hwnd)
{
	focus_t *ptr = NULL;
	desktop_t *dkp = (desktop_t *)ctx;
	
	ptr = &dkp->icon.focus;
	if (SYME_KEY_ARROW_LEFT == keycode || SYME_KEY_ARROW_RIGHT == keycode) {
		calculate_focus_index(ptr,dkp->icon.nr,SYME_KEY_ARROW_LEFT==keycode ? 0 : 1);
		invalidate_focus(ptr,hwnd);
		dkp->icon.partial_draw = 1;
	}  else if (SYME_KEY_ENTER == keycode) {
		entry_app(&dkp->icon,ptr->idx,hwnd);
		//app_set(hwnd);
		//app_filemgr(hwnd);
		//app_music(hwnd);
	} 
	//else if (SYME_KEY_ESC == keycode) {
	//	PostMessage (hwnd, MSG_CLOSE, 0, 0);
	//}
}

static int desktop_erase_bankgroud(void *ctx, HDC hdc, HWND hwnd, RECT* clip)
{
	desktop_t *dkp = (desktop_t *)ctx;
	return dkp->bkg_ops->paint(dkp->bkg_handle,hdc,hwnd,clip);
}


static wnd_ops_t dsk_wnd_ops = {
	.name = "desktop",
	.ctx_size = sizeof(desktop_t),
	.init = desktop_init,
	.destory = desktop_destory,
	.draw = desktop_draw,
	.key_event = desktop_key_event,
	.erase_background = desktop_erase_bankgroud,
	.timer_event = NULL,
};


int app_desktop(HWND hosting)
{
	return app_create(hosting,&dsk_wnd_ops);
}

