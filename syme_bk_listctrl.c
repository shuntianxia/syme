#include <stdio.h>
#include <stdlib.h>
#include "mg_common_header.h"

/*add modify*/
#include "listbox_impl.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"



#define LST_INTER_BMPTEXT       2
/** minimum height of checkmark */
#define LFRDR_LB_CHECKBMP_MIN   6


typedef struct {
	PBITMAP bk_pbmp;
	WNDPROC wndproc;
} bk_listctrl_t;


static int CheckMarkHeight (int item_height)
{
    int checkmark_height =
        item_height - LFRDR_LB_CHECKBMP_MIN;

    if (0 < checkmark_height)
        checkmark_height += checkmark_height >> 2;

    return checkmark_height;
}

static PLISTBOXITEM lstGetItem (PLISTBOXDATA pData, int pos)
{
    int i;
    PLISTBOXITEM plbi;

    plbi = pData->head;
    for (i=0; i < pos && plbi; i++)
        plbi = plbi->next;

    return plbi;
}

static void lstGetItemsRect (PLISTBOXDATA pData, 
                int start, int end, RECT* prc)
{
    if (start < 0)
        start = 0;

    prc->top = (start - pData->itemTop)*pData->itemHeight;

    if (end >= 0)
        prc->bottom = (end - pData->itemTop + 1)*pData->itemHeight;

}


