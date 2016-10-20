#include "mg_common_header.h"
#include "syme_app.h"

int init_app_create_Info(MAINWINCREATE *info, HWND hosting, wnd_ops_t *wnd_ops, WNDPROC wndProc)
{
	//info->dwStyle = WS_NONE;
	//info->dwExStyle = WS_EX_AUTOSECONDARYDC;
	info->dwStyle = WS_VISIBLE | WS_BORDER;
	info->dwExStyle = WS_EX_NONE;
	info->spCaption = wnd_ops->name;
	info->hMenu = 0;
	info->hCursor = GetSystemCursor(0);
	info->hIcon = 0;
	info->MainWindowProc = wndProc;
	info->lx = 0;
	info->ty = 0;
	//get_screen_size(&info->rx,&info->by);
	info->rx = 400;
	info->by = 1280;
	info->iBkColor = COLOR_lightwhite;
	info->dwAddData = (DWORD)wnd_ops;
	info->hHosting = hosting;

	wnd_ops->parent_ops = (wnd_ops_t *)GetWindowAdditionalData(hosting);
	
	return 0;
}

int general_app_create(HWND hosting, char *name, WNDPROC wndProc)
{
	MSG Msg;
	HWND hMainWnd;
	MAINWINCREATE CreateInfo;

#if 1
	//general_app_create_ex(hosting,name,wndProc,&hMainWnd);
#else

	initAppCreateInfo(&CreateInfo,hosting,name,wndProc);

	hMainWnd = CreateMainWindow (&CreateInfo);
	if (HWND_INVALID == hMainWnd) {
		fprintf(stderr,"failed to create main window!\n");
		return CN_ERR;
	}
	
	ShowWindow(hMainWnd, SW_SHOWNORMAL);
	while (GetMessage (&Msg, hMainWnd)) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}
	
	MainWindowThreadCleanup (hMainWnd);
#endif
	
    return 0;
}

int general_app_create_ex(HWND hosting, wnd_ops_t *wnd_ops, WNDPROC wndProc, HWND *hwnd)
{
	MSG Msg;
	MAINWINCREATE CreateInfo;

	init_app_create_Info(&CreateInfo,hosting,wnd_ops,wndProc);

	*hwnd = CreateMainWindow (&CreateInfo);
	if (HWND_INVALID == *hwnd) {
		fprintf(stderr,"failed to create main window!\n");
		return CN_ERR;
	}
	
	ShowWindow(*hwnd, SW_SHOWNORMAL);
	while (GetMessage (&Msg, *hwnd)) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}
	
	MainWindowThreadCleanup (*hwnd);
	
    	return 0;
}



