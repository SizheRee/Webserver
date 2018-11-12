#include "Server.h"
#include <iostream>

/**
 * [main 主函数]
 * 构造HTTPServer， 在构造函数中自动初始化。
 * 进入一个忙等待，当按下start时开启HTTPServer主线程循环
 * 进入一个忙等待，当按下AB时结束HTTPServer主线程
 */
int main(int argc, char *argv[]){
	HTTPServer server;
	string s;
	
	while(true){
		cout << "输入指令start开启HTTPServer服务" << endl;
		cin >> s;
		if(s=="start") break;
		else cout << "成功开启HTTP服务" << endl;
	}

	server.setMainLoopthread();
	while(1){
		//快速按下AB
		if(GetKeyState('A') & 0x8000/*check if high-order bit is set (1 << 15)*/)
		{
			Sleep(50);
			if(GetKeyState('B') & 0x8000){
				server.closeServer();
				break;
			}
		}
		Sleep(10);
	}
	return 0;
}
