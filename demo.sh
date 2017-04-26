#!/bin/bash
echo ClearSight Photogrammetric 3D Modeling
echo Start Capturing photos!!
#"C:\Users\Daniel Hull\AppData\Local\Programs\Python\Python36\python.exe" "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/save-jpg.py"
sleep 5

echo Start Agisoft recontruction!!
"C:\Program Files\Agisoft\PhotoScan Pro\photoscan.exe" -r "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/agisoft.py"

echo Start Matlab User Interface...
"C:\Program Files\MATLAB\R2016b\bin\matlab.exe" -nodisplay -nosplash -nodesktop -r "run('C:/Users/Daniel Hull/Documents/Projects/BFPD/matlab_demo.m');"
