# Makefile for building: hellorect
CXX         = g++
LINKER      = g++
UIC = F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\bin\uic

LFLAGS 	= -lQt5Core -lQt5Gui -lQt5Widgets

OBJECTS     = qttest1.o

QTLIBPATH = F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\lib
QTINCPATH = F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\include

QTPATH=F:\software\Qt\Qt5.6.3\5.6.3\mingw49_32\bin;F:\software\Qt\Qt5.6.3\5.6.3\Tools\mingw49_32\bin


TARGET = QTtar

# Default rule
all: qttest2


$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) -L $(QTLIBPATH) $(LFLAGS)  -o $@
	# @echo 正在运行目标文件 $(TARGET)
	# start cmd /c "set PATH=$(QTPATH);%PATH% && $(TARGET)"

testui.h: test.ui
	@echo 正在编译资源文件 $<
	$(UIC) $< -o $@ 

qttest1.o: qttest1.cpp testui.h
	@echo 正在编译源文件 $<
	$(CXX) -c  -o  $@  $<  -I $(QTINCPATH)

ui_widget.h: ui_widget.ui
	@echo 正在编译资源文件 $<
	$(UIC) $< -o $@ 

qttest2.o: qttest2.cpp qttest2.h ui_widget.h
	@echo 正在编译源文件 $<
	$(CXX) -I $(QTINCPATH) -c -o  $@  $<  

qttest2: qttest2.o
	$(LINKER) qttest2.o -L $(QTLIBPATH) $(LFLAGS) -o $@
	@echo 正在运行目标文件 $@
	start cmd /c "set PATH=$(QTPATH);%PATH% && $@"

clean:
	rm  $(OBJECTS)  hellorect.exe