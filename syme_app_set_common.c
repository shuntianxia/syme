#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "mg_common_header.h"
#include "syme_app_set_common.h"
#include "syme_app_set_page.h"

#include "syme_background.h"


static child_option_t product_opt[] = {
	{
		.name = "product info",
		.val = NULL,
		.nb = 0,
		.idx = 0,
		.prompt = "disk capacity",
	},
	{
		.name = "restore factory settings",
		.val = NULL,
		.nb = 0,
		.idx = 0,
		.prompt = "restore factory settings",
	}
};


/*dialog*/
static DLGTEMPLATE product_dlg =
{
    WS_VISIBLE, 
    WS_EX_NONE,
    175, 70, 300, 200, 
    "",
    0, 0,
    3, NULL,
    0
};

static CTRLDATA product_ctrl[] =
{ 
    {
        "static",
        WS_VISIBLE | SS_CENTER,
        0, 15, 300, 44, 
        100, 
        "version:v1.0",
        0,
        WS_EX_TRANSPARENT
    },
    {
        "static",
        WS_VISIBLE,
        20, 50, 300, 32, 
        101, 
        "TF capacity : ",
        0,
        WS_EX_TRANSPARENT
    },
      {
        "static",
        WS_VISIBLE,
        20, 80, 300, 32, 
        102, 
        "Remaining capacity : ",
        0,
        WS_EX_TRANSPARENT
    },
    
};

static BITMAP product_bmp;
static void *bkg_handle;
extern bkg_ops_t def_bkg_ops;

static int product_dlg_proc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT *clip;
	DWORD keycode;
	char str[64] = {0};
	gal_pixel old_color;
	
	switch (message) {
		case MSG_INITDIALOG:
			hdc = GetClientDC(hDlg);
			//LoadBitmapFromFile(HDC hdc,&product_bmp,"res/set_info.png");
			bkg_handle = def_bkg_ops.init("res/msgbox.png",hdc);
			GetDlgItemText(hDlg,101,str,sizeof(str));
			CN_PRINT("%s\n",str);
			sprintf(str+strlen(str),"%.2f%s",0.00,"GB");
			SetDlgItemText(hDlg,101,str);
			GetDlgItemText(hDlg,102,str,sizeof(str));
			sprintf(str+strlen(str),"%.2f%s",0.00,"GB");
			SetDlgItemText(hDlg,102,str);
			return 1;
			//break;
		case MSG_ERASEBKGND:
			hdc = (HDC)wParam;
			clip = (RECT*) lParam;
			def_bkg_ops.paint(bkg_handle,hdc,hDlg,clip);
			return 0;
			//break;
		case MSG_COMMAND:
			break;
		case MSG_KEYDOWN:
			//CN_PRINT("MSG_KEYDOWN\n");
			def_bkg_ops.destory(bkg_handle);
			EndDialog (hDlg, IDCANCEL);
			break;
		case MSG_CLOSE:
			//CN_PRINT("MSG_CLOSE\n");
			def_bkg_ops.destory(bkg_handle);
			EndDialog (hDlg, IDCANCEL);
			return 0;
	}

	return DefaultDialogProc (hDlg, message, wParam, lParam);
}


static int show_product_info(HWND owner)
{
	product_dlg.controls = product_ctrl;
	return DialogBoxIndirectParam(&product_dlg,owner,product_dlg_proc,0L);
}


int common_set_init(common_set_t *cs, set_page_t *page)
{
	cs->page = page;
	cs->page->title = "common set";
	set_page_var_init(cs->page,product_opt,ARRY_SIZE(product_opt));
	
	return 0;
}

int common_set_draw(common_set_t *cs, HDC hdc)
{
	set_page_draw(cs->page, hdc);
	
	return 0;
}


int common_set_destory(common_set_t *cs)
{
	return 0;
}

int common_set_key_event(common_set_t *cs, unsigned short keycode, HWND hwnd)
{
	if (SYME_KEY_ENTER == keycode && 0 == cs->page->fcs.idx) {
		show_product_info(hwnd);
		//show_volume_dlg(hwnd);
	} else {
		set_page_key_event(cs->page,keycode,hwnd);
	}
	
	return 0;
}


