#include "syme_app_set_power.h"
#include "syme_app_set_page.h"


static char *light_opt_val[] = {
	"light one","light two","light three","light four","light five",
};

static char *bklt_opt_val[] = {
	"turn","off","never",
};

static child_option_t light_opt[] = {
	{
		.name = "brightness",
		.val = light_opt_val,
		.nb = ARRY_SIZE(light_opt_val),
		.idx = 0,
		.prompt = "brightness: screen brightness",
	},
	{
		.name = "backlight",
		.val = bklt_opt_val,
		.nb = ARRY_SIZE(bklt_opt_val),
		.idx = 0,
		.prompt = "close backlight : set it's time",
	}
};


int power_set_init(power_set_t *ps, set_page_t *page)
{
	ps->page = page;
	ps->page->title = "power";
	set_page_var_init(ps->page,light_opt,ARRY_SIZE(light_opt));
	
	return 0;
}

int power_set_draw(power_set_t *ps, HDC hdc)
{
	set_page_draw(ps->page, hdc);
	
	return 0;
}


int power_set_destory(power_set_t *ps)
{
	return 0;
}

int power_set_key_event(power_set_t *ps, unsigned short keycode, HWND hwnd)
{
	return set_page_key_event(ps->page,keycode,hwnd);
}


