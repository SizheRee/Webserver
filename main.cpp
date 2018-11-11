#include "Server.h"
#include <iostream>

/**
 * [main 主函数]
 功能 实现HTTPServer开启指令服务
 当开启后快速按下ab键可以关闭服务器
 */
int main(int argc, char *argv[]){
	HTTPServer server;
	string s;
	//开启指令
	while(true){
		cout << "请输入start开启服务" << endl;
		cin >> s;
		if(s=="start") break;
		else cout << "启动指令错误" << endl;
	}

	server.setMainLoopthread();
	while(1){
		//按下AB关闭服务器
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
