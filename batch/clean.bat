@echo off
cd ..
if exist build rmdir /s /q build
mkdir build
echo Build directory cleaned and recreated.