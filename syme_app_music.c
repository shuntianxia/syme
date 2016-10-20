#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mg_common_header.h"
#include "syme_app_music.h"
#include "syme_app.h"

#define PROGRESS_OFFSET_X (50)


int audio_file_filter(file_entry_t *fe);

char *get_file_name(char *path)
{
	char *name = NULL;
	name = strrchr(path,'/');
	return (name ? (name + 1) : NULL);
}

static int add_msc_list(music_t *msc)
{
	LISTBOXITEMINFO lbii;
	int i = 0;
	if (msc->audio_panel.dirty) {
		SendMessage (msc->list_hwnd, LB_RESETCONTENT, 0, (LPARAM)0);
		for (i=0; i<msc->audio_panel.dir.len; i++) {
			//lbii.hIcon = (DWORD) (S_ISDIR(msc->panel.dir.list[i].st.st_mode) ? &msc->lst_folder_bmp : &msc->lst_file_bmp);
			//lbii.cmFlag = CMFLAG_BLANK | IMGFLAG_BITMAP;
			lbii.hIcon = 0;
			lbii.cmFlag = CMFLAG_BLANK;
			lbii.string = get_file_name(msc->audio_panel.dir.list[i].name);
			SendMessage (msc->list_hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
		}
		SendMessage (msc->list_hwnd, LB_SETCURSEL, msc->audio_panel.selected, 0);
		msc->audio_panel.dirty = 0;
	}

	return 0;
}

static int load_music_config(music_t *msc, char *cfg)
{
	msc->bk_list_bpath = strdup("res/set_item_f.png");
	//msc->audio_panel.cwd_vpath = strdup("./");
	msc->path_prefix = strdup("playlist:");

	msc->spt.bpath = strdup("res/particle.bmp");
	msc->spt.num = 10;
	msc->spt.box.x = msc->screen_w / 2 + 20,
	msc->spt.box.y = 60,
	msc->spt.box.w= 9;
	msc->spt.box.h = 60;
	msc->spt.freq = (float *)malloc(sizeof(float)*msc->spt.num);
	memset(msc->spt.freq,0,sizeof(float)*msc->spt.num);
	

	/*
	now playing:
	total number of songs
	*/
}

static int show_file_info(music_t *msc)
{
	char buf[64] = {0};
	
	sprintf(buf,"%s%d\n%s%d","now playing:", msc->audio_panel.selected,"total number of songs:",msc->audio_panel.dir.len);
	SendMessage(msc->file_info_hwnd,MSG_SETTEXT,0,(LPARAM)buf);
	return 0;
}

static void spectrum_callback(float *feq_domain, int num, void *cb_data)
{
	music_t *msc = (music_t *)cb_data;
	
	memcpy(msc->spt.freq,feq_domain+10,msc->spt.num*sizeof(float));
#if 0
	int i = 0;
	srand((int)time(0));
	//fprintf(stderr,"\n---------------------------\n");
	for (i=0; i<msc->spt.num; i++) {
		msc->spt.freq[i] =(rand() % 10 ) * 0.1;
		//fprintf(stderr," %f ",msc->spt.freq[i]);
	}
#endif
	
	RECT rc;
	rc.left = msc->spt.box.x;
	rc.top = msc->spt.box.y;
	rc.right = rc.left + msc->spt.num * (msc->spt.box.w + 10);
	rc.bottom =rc.top + msc->spt.box.h;
	InvalidateRect(msc->hwnd,&rc,1);
}

/********spectrum panel**************/
static int draw_spt_panel(spectrum_t *spt, HDC hdc)
{
	int x = 0, y = 0, h = 0, w = 0;;
	int i = 0;
	
	x = spt->box.x;
	y = spt->box.y;
	w = spt->box.w;
	for (i=0; i<spt->num; i++) {
		h = (int)(spt->box.h * spt->freq[i]);
		if (0 == h) {
			h = 1;
		}
		
		BitBlt(spt->bmpToDC,0,0,spt->box.w,h,hdc,x,y + spt->box.h - h,0);
		x += w + 10;
	}
	
	return 0;
}


static int music_play(music_t *msc)
{
#if 0
	char *fmt = NULL;
	char abs_file_path[128] = {0};
	if ('/' == *(msc->audio_panel.cwd_vpath+strlen(msc->audio_panel.cwd_vpath)-1) ) {
		fmt = "%s%s";
	} else {
		fmt = "%s/%s";
	}
	sprintf(abs_file_path,fmt,msc->audio_panel.cwd_vpath,msc->audio_panel.dir.list[msc->audio_panel.selected].name);
#endif
	
	snd_player_open(&msc->player,msc->audio_panel.dir.list[msc->audio_panel.selected].name);
	register_cb_func(&msc->player.sptaly,spectrum_callback, (void *)msc);
	snd_player_play(&msc->player);
	
	msc->cur_sec = 0;
	msc->total_sec = snd_player_get_duration(&msc->player);
	CN_PRINT("abs file path:%s total time:%d\n",msc->audio_panel.dir.list[msc->audio_panel.selected].name,msc->total_sec);
	SendMessage (msc->progress_hwnd, PBM_SETRANGE, (WPARAM)msc->total_sec, 0L);

	return 0;
}

static int music_stop(music_t *msc)
{
	if (SND_CLOSE != msc->player.status) {
		snd_player_close(&msc->player);
	}
	
	return 0;
}

static int music_pause(music_t *msc)
{
	if (SND_PLAY == msc->player.status) {
		snd_player_pause(&msc->player);
	}
	
	return 0;
}

static int music_init(void *ctx, HDC hdc, HWND hwnd)
{
	box_t box = {0,0,320,350};
	char buf[64] = {0};
	char mnt_path[128] = {0};
	music_t *msc = (music_t *)ctx;

	get_screen_size(&msc->screen_w,&msc->screen_h);
	box.w = msc->screen_w /2;
	box.h = msc->screen_h * 2 / 3;
	
	load_music_config(msc, "configs/msc_cfg");
	
	LoadBitmapFromFile(hdc,&msc->bk_list_bmp,msc->bk_list_bpath);
	msc->list_hwnd = create_bk_listctl("",hwnd,box,WS_CHILD | LBS_NOTIFY | WS_VISIBLE | WS_VSCROLL | LBS_USEICON,0xf5,&msc->bk_list_bmp);
	SetFocusChild(msc->list_hwnd);

	msc->path_hwnd = CreateWindowEx (CTRL_STATIC, "", 
                            WS_CHILD | WS_VISIBLE | SS_LEFT, WS_EX_TRANSPARENT,
                            0xf1, box.x, box.h + 30, 160, 100, hwnd , 0);
	msc->file_info_hwnd = CreateWindowEx (CTRL_STATIC, "", 
                            WS_CHILD | WS_VISIBLE | SS_LEFT, WS_EX_TRANSPARENT,
                            0xf1, box.x + 80, box.h + 30, 160, 100, hwnd , 0);
	
	panel_init(&msc->audio_panel);
	#ifdef _SUNXI_C500
		get_device_mnt_path("mmc",mnt_path);
	#else
		sprintf(mnt_path,"%s","/home/work");
	#endif
	panel_dir_recursive_load(&msc->audio_panel.dir,mnt_path,-1,audio_file_filter);
	msc->audio_panel.dirty = 1;
	msc->audio_panel.selected = 0;
	
	sprintf(buf,"%s\n%s\n",msc->path_prefix, msc->audio_panel.cwd_vpath);
	SendMessage(msc->path_hwnd,MSG_SETTEXT,0,(LPARAM)buf);
	show_file_info(msc);

	RegisterResFromFile (hdc, "res/lfskin_ProTrack.gif");
	msc->progress_hwnd = CreateWindowEx ("progressbar", NULL, 
                        WS_VISIBLE, WS_EX_NONE,0xf6,
                        msc->screen_w /2 + PROGRESS_OFFSET_X, msc->screen_h / 2, 200, 8, hwnd , 0);
	SetWindowElementAttr (msc->progress_hwnd, WE_LFSKIN_PROGRESS_HCHUNK, (DWORD)"res/lfskin_ProTrack.gif");
	SetTimer (hwnd, 1000, 100);
	
	LoadBitmapFromFile(hdc,&msc->spt.bmp,msc->spt.bpath);
	msc->hwnd = hwnd;

	msc->player.status = SND_CLOSE;

	msc->spt.bmpToDC = CreateMemDCFromBitmap(hdc, &msc->spt.bmp);

	
	LoadBitmapFromFile(hdc,&msc->play_bmp,"res/play.png");
	LoadBitmapFromFile(hdc,&msc->pause_bmp,"res/pause.png");
	msc->pa_box.x = msc->screen_w /2+20;
	msc->pa_box.y = msc->screen_h / 2 - 5;
	msc->pa_box.w = 22;
	msc->pa_box.h = 18;

	music_play(msc);
	return 0;
}

static int music_destory(void *ctx)
{
	music_t *msc = (music_t *)ctx;
	
	if (msc->bk_list_bpath) {
		free(msc->bk_list_bpath);
	}

	if (msc->path_prefix) {
		free(msc->path_prefix);
	}

	DeleteMemDC (msc->spt.bmpToDC);

	music_stop(msc);

	panel_destory(&msc->audio_panel);
	return 0;
}

static int time_conversion(char *buf, int total_sec)
{
	int hour = 0;
	int minutes = 0;
	int sec = 0;

	hour = total_sec / 3600;
	minutes = (total_sec % 3600 ) / 60;
	sec = total_sec % 60;
	sprintf(buf,"%2d:%2d",minutes,sec);

	return 0;
}

static int music_draw(void *ctx, HDC hdc)
{
	PBITMAP pbmp;
	char buf[32] = {0};
	music_t *msc = (music_t *)ctx;
	
	add_msc_list(msc);
	draw_spt_panel(&msc->spt,hdc);

	time_conversion(buf,msc->cur_sec);
	TextOut(hdc,msc->screen_w /2 + PROGRESS_OFFSET_X,msc->screen_h / 2-30,buf);
	
	time_conversion(buf,msc->total_sec);
	TextOut(hdc,msc->screen_w /2 + PROGRESS_OFFSET_X + 180,msc->screen_h / 2-30,buf);

	if (SND_PLAY == msc->player.status) {
		pbmp = &msc->play_bmp;
	} else {
		pbmp = &msc->pause_bmp;
	}
	FillBoxWithBitmap(hdc,msc->pa_box.x,msc->pa_box.y,msc->pa_box.w,msc->pa_box.h,pbmp);
	
	return 0;
}


static int music_key_event(void *ctx, unsigned short keycode, HWND hwnd)
{
	char abs_file_path[128] = {0};
	
	music_t *msc = (music_t *)ctx;
	int *sel = &msc->audio_panel.selected;
	
	if (SYME_KEY_ARROW_UP == keycode || SYME_KEY_ARROW_DOWN == keycode) {
		if (SYME_KEY_ARROW_UP == keycode) {
			if (*sel > 0) {
				*sel -= 1;
				#ifdef _SUNXI_C500
				SendMessage(msc->list_hwnd,MSG_KEYDOWN,SCANCODE_CURSORBLOCKUP,0);
				#endif
			}
		} else {
			if (*sel < msc->audio_panel.dir.len - 1) {
				*sel += 1;
				#ifdef _SUNXI_C500
				SendMessage(msc->list_hwnd,MSG_KEYDOWN,SCANCODE_CURSORBLOCKDOWN,0);
				#endif
			}
		}

		show_file_info(msc);
		
		music_stop(msc);
		music_play(msc);
	} else if (SYME_KEY_ENTER== keycode) {
		if (SND_PLAY == msc->player.status) {
			music_pause(msc);
			//msc->player.status = SND_PAUSE; /*you can't change player's status direct*/
		} else if (SND_PAUSE == msc->player.status) {
			//msc->player.status = SND_PLAY;
			snd_player_play(&msc->player); //not good for api (msuic_play music_pause music_stop)
		}
		
		RECT rc;
		rc.left = msc->pa_box.x;
		rc.top = msc->pa_box.y;
		rc.right = rc.left + msc->pa_box.w;
		rc.bottom = rc.top + msc->pa_box.h;
		InvalidateRect(hwnd,&rc,1);
	} else if (SYME_KEY_ESC== keycode) {
		PostMessage (hwnd, MSG_CLOSE, 0, 0);
	}

}

static int music_timer_event(void *ctx,int id, DWORD tick_cnt, HWND hwnd)
{
	int pos = 0;
	RECT rc;
	music_t *msc = (music_t *)ctx;
	
	if (SND_PLAY == msc->player.status) {
		if (msc->cur_sec < msc->total_sec) {
			msc->cur_sec++;
			pos = msc->cur_sec;

			CN_PRINT("pos:%d\n",pos);
			SendMessage (msc->progress_hwnd, PBM_SETPOS, (WPARAM)pos, 0L);

			
			rc.left = msc->screen_w /2 + PROGRESS_OFFSET_X;
			rc.top = msc->screen_h / 2-30;
			rc.right = rc.left + 230;
			rc.bottom = rc.top + 30;
			InvalidateRect(hwnd,&rc,1);
		} else {
			//to do
			music_stop(msc);
		}
	}
	
	return 0;
}

static int music_erase_background(void *ctx, HDC hdc, HWND hwnd, RECT* clip)
{
	typedef int (*erase_func)(void *ctx, HDC hdc, HWND hwnd, RECT* clip);
	
	wnd_ops_t *parent_ops = NULL;
	erase_func func_addr = music_erase_background;
	
	wnd_ops_t *wnd_ops = (wnd_ops_t *)GetWindowAdditionalData(hwnd);
	/*child call it*/
	if (wnd_ops->erase_background != func_addr) {
		parent_ops = wnd_ops->parent_ops->parent_ops; /*need to fix bug*/
	} else {
		parent_ops = wnd_ops->parent_ops;
	}
	
	
	return parent_ops->erase_background(parent_ops->ctx,hdc,hwnd,clip); 
}


static wnd_ops_t music_wnd_ops = {
	.name = "music",
	.ctx_size = sizeof(music_t),
	.init = music_init,
	.destory = music_destory,
	.draw = music_draw,
	.key_event = music_key_event,
	.erase_background = music_erase_background,
	.timer_event = music_timer_event,
};


int app_music(HWND hosting)
{
	SetDefaultWindowElementRenderer("skin");
	return app_create(hosting,&music_wnd_ops);
}

int audio_file_filter(file_entry_t *fe)
{
	if (strstr(fe->name,".mp3") || strstr(fe->name,".wav") || strstr(fe->name,".wma") 
		|| strstr(fe->name,".aac") || strstr(fe->name,".flac")) {
		return 0;
	}

	return 1;
}


