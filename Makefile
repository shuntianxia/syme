APP_NAME = syme

##########compile############
#PREFIX =arm-none-linux-gnueabi-
PREFIX =
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
LD = $(PREFIX)ld
AR = $(PREFIX)ar

##########parameters#########
ifeq ($(PREFIX),)
$(warnning "--------lative--------")
CFLAGS= -I/usr/local/include 
LIBS =  -L/usr/local/lib
else
$(warnning "--------cross--------")
CFLAGS= -I/home/work/minigui/arm_cross/include -D_SUNXI_C500
LIBS =  -L/home/work/minigui/arm_cross/lib 
endif

CPPFLAGS=
CFLAGS += -g -I./sound -I./cdlb
LIBS += -lminigui_ths -ljpeg -lpng -lm -ldl -lz -lpthread -lasound -lavcodec -lavformat -lswresample -lavutil

##########target#############
#OBJS=syme_app_create.o syme_app_set.o syme_icon.o syme_base_item.o \
#		syme_main.o syme_fb.o syme_focus.o syme_background.o syme_app_desktop.o \
#		syme_abstract_focus.o syme_app_filemgr.o syme_vfs.o \
#		syme_bk_listctrl.o cdlb_string.o cdlb_sort.o syme_app_music.o \
#		snd_alsa.o snd_codec.o snd_player.o snd_audio_resampling.o cdlb_alk_fifo.o \
#		spectrum_analyser.o fast_fourier_transform.o syme_app.o \
#		syme_app_set_power.o syme_spin_item.o syme_app_set_page.o \
#		syme_app_set_common.o syme_app_set_volume.o
		
OBJS=syme_app_create.o syme_app.o syme_app_set.o syme_app_desktop.o syme_app_filemgr.o  \
		syme_app_set_volume.o syme_app_set_common.o syme_app_music.o syme_app_set_power.o \
		syme_app_set_page.o syme_app_image.o \
		syme_main.o syme_focus.o syme_background.o syme_icon.o syme_base_item.o syme_abstract_focus.o  \
		syme_bk_listctrl.o  syme_spin_item.o  \
		cdlb/cdlb_string.o cdlb/cdlb_sort.o cdlb/cdlb_alk_fifo.o cdlb/cdlb_fops_lnx.o \
		sound/snd_alsa.o sound/snd_codec.o sound/snd_player.o sound/snd_audio_resampling.o sound/spectrum_analyser.o sound/fast_fourier_transform.o \
		sound/snd_alsa_mixer.o \
		syme_vfs.o syme_fb.o
		
all:$(OBJS)
	$(CC) -o $(APP_NAME) $(OBJS) $(CFLAGS) $(LIBS)
clean:
	find . -name "*.o"  | xargs rm -f
	rm -rf $(APP_NAME)


##########rules##############
%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

	
