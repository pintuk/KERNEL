#!/bin/bash


for i in `seq 1 2`
do 
	ffmpeg -i video1.mp4 -f null ~ & 
done


