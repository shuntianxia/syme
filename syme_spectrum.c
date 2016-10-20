
typedef enum {
	SPT_PLAY,
	SPT_PAUSE,
	SPT_STOP,
} spt_status_e;

typedef struct {
	spt_status_e status;
	int delay_tm;
	int elapsed_tm;
	
	void *ctx;
	int (*draw_frame)(void *ctx);
	int (*notify)(void *ctx);
} spectrum_t;


typedef struct {
	box_t box;
	int gap;
	int meter_width; 
	//int meter_num;

	HDC hdc;
	HWND hwnd;
	BITMAP bmp;
} spt_ctx_t;

static int spt_draw_frame(void *ctx)
{
	int i = 0;
	int x,y,w,h;
	int meter_num = 0;
	
	spt_ctx_t *sctx = (spt_ctx_t *)ctx;

	meter_num = sctx->box.x / (sctx.meter_width + sctx.gap);

	for (i=0; i<meter_num; i++) {
		//y = ;
		x = i * (sctx.meter_width + sctx.gap);
		FillBoxWithBitmap(sctx->hdc,x,y,sctx->meter_width,sctx->box.h,sctx->bmp);
	}
	return CN_OK;
}

static int spt_notify(void *ctx)
{
	RECT rc;
	spt_ctx_t *sctx = (spt_ctx_t *)ctx;
	
	rc.left = sctx->box.x;
	rc.top = sctx->box.y;
	rc.right = rc.left + sctx->box.w;
	rc.bottom = rc.top + sctx->box.h;
	
	InvalidateRect(sctx->hwnd,&rc,1);
	return CN_OK;
}



/*************************************************api****************************************************/

int spectrum_init(spectrum_t *spt, void *ctx)
{
	spt->delay_tm = 10;
	spt->ctx = ctx;
	spt->draw_frame = spt_draw_frame;
	spt->notify = spt_notify;

	return CN_OK;
}

int spectrum_start(spectrum_t *spt)
{
	spt->status = SPT_PLAY;
	return CN_OK;
}

int spectrum_update(spectrum_t *spt)
{
	if (SPT_PLAY != spt->status) {
		goto lab_out;
	}
	
	spt->elapsed_tm++;
	if (spt->elapsed_tm < spt->delay_tm) {
		goto lab_out;
	}
	
	spt->draw_frame(spt->ctx);
	spt->notify(spt->ctx);
	spt->elapsed_tm = 0;

lab_out:
	return CN_OK;
}

int spectrum_stop(spectrum_t *spt)
{
	spt->status = SPT_STOP;
	return CN_OK;
}

int spectrum_destory(spectrum_t *spt)
{
	
	return CN_OK;
}

