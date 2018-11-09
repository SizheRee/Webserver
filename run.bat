@echo off

set arg=^
echo  %arg% > input2.txt

set arg=127.0.0.1
set "arg=%arg% 5051"
set "arg=%arg% %CD:\=/%/Source"

echo %arg% >> input2.txt

WebServer.exe < input2.txt

pause