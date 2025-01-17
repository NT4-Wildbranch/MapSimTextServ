cd src
md build
cd build
cmake ..
cmake --build .
cd..
copy build\debug\MapSimTextServ.exe ..\bin\MapSimTextServ.exe
pause