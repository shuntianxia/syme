#include "syme_app.h"

static  int DefWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	WORD keycode = -1;
	wnd_ops_t *wnd_ops = NULL;

	wnd_ops = (wnd_ops_t *)GetWindowAdditionalData(hWnd);
	switch (message) {
	case MSG_CREATE:
		wnd_ops->ctx = malloc(wnd_ops->ctx_size);
		if (!wnd_ops->ctx ) {
			CN_PRINT("no mem!\n");
			return CN_ERR;
		}
		wnd_ops->init(wnd_ops->ctx,HDC_SCREEN,hWnd);
		break;

	case MSG_PAINT:
		hdc = BeginPaint (hWnd);
		wnd_ops->draw(wnd_ops->ctx,hdc);
		EndPaint (hWnd, hdc);
		return 0;

	case MSG_ERASEBKGND:
		if (wnd_ops->erase_background) {
			HDC hdc = (HDC)wParam;
			RECT* clip = (RECT*) lParam;
			wnd_ops->erase_background(wnd_ops->ctx,hdc,hWnd, clip);
			return 0;
		}
		//return 0; /*draw*/
		break; /*not draw*/
		
	case MSG_KEYDOWN:
		keycode = LOWORD(wParam);
		CN_PRINT("keycode:%x\n",keycode);
		if (wnd_ops->key_event) {
			wnd_ops->key_event(wnd_ops->ctx,keycode,hWnd);
		}
		//if (SYME_KEY_ESC == keycode) {
		//	PostMessage (hWnd, MSG_CLOSE, 0, 0);
		//} else {
		//}
		break;
	case MSG_TIMER:
		if (wnd_ops->timer_event) {
			wnd_ops->timer_event(wnd_ops->ctx,(int)lParam,(DWORD)wParam,hWnd);
		}
		break;
	case MSG_COMMAND:
		if (wnd_ops->cmd_event) {
			wnd_ops->cmd_event(wnd_ops->ctx,LOWORD(wParam),HIWORD(wParam),hWnd);
		}
		break;
	case MSG_CLOSE:
		DestroyMainWindow (hWnd);
		wnd_ops->destory(wnd_ops->ctx);
		free(wnd_ops->ctx);
		return 0;
	}
	
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int app_create(HWND hosting, wnd_ops_t *wnd_ops)
{
	HWND hwnd;
	general_app_create_ex(hosting,wnd_ops,DefWinProc,&hwnd);
	
	return hwnd;
}

