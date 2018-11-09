CXX = g++
LINKER = g++

SOCKLFLAGS = -lwsock32


object = Server.o main.o util.o

TARGET = Webserver

# CPPFLAGS = -static

# get $(param) from sublime-build

build: $(TARGET)


$(TARGET):	$(object)
	@echo 正在连接文件...
	$(LINKER) $(object) $(SOCKLFLAGS) -o $@
	@echo 正在运行目标文件 $(TARGET) ...
	start cmd /c "$(TARGET) < input.txt && pause"

Server.o: Server.cpp
	@echo 正在编译源文件 $< ...
	$(CXX) -c -o $@ $< 

%.o : %.cpp
	@echo 正在编译源文件 $< ...
	$(CXX) -c   $< -o $@