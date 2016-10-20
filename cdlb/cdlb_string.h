#ifndef _CDLB_STRING_H
#define _CDLB_STRING_H

#ifdef _cplusplus
	extern "c" {
#endif

typedef int (*judge_func)(char ch); 
char *strip_ch(char *str, judge_func func);



#ifdef _cplusplus
	}
#endif

#endif

