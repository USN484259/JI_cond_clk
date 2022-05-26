#!/bin/bash
if [ "$EUID" -eq 0 ];
then
	echo "shouldn't be root"
	exit
fi
cd ~/Desktop/JI_cond_clk/
pwd
sleep 10s
nohup xdg-open https://USN484259.github.io > /dev/null &
sleep 30s
java -cp "luaj-jse-3.0.2.jar:JI_cond_clk.jar:jna.jar:./" lua auto_report.lua
read -p paused

