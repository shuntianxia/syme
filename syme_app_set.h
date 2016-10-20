#ifndef _SYME_SET_H
#define _SYME_SET_H

#ifdef _cplusplus
	extern "c" {
#endif

#include "syme_base_item.h"
#include "syme_abstract_focus.h"
#include "syme_app_set_page.h"
#include "syme_app_set_power.h"
#include "syme_app_set_common.h"


#define SET_ITEM_NUM (4)

typedef struct {
	base_item_t *bim_ary[SET_ITEM_NUM];
	base_item_t *bim_focus;
	base_item_ops_t *bim_ops;

	int partial_draw;
	
	int sx; /*start x coordinates*/
	int sy; /*start y coordinates*/
	int hs; /*horizontal spacing*/
	int vs; /*vertical spacing*/
	int rx; /*text on image displacement*/
	char *bmp_path;
	char *fcs_path;
	box_t box;

	//focus_t focus;
	abstract_focus_t fcs;

	int sub_page;
	int sub_id;
	set_page_t page;

	power_set_t ps;
	common_set_t cs;
} set_t;

#ifdef _cplusplus
	}
#endif


#endif

