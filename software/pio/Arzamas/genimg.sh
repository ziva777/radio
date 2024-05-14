#!/bin/sh

./img2cpp.py ./img/sprite_1.png > ./include/sprite_1.h
./img2cpp.py ./img/sprite_2.png > ./include/sprite_2.h

# back
IMG=sp_back_am
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# freq big
IMG=sp_freq_big
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# AGC
IMG=sp_agc_on
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_agc_off
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# ATT
IMG=sp_att_on
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_att_off
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# SQL
IMG=sp_sql_on
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_sql_off
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# NR
IMG=sp_nr_on
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_nr_off
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# Font 3x5
IMG=sp_3x5_font
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# AM horizontal scale bar
IMG=sp_am_band_select
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# Mode
IMG=sp_mode
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# STEREO label
IMG=sp_stereo
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# MONO label
IMG=sp_mono
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# Bandwidth
IMG=sp_bandwidth_1
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_bandwidth_1_8
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_bandwidth_2
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_bandwidth_2_5
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_bandwidth_3
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_bandwidth_4
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
IMG=sp_bandwidth_6
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# MUTE
IMG=sp_mute
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# AUX
IMG=sp_aux
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# Vertial bar
IMG=sp_v_bar
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h

# Splash
IMG=sp_splash
./img2cpp.py ./img/$IMG.png > ./include/$IMG.h
