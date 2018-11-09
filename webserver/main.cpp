#include "Server.h"

int main(int argc, char *argv[]){
	HTTPServer server;
	server.setMainLoopthread();
	while(1){
		//按下A关闭服务器
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
