#!/bin/bash
# A sample Bash script, by Ryan
echo Start Capturing Images!
sleep 5
"C:\Program Files\Agisoft\PhotoScan Pro\photoscan.exe" -r "C:/Users/Ruolan Xu/Desktop/BME 436/biophotonics/software/agisoft.py"
"C:\Program Files\MATLAB\R2015b\bin\matlab.exe" -nodisplay -nosplash -nodesktop -r "run('C:\Users\Ruolan Xu\Documents\MATLAB\matlab_start.m');"