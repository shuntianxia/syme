#ifndef _SYME_SPIN_ITEM_H
#define _SYME_SPIN_ITEM_H

#ifdef _cplusplus
	extern "c" {
#endif

#include "mg_common_header.h"

#if 0
typedef struct {
	int idx;
	int spin_num;
	int space; /*the spacing between text and image*/
	int bmp_num;
	
	base_item_t bim;
} spin_item_t;
#endif
	
#define MAX_BMP_NB (2)

typedef struct {
	int idx;
//	int bmp_num;
	
	int spin_num;
	char **text;
	char *img_path[MAX_BMP_NB];
	PBITMAP pbmp;
	
	int space;
	box_t text_box;
} spin_item_t;


#ifdef _cplusplus
	}
#endif

#endif
