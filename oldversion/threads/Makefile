CXX = g++
LINKER = g++

SOCKLFLAGS = -lwsock32

UIC = F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\bin\uic
QTLFLAGS 	= -lQt5Core -lQt5Gui -lQt5Widgets

QTLIBPATH = F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\lib
QTINCPATH = F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\include

QTPATH=F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\bin;F:\software\Qt\Qt5.6.3\5.6.3\Tools\mingw49_32\bin

object = Server.o main.o util.o

TARGET = Webserver
# CPPFLAGS = -static

# get $(param) from sublime-build

build: $(TARGET)


$(TARGET):	$(object)
	@echo 正在连接文件...
	$(LINKER) $(object) $(SOCKLFLAGS) -L $(QTLIBPATH) $(QTLFLAGS) -o $@
	@echo 正在运行目标文件 $(TARGET) ...
	start cmd /c "set PATH=$(QTPATH);%PATH% && $(TARGET)< input.txt"

httpserverui.h: httpserver.ui
	@echo 正在编译资源文件 $<
	$(UIC) $< -o $@

Server.o: Server.cpp httpserverui.h
	@echo 正在编译源文件 $< ...
	$(CXX) -c -o $@ $< -I $(QTINCPATH)


%.o : %.cpp
	@echo 正在编译源文件 $< ...
	$(CXX) -c   $< -o $@