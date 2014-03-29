#!/bin/sh

w_blender_exe='c:\Program Files\Blender Foundation\Blender\Blender.exe'
c_blender_exe=$(cygpath -u "$w_blender_exe")
w_1=$(cygpath -a -w "$1")

echo "$c_blender_exe" -b "$w_1" -o "$2" -f 0
"$c_blender_exe" -b "$w_1" -o "$2" -f 0
