object = Server_thread.o main.o util.o
ldlib = -lwsock32
# CPPFLAGS = -static

# get $(param) from sublime-build

build: Webserver

Webserver:	$(object)
	@echo 正在连接文件...
	g++ $(object) $(ldlib) -o "Webserver.exe"
	@echo 运行程序...
	start cmd /c "Webserver < input.txt"

%.o : %.cpp
	@echo 正在编译源文件...  "$<"
	g++ -c  $(CPPFLAGS) $< -o $@