static void lstOnDrawSListBoxItems (HWND hWnd, HDC hdc, PLISTBOXDATA pData, int width)
{
    DWORD dwStyle = GetWindowStyle (hWnd);
    PLISTBOXITEM plbi;
    PCONTROL pWin;
    RECT rect;
    int i;
    int x = 0, y = 0;
    int checkmark_height;       /** checkmark bmp height */
    int checkmark_status;       /** checkmark bmp status */

    gal_pixel gp_hilite_bgc;
    gal_pixel gp_hilite_fgc;
    gal_pixel gp_sign_bgc;
    gal_pixel gp_sign_fgc;
    gal_pixel gp_disabled_bgc;
    gal_pixel gp_disabled_fgc;
    gal_pixel gp_normal_bgc;
    gal_pixel gp_normal_fgc;
    gal_pixel gp_bkcolor;

	bk_listctrl_t *bk_listctrl;
	bk_listctrl = (bk_listctrl_t *)GetWindowAdditionalData(hWnd);

    pWin = (PCONTROL) hWnd;
    if (NULL == pWin) return;

    if (!pWin->we_rdr)
    {
        _MG_PRINTF ("CONTROL>ListBox: NULL LFRDR.\n");
        return;
    }

    checkmark_height = CheckMarkHeight (pData->itemHeight);
    plbi = lstGetItem (pData, pData->itemTop);

    gp_hilite_bgc   = GetWindowElementPixelEx 
        (hWnd, hdc, WE_BGC_HIGHLIGHT_ITEM);
    gp_hilite_fgc   = GetWindowElementPixelEx
        (hWnd, hdc, WE_FGC_HIGHLIGHT_ITEM);
    gp_sign_bgc   = GetWindowElementPixelEx 
        (hWnd, hdc, WE_BGC_SIGNIFICANT_ITEM);
    gp_sign_fgc   = GetWindowElementPixelEx
        (hWnd, hdc, WE_FGC_SIGNIFICANT_ITEM);
    gp_disabled_bgc = GetWindowElementPixelEx
        (hWnd, hdc, WE_BGC_DISABLED_ITEM);
    gp_disabled_fgc = GetWindowElementPixelEx
        (hWnd, hdc, WE_FGC_DISABLED_ITEM);
    gp_normal_bgc   = GetWindowElementPixelEx
        (hWnd, hdc, WE_BGC_WINDOW);
    gp_normal_fgc   = GetWindowElementPixelEx
        (hWnd, hdc, WE_FGC_WINDOW);

    (void)SetBkMode (hdc, BM_TRANSPARENT);

    gp_bkcolor = GetBkColor (hdc);

    for (i = 0; plbi && i < (pData->itemVisibles + 1); i++) {
            rect.left  = 0;
            rect.top   = y;
            rect.right = width;
            rect.bottom = rect.top + pData->itemHeight;

        if (plbi->dwFlags & LBIF_DISABLE) {
            SetBkColor (hdc, gp_disabled_fgc); 
            SetTextColor (hdc, gp_disabled_fgc);
            SetBrushColor (hdc, gp_disabled_bgc);

            /** render disabled item */
            pWin->we_rdr->draw_disabled_item (hWnd, hdc, &rect,
                    GetWindowElementAttr (hWnd, WE_BGC_DISABLED_ITEM));
        }
        else if (plbi->dwFlags & LBIF_SELECTED) {
		if (NULL==bk_listctrl->bk_pbmp || NULL==bk_listctrl->bk_pbmp->bmBits) {
			SetBkColor (hdc, gp_hilite_bgc); 
			SetTextColor (hdc, gp_hilite_fgc);
			SetBrushColor (hdc, gp_hilite_bgc);

			/** render hilited item */
			pWin->we_rdr->draw_hilite_item (hWnd, hdc, &rect,
				GetWindowElementAttr (hWnd, WE_BGC_HIGHLIGHT_ITEM));
		}
        } else {
            //SetBkColor (hdc, gp_normal_fgc);
            SetTextColor (hdc, gp_normal_fgc); 
            SetBrushColor (hdc, gp_normal_bgc);

            /** render normal item */
            if (!(pWin->dwExStyle & WS_EX_TRANSPARENT))
            pWin->we_rdr->draw_normal_item (hWnd, hdc, &rect,
                    GetWindowElementAttr (hWnd, WE_BGC_WINDOW));
        }

        /** render significant item */
        if ( (plbi->dwFlags & LBIF_SIGNIFICANT) &&
             !(plbi->dwFlags & LBIF_DISABLE)
           ) {
            SetBkColor (hdc, gp_sign_bgc); 
            SetTextColor (hdc, gp_sign_fgc);
            SetBrushColor (hdc, gp_sign_bgc);

            pWin->we_rdr->draw_significant_item (hWnd, hdc, &rect,
                    GetWindowElementAttr (hWnd, WE_BGC_SIGNIFICANT_ITEM));
        }

        if (pData->lst_font != INV_LOGFONT) {
            if (plbi->dwFlags & LBIF_BOLDSTYLE)
                pData->lst_font->style |= FS_WEIGHT_BOLD;
            else
                pData->lst_font->style &= ~FS_WEIGHT_BOLD;
        }
        
        x = LST_INTER_BMPTEXT - pData->itemLeft;

        if (dwStyle & LBS_CHECKBOX) {
            checkmark_status = LFRDR_MARK_HAVESHELL;

            rect.left   = x;
            rect.top    = y + ((pData->itemHeight - checkmark_height) >> 1);
            rect.right  = x + checkmark_height; 
            rect.bottom = rect.top + checkmark_height;

            if (plbi->dwFlags & LBIF_CHECKED)
                checkmark_status |= LFRDR_MARK_ALL_SELECTED;
            else if (plbi->dwFlags & LBIF_PARTCHECKED)
                checkmark_status |= LFRDR_MARK_HALF_SELECTED;

            gp_bkcolor = SetBkColor (hdc, gp_bkcolor);

            pWin->we_rdr->draw_checkmark (hdc, &rect,
                    GetWindowElementAttr (hWnd, WE_FGC_WINDOW), 
                    checkmark_status);

            gp_bkcolor = SetBkColor (hdc, gp_bkcolor);

            x += checkmark_height + LST_INTER_BMPTEXT;
        }

        if (dwStyle & LBS_USEICON && plbi->dwImage) {
            if (plbi->dwFlags & LBIF_USEBITMAP) {
                FillBoxWithBitmap (hdc, x, y, 0, pData->itemHeight, 
                            (PBITMAP) plbi->dwImage);
                x += ((PBITMAP)(plbi->dwImage))->bmWidth;
            }
            else {
                int width;
                DrawIcon (hdc, x, y, 0, pData->itemHeight, 
                            (HICON) plbi->dwImage);
                GetIconSize ((HICON) plbi->dwImage, &width, NULL);
                x += width;
            }
            x += LST_INTER_BMPTEXT;
        }

        TextOut (hdc, x, y + ((pData->itemHeight - GetCurFont (hdc)->size) >> 1), plbi->key);

        y += pData->itemHeight;
        plbi = plbi->next;
    }

    SetBkColor (hdc, gp_bkcolor);
}

