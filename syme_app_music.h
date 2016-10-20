#ifndef _SYME_MUSIC_H
#define _SYME_MUSIC_H

#ifdef _cplusplus
	extern "c" {
#endif

#include "syme_vfs.h"
#include "snd_player.h"

typedef struct {
	box_t box;
	int num;
	float *freq;
	char *bpath;
	BITMAP bmp;

	 HDC bmpToDC;
} spectrum_t;

typedef struct {
	HWND hwnd;
	
	HWND list_hwnd;
	HWND path_hwnd;
	HWND file_info_hwnd;

	HWND img_hwnd;
	HWND song_info_hwnd;
	
	panel_t audio_panel; /*audio file*/

	char *bk_list_bpath;
	BITMAP bk_list_bmp;

	char *path_prefix;

	/*progress ref*/
	HWND progress_hwnd;
	int cur_sec;
	int total_sec;
	int range; /*the range of progress*/


	spectrum_t spt;

	snd_player_t player;

	int screen_w;
	int screen_h;

	BITMAP play_bmp;
	BITMAP pause_bmp;
	box_t pa_box;
	
} music_t;





#ifdef _cplusplus
	}
#endif

#endif

