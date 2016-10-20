#include "mg_common_header.h"

#include "syme_app_set_common.h"
#include "syme_app_set_page.h"

#include "syme_background.h"
#include "sound/snd_alsa_mixer.h"


#define ID_VOLUME_CMBX (0)
#define ID_VOLUME_BUTTON (1)

static child_option_t volume_opt[] = {
	{
		.name = "volume info",
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
static DLGTEMPLATE volume_dlg =
{
    WS_VISIBLE, 
    WS_EX_NONE,
    175, 120, 300, 200, 
    "",
    0, 0,
    3, NULL,
    0
};

/*
the num more than 3 why ???
*/
static CTRLDATA volume_ctrl[] =
{
	{
	"static",
	WS_DISABLED,
	0, 15, 100, 25, 
	100, 
	"ddd",
	0
	},
	{
	CTRL_COMBOBOX,
	WS_CHILD | WS_VISIBLE | ES_CENTER |CBS_SPINARROW_LEFTRIGHT |CBS_READONLY | CBS_AUTOSPIN | CBS_AUTOLOOP,
	125, 45, 50, 25,
	105,
	"",
	0
	},
	{
	"button",
	WS_VISIBLE | BS_PUSHBUTTON,
	115, 120, 70, 25,
	IDOK,
	"OK",
	0
	},
};

static BITMAP volume_bmp;
static void *bkg_handle;
extern bkg_ops_t def_bkg_ops;

static snd_alsa_mixer_t m_amx;

static int volume_dlg_proc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT *clip;
	DWORD keycode;
	char str[64] = {0};
	gal_pixel old_color;
	
	switch (message) {
		case MSG_INITDIALOG:
			snd_amx_init(&m_amx,10);
			SendDlgItemMessage(hDlg, 105, CB_SETSPINFORMAT, 0, (LPARAM)"%d");
			SendDlgItemMessage(hDlg, 105, CB_SETSPINRANGE, 0, 10);
			SendDlgItemMessage(hDlg, 105, CB_SETSPINVALUE, snd_amx_get_volume(&m_amx), 0);
			SendDlgItemMessage(hDlg, 105, CB_SETSPINPACE, 1, 1);
			return 1;
		case MSG_COMMAND:
			break;
		case MSG_KEYDOWN:
			keycode = LOWORD(wParam);
			CN_PRINT("keycode:%lx\n",keycode);
			if (SYME_KEY_ARROW_LEFT == keycode) {
				SendDlgItemMessage(hDlg, 105, CB_SPIN, TRUE, 0);
				snd_amx_set_volume(&m_amx,snd_amx_get_volume(&m_amx)-1);
			} else if (SYME_KEY_ARROW_RIGHT == keycode) {
				SendDlgItemMessage(hDlg, 105, CB_SPIN, FALSE, 0);
				snd_amx_set_volume(&m_amx,snd_amx_get_volume(&m_amx)+1);
			} else if (SYME_KEY_ENTER == keycode) {
				snd_amx_destory(&m_amx);
				EndDialog (hDlg, IDCANCEL);
				
			}
			return 0;
		case MSG_CLOSE:
			snd_amx_destory(&m_amx);
			EndDialog (hDlg, IDCANCEL);
			
			return 0;
	}

	return DefaultDialogProc (hDlg, message, wParam, lParam);
}

 int show_volume_dlg(HWND owner)
{
	volume_dlg.controls = volume_ctrl;
	return DialogBoxIndirectParam(&volume_dlg,owner,volume_dlg_proc,0L);
}


