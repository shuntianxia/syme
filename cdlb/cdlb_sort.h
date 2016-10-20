#ifndef _CDLB_SORT_H
#define _CDLB_SORT_H

#ifdef _cplusplus
extern "c" {
#endif

typedef struct {
    void *arry;
    void *(*get_elem)(void *arry, int offset);
    int (*get_key)(void *elem);
    int (*compare)(int key1, int key2);
    void (*swap)(void *a, void *b);
} quick_sort_t;

void quick_sort(quick_sort_t *qs, int low, int high);



#ifdef _cplusplus
	}
#endif

#endif