static void lstDrawFocusRect (HWND hwnd, HDC hdc, PLISTBOXDATA pData)
{ 
	RECT rc;
	PCONTROL pWin;
	DWORD light_dword;
	bk_listctrl_t *bk_listctrl;

	pWin = (PCONTROL) hwnd;
	if (!pWin || !pWin->we_rdr) return;

	bk_listctrl = (bk_listctrl_t *)GetWindowAdditionalData(hwnd);
	if (pData->itemHilighted < pData->itemTop
		|| pData->itemHilighted > (pData->itemTop + pData->itemVisibles))
		return;

	if (pData->dwFlags & LBF_FOCUS) {
		GetClientRect (hwnd, &rc);
		lstGetItemsRect (pData, 
		pData->itemHilighted, pData->itemHilighted,
		&rc);
		InflateRect (&rc, -1, -1);
		if (NULL == bk_listctrl->bk_pbmp || NULL == bk_listctrl->bk_pbmp->bmBits) {
			//fprintf(stderr,"draw_focus_frame!!!\n");
			light_dword = pWin->we_rdr->calc_3dbox_color
			(GetWindowElementAttr (hwnd, WE_BGC_WINDOW), 
			LFRDR_3DBOX_COLOR_LIGHTEST);
			pWin->we_rdr->draw_focus_frame (hdc, &rc, light_dword); 
		} else {
			//fprintf(stderr,"fill box with bit map!!!\n");
			FillBoxWithBitmap(hdc,rc.left,rc.top,RECTW(rc),RECTH(rc),bk_listctrl->bk_pbmp);
		}
	}
}

static int list_box_wndproc(HWND hwnd, int msg, WPARAM wparam, LPARAM lparam)
{
	HDC             hdc;
	PCONTROL        pCtrl;
	PLISTBOXDATA    pData;
	DWORD           dwStyle;
	bk_listctrl_t *bk_listctrl;
    
	pCtrl   = gui_Control(hwnd);
	dwStyle = pCtrl->dwStyle;
	bk_listctrl = (bk_listctrl_t *)GetWindowAdditionalData(hwnd);
	
	switch(msg) {
		case MSG_PAINT:
		{
			RECT rc;

			hdc = BeginPaint (hwnd);
			GetClientRect (hwnd, &rc);			
			pData = (PLISTBOXDATA)pCtrl->dwAddData2;
			if (pData->lst_font != INV_LOGFONT) {
				SelectFont(hdc, pData->lst_font);
			}
			lstOnDrawSListBoxItems (hwnd, hdc, pData, RECTW (rc));
			lstDrawFocusRect (hwnd, hdc, pData);
			
			EndPaint (hwnd, hdc);
		}
			return 0;
		case MSG_DESTROY:
			if (bk_listctrl) {
				free(bk_listctrl);
			}
			break;
	}
	
	return (bk_listctrl->wndproc)(hwnd,msg,wparam,lparam);
}


HWND create_bk_listctl(char *caption, HWND parent, box_t box, DWORD style, int id, PBITMAP bk_pbmp)
{
	HWND hwnd;
	WNDPROC old_wndproc;
	bk_listctrl_t *bk_listctrl;

	bk_listctrl = (bk_listctrl_t *)malloc(sizeof(bk_listctrl_t));
	if (!bk_listctrl) {
		CN_PRINT("no mem!\n");
		return CN_ERR;
	}

	bk_listctrl->bk_pbmp = bk_pbmp;
	//hwnd =  CreateWindow (CTRL_LISTBOX, caption, style, id,box.x, box.y, box.w, box.h, parent, (DWORD)bk_listctrl);
	
	hwnd = CreateWindowEx (CTRL_LISTBOX, caption, style, WS_EX_TRANSPARENT,id,box.x, box.y, box.w, box.h, parent, (DWORD)bk_listctrl);
	
	bk_listctrl->wndproc = SetWindowCallbackProc(hwnd,list_box_wndproc);
	//CN_PRINT("%p",bk_listctrl->wndproc);
	//SetFocusChild(hwnd);
	
	return hwnd;
}

