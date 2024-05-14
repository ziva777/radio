#!/bin/sh

#
# IMG=sp_back_am
#./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

listVar="
sp_3d_venture_font
sp_splash
sp_fm_canvas
sp_am_canvas
sp_pointer
sp_mono
sp_stereo
sp_v_bar
sp_band_select_canvas
sp_band_select_pointer
sp_3x5_font
sp_eh_font
"
for img in $listVar; do
	IMG=$img
	./img2cpp.py ./img/$IMG.png > ./include/sp/$IMG.h
done
