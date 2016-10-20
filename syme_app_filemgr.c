#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syme_app_filemgr.h"
#include "cdlb_string.h"
#include "syme_bk_listctrl.h"
#include "syme_app.h"

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif


static int month_aton(char *mon)
{
	int idx = 0;
	char *ptr = NULL;
	char *mstr = "JanFebMarAprMayJunJulAugSepOctNovDec";

	ptr = strstr(mstr,mon);
	idx = ptr - mstr;
	if (idx % 3) {
		CN_PRINT("illeagal param\n");
		return CN_ERR;
	}
	
	idx /= 3;
	return idx;
}

static int is_strip_space(char ch)
{
	if (' ' == ch || '\n' == ch) {
		return 1;
	}

	return 0;
}

/*time year month day*/
static char *calendar_astime(struct tm *tm, char *local_tm)
{
	char *date;
	char *year;
	char *mon;
	char *week;
	char *day;
	char *day_time;
	char *ptr;
	int i = 0;
	
	date = asctime(tm);
	ptr = strtok(date, " ");
	while(ptr != NULL)
	{
		switch(i)
		{
			case 0: 
				week = ptr;
				break;
			case 1: 
				mon = ptr; 
				break;
			case 2: 
				day = ptr; 
				break;
			case 3: 
				day_time = ptr; 
				break;
			case 4: 
				year = ptr; 
				break;
			default: 
				break;
		}
		ptr = strtok(NULL, " ");
		i++;
	}

	sprintf(local_tm,"%s\n%s year %d month %s day",day_time,strip_ch(year,is_strip_space),month_aton(mon),day);
	//sprintf(local_tm, "%s, %s %s %s %s GMT\n",week,day,mon,strip_ch(year," "),day_time);
}

static int is_same_file_type(file_mgr_t *flmgr, int last_sel)
{
	file_entry_t *list = flmgr->panel.dir.list;
	int cur_sel = flmgr->panel.selected;
	
	if (S_ISDIR(list[cur_sel].st.st_mode) == S_ISDIR(list[last_sel].st.st_mode)) {
		return 1;
	}
	
	return 0;
}

static int show_flmgr_text(file_mgr_t *flmgr)
{
	char buf[128] = {0};
	file_entry_t *list = flmgr->panel.dir.list;
	int *sel = &flmgr->panel.selected;
	
	sprintf(buf,"%s%s%s",flmgr->path_prefix, flmgr->panel.cwd_vpath,list[*sel].name);
	SendMessage(flmgr->path_hwnd,MSG_SETTEXT,0,(LPARAM)buf);
	memset(buf,0,sizeof(buf));
	sprintf(buf,"%lx byte\n",list[*sel].st.st_size);
	calendar_astime(localtime(&list[*sel].st.st_mtime),buf+strlen(buf));
	SendMessage(flmgr->info_hwnd,MSG_SETTEXT,0,(LPARAM)buf);
}

static void dir_notify_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
	int cur_sel = 0;
	file_mgr_t *flmgr = NULL;

	flmgr = (file_mgr_t *)add_data;
	if (LBN_ENTER == nc) {
		cur_sel = SendMessage (hwnd, LB_GETCURSEL, 0, 0L);
	}
}

