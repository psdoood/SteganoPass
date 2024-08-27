@echo off
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
echo Build completed. Executable and Tests can be found in build\Release\