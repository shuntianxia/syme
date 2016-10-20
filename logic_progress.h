#ifndef _LOGIC_PROGRESS_H
#define _LOGIC_PROGRESS_H

#ifdef _cplusplus
	extern "c" {
#endif


typedef struct {
	int hd; /*handle*/
	int range;
	int pos;
	box_t box;

//	int (*create)();
	int (*refresh)(int hd, box_t box);
//	int (*set)(int hd, int val);
//	int (*destory)(int hd);
} lgc_progress_t;

typedef struct {
	int hd; /*handle*/
	int range;
	int pos;
	box_t box;

	int total_ms;
	int cur_ms;

//	int (*create)();
	int (*refresh)(int hd, box_t box);
//	int (*set)(int hd, int val);
//	int (*destory)(int hd);
} lgc_tm_progress_t;



#ifdef _cplusplus
	}
#endif

#endif