static int add_flmgr_list(file_mgr_t *flmgr)
{
#if 0
	int i = 0;
	if (flmgr->panel.dirty) {
		SendMessage (flmgr->list_hwnd, LB_RESETCONTENT, 0, (LPARAM)0);
		for (i=0; i<flmgr->panel.dir.len; i++) {
			SendMessage (flmgr->list_hwnd, LB_ADDSTRING, 0, (LPARAM)flmgr->panel.dir.list[i].name);
		}
		SendMessage (flmgr->list_hwnd, LB_SETCURSEL, flmgr->panel.selected, 0);
	}
#else
	LISTBOXITEMINFO lbii;
	int i = 0;
	if (flmgr->panel.dirty) {
		SendMessage (flmgr->list_hwnd, LB_RESETCONTENT, 0, (LPARAM)0);
		for (i=0; i<flmgr->panel.dir.len; i++) {
			lbii.hIcon = (DWORD) (S_ISDIR(flmgr->panel.dir.list[i].st.st_mode) ? &flmgr->lst_folder_bmp : &flmgr->lst_file_bmp);
			lbii.cmFlag = CMFLAG_BLANK | IMGFLAG_BITMAP;
			//lbii.hIcon = m_hicon;
			//lbii.cmFlag = CMFLAG_BLANK;
			lbii.string = flmgr->panel.dir.list[i].name;
			SendMessage (flmgr->list_hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
		}
		SendMessage (flmgr->list_hwnd, LB_SETCURSEL, flmgr->panel.selected, 0);
		flmgr->panel.dirty = 0;
	}
#endif

	
	return 0;
}

static int load_flmgr_config(file_mgr_t *flmgr, char *cfg)
{
	flmgr->name = strdup("file mgr");
	flmgr->path_prefix = strdup("path:");
	flmgr->lst_bmp_path = strdup("res/set_item_f.png");
	
	flmgr->folder_bpath = strdup("res/cls_dir.png");
	flmgr->file_bpath = strdup("res/cls_file.png");

	flmgr->lst_folder_bpath = strdup("res/lst_folder.png");
	flmgr->lst_file_bpath = strdup("res/lst_file.png");

	//flmgr->panel.cwd_vpath = strdup("./");
	flmgr->panel.lwd_vpath = NULL;
	
	return 0;
}

static int file_mgr_init(void *ctx, HDC hdc, HWND hwnd)
{
	int h = 0;
	file_mgr_t *flmgr = (file_mgr_t *)ctx;
	
	load_flmgr_config(flmgr, "configs/flmgr_cfg");
	LoadBitmapFromFile(hdc,&flmgr->file_bmp,flmgr->file_bpath);
	LoadBitmapFromFile(hdc,&flmgr->folder_bmp,flmgr->folder_bpath);
	LoadBitmapFromFile(hdc,&flmgr->lst_bmp,flmgr->lst_bmp_path);
	LoadBitmapFromFile(hdc,&flmgr->lst_file_bmp,flmgr->lst_file_bpath);
	LoadBitmapFromFile(hdc,&flmgr->lst_folder_bmp,flmgr->lst_folder_bpath);
	
	//m_pbmp = &flmgr->lst_bmp;

#if 0
	flmgr->path_hwnd = CreateWindow (CTRL_STATIC, flmgr->path_prefix, 
               WS_CHILD | SS_NOTIFY | WS_VISIBLE, IDC_STATIC, 
              10, 10, 480, 30, hwnd ,0);

	flmgr->info_hwnd = CreateWindow (CTRL_STATIC, "", 
                WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE | DT_SINGLELINE | WS_EX_TRANSPARENT,0xf2, 
                10, 350, 150, 60, hwnd, 0);
#endif
	flmgr->path_hwnd = CreateWindowEx (CTRL_STATIC, flmgr->path_prefix, 
                            WS_CHILD | WS_VISIBLE | SS_SIMPLE, WS_EX_TRANSPARENT,
                            0xf1, 10, 10, 480, 30, hwnd , 0);
	
	flmgr->info_hwnd = CreateWindowEx (CTRL_STATIC, "", 
                WS_CHILD | SS_SIMPLE | WS_VISIBLE | DT_SINGLELINE,WS_EX_TRANSPARENT,
                0xf2, 10, 350, 150, 60, hwnd, 0);
	
//	flmgr->list_hwnd = CreateWindow (CTRL_LISTBOX, "", 
//		WS_CHILD | LBS_NOTIFY | WS_VISIBLE | WS_VSCROLL, 0xf4,
//		250, 50, 320, 360, hwnd, 0);
//	flmgr->list_hwnd = CreateWindow (CTRL_LISTBOX, "", 
//		WS_CHILD | LBS_NOTIFY | WS_VISIBLE | WS_VSCROLL | LBS_USEICON, 0xf4,
//		250, 50, 320, 360, hwnd, 0);

	box_t box = {250,50,320,360};
	flmgr->list_hwnd = create_bk_listctl("",hwnd,box,WS_CHILD | LBS_NOTIFY | WS_VISIBLE | WS_VSCROLL | LBS_USEICON, 0xf4,&flmgr->lst_bmp);
	SetFocusChild(flmgr->list_hwnd);
	//SetWindowAdditionalData(flmgr->list_hwnd,(DWORD)flmgr);
	//SetNotificationCallback(flmgr->list_hwnd,dir_notify_proc);
	
	panel_init(&flmgr->panel);
	panel_set_cwd(&flmgr->panel,"./");
	panel_dir_load(&flmgr->panel);
	flmgr->panel.selected = 0;
	
	//m_hicon = LoadIconFromFile (HDC_SCREEN, "res/music.ico", 1);
	 //GetIconSize (m_hicon, NULL, &h);
    	SendMessage (flmgr->list_hwnd, LB_SETITEMHEIGHT, 0, flmgr->lst_folder_bmp.bmHeight);

	show_flmgr_text(flmgr);
	return 0;
}

static int file_mgr_destory(void *ctx)
{
	file_mgr_t *flmgr = (file_mgr_t *)ctx;
	
	if (flmgr->name) {
		free(flmgr->name);
	}

	if (flmgr->path_prefix) {
		free(flmgr->path_prefix);
	}

	if (flmgr->file_bpath) {
		free(flmgr->file_bpath);
	}

	if (flmgr->folder_bpath) {
		free(flmgr->folder_bpath);
	}
	
	panel_destory(&flmgr->panel);
	
	return 0;
}

static int file_mgr_draw(void *ctx, HDC hdc)
{
	file_mgr_t *flmgr = (file_mgr_t *)ctx;
	
	add_flmgr_list(flmgr);
	//show_flmgr_text(flmgr); /*when use CreateWindowEx, it isn't correct,Why????*/
	if (S_ISDIR(flmgr->panel.dir.list[flmgr->panel.selected].st.st_mode)) {
		FillBoxWithBitmap(hdc,50,160,64,64,&flmgr->folder_bmp);
	} else {
		FillBoxWithBitmap(hdc,50,160,64,64,&flmgr->file_bmp);
	}
	
	return 0;
}

static int file_erase_background(void *ctx, HDC hdc, HWND hwnd, RECT* clip)
{
	wnd_ops_t *parent_ops = NULL;
	wnd_ops_t *wnd_ops = (wnd_ops_t *)GetWindowAdditionalData(hwnd);
	parent_ops = wnd_ops->parent_ops;
	
	return parent_ops->erase_background(parent_ops->ctx,hdc,hwnd,clip); 
}


static int file_mgr_key_event(void *ctx, unsigned short keycode, HWND hwnd)
{
	//char buf[128] = {0};
	//file_entry_t *list = flmgr->panel.dir.list;
	RECT rc;
	int last_sel = 0;
	file_mgr_t *flmgr = (file_mgr_t *)ctx;
	int *sel = &flmgr->panel.selected;

	last_sel = *sel;
	if (SYME_KEY_ARROW_UP == keycode || SYME_KEY_ARROW_DOWN == keycode) {
		if (SYME_KEY_ARROW_UP == keycode) {
			//*sel = (*sel + flmgr->panel.dir.len - 1) % flmgr->panel.dir.len;
			if (*sel > 0) {
				*sel -= 1;
			#ifdef _SUNXI_C500
			SendMessage(flmgr->list_hwnd,MSG_KEYDOWN,SCANCODE_CURSORBLOCKUP,0);
			#endif
			}
		} else {
			//*sel = (*sel + 1) % flmgr->panel.dir.len;
			if (*sel < flmgr->panel.dir.len - 1) {
				*sel += 1;
				#ifdef _SUNXI_C500
				SendMessage(flmgr->list_hwnd,MSG_KEYDOWN,SCANCODE_CURSORBLOCKDOWN,0);
				#endif
			}
		}

	#if 0
		sprintf(buf,"%s%s%s",flmgr->path_prefix, flmgr->panel.cwd_vpath,list[*sel].name);
		SendMessage(flmgr->path_hwnd,MSG_SETTEXT,0,(LPARAM)buf);
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%x b\n",list[*sel].st.st_size);
		calendar_astime(localtime(&list[*sel].st.st_mtime),buf+strlen(buf));
		SendMessage(flmgr->info_hwnd,MSG_SETTEXT,0,(LPARAM)buf);
	#endif

		show_flmgr_text(flmgr);

		if (0 == is_same_file_type(flmgr,last_sel)) {
			rc.left = 50;
			rc.top = 160;
			rc.right = 50 + 64;
			rc.bottom = 160 + 64;
			InvalidateRect(hwnd,&rc,1);
		}
	}else if (SYME_KEY_ESC == keycode) {
		PostMessage (hwnd, MSG_CLOSE, 0, 0);
	}
	
	return 0;
}

/*
SetNotificationCallback may be better,but i can't find any way to add add_data.
then it destroy designs. 
*/
static int file_mgr_cmd_event(void *ctx, int id, unsigned short cmd, HWND hwnd)
{
	RECT rc;
	char buf[256] = {0};
	char *name = NULL;
	char* slash = NULL;
	file_mgr_t *flmgr = (file_mgr_t *)ctx;

	if (LBN_ENTER == cmd && S_ISDIR(flmgr->panel.dir.list[flmgr->panel.selected].st.st_mode)) {
		name = flmgr->panel.dir.list[flmgr->panel.selected].name;
		if (0 == strcmp(name,".")) {
			return 0;
		} else if (0 == strcmp(name,"..")) {
			strcpy(buf,flmgr->panel.cwd_vpath);
			if (0 == strcmp (buf, "/"))
				return 0;
			
			slash = strrchr (buf, '/');
			if (slash == NULL)
				return 0;
			if (slash == buf)
				strcpy (buf, "/");
			else
				*slash = '\0';
		} else {
			if ('/' == *(flmgr->panel.cwd_vpath+strlen(flmgr->panel.cwd_vpath)-1) ) {
				sprintf(buf,"%s%s",flmgr->panel.cwd_vpath,name);
			} else {
				sprintf(buf,"%s/%s",flmgr->panel.cwd_vpath,name);
			}
			
		}
		panel_set_cwd(&flmgr->panel,buf);
		panel_dir_load(&flmgr->panel);
		flmgr->panel.selected = 0;		
		add_flmgr_list(flmgr);
		
		rc.left = 50;
		rc.top = 160;
		rc.right = 50 + 64;
		rc.bottom = 160 + 64;
		InvalidateRect(GetParent(hwnd),&rc,1);
	}
	return 0;
}


static wnd_ops_t flmgr_wnd_ops = {
	.name = "filemgr",
	.ctx_size = sizeof(file_mgr_t),
	.init = file_mgr_init,
	.destory = file_mgr_destory,
	.draw = file_mgr_draw,
	.erase_background = file_erase_background,
	.key_event = file_mgr_key_event,
	.cmd_event = file_mgr_cmd_event,
	.timer_event = NULL,
};

int app_file_mgr(HWND hosting)
{
	return app_create(hosting,&flmgr_wnd_ops);
}

