#include <stdio.h>
#include <stdlib.h>

#include "cdlb_string.h"


char *strip_ch(char *str, judge_func func)
{
	char *ptr = NULL;
	char *pre = NULL;
	
	pre = ptr = str;

	while (ptr && ('\0'  != *ptr)) {
		if (func(*ptr)) {
			ptr ++;
			continue;
		}
		
		*pre++ = *ptr++;
	}
	*pre = '\0';
	
	return str;
}

