#ifndef _MG_COMMON_HEADER_H
#define _MG_COMMON_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


#ifdef _DEBUG
#define CN_PRINT(format, ...) fprintf (stderr,"[%s:%d]"format, __FUNCTION__,__LINE__,## __VA_ARGS__)
#else
#define CN_PRINT(format, ...) 
#endif

#define CN_ERR (-1)
#define CN_OK (0)


#define offset_of(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offset_of(type,member) );})


#define ARRY_SIZE(arry) (sizeof(arry)/sizeof(arry[0]))


//#define MIN(a,b) ((a) > (b) ? (b) : (a))
//#define MAX(a,b) ((a) > (b) ? (a) : (b))

//#define GET_RECT_WIDTH(rc) ((rc).right - (rc).left)
//#define GET_RECT_HEIGHT(rc) ((rc).bottom - (rc).height)


#define CFG_PATH "./configs/"

#ifdef _SUNXI_C500
/*only five key just so so...*/
#define SYME_KEY_ARROW_UP (0x73) //volume +
#define SYME_KEY_ARROW_DOWN (0x72) //volume -
#define SYME_KEY_ARROW_LEFT (0x73) //volume +
#define SYME_KEY_ARROW_RIGHT (0x72) //volume -
#define SYME_KEY_ENTER (0x1c) //enter
#define SYME_KEY_ESC (0x66) //home
#else
#define SYME_KEY_ARROW_UP (0x67)
#define SYME_KEY_ARROW_DOWN (0x6c)
#define SYME_KEY_ARROW_LEFT (0x69)
#define SYME_KEY_ARROW_RIGHT (0x6a)
#define SYME_KEY_ENTER (0x1c)
#define SYME_KEY_ESC (0x1)
#endif



typedef struct _box {
	int x;
	int y;
	int w;
	int h;
} box_t;


#ifdef __cplusplus
}
#endif

#endif


