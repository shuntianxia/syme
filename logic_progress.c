#include "logic_progress.h"


int update_progress(lgc_progress_t *pgs, int val)
{	
	pgs->pos += val;
	if (pgs->pos > pgs->range) {
		pgs->pos = pgs->range;
	}
	
	if (pgs->pos < 0) {
		pgs->pos = 0;
	}
	
	return pgs->refresh(pgs->hd,pgs->box);
}

int update_time_progress(lgc_tm_progress_t *tm_pgs)
{
	int pos = 0;
	
	tm_pgs->cur_ms++;
	pos = tm_pgs->cur_ms / tm_pgs->total_ms;

	
}
