#include "Server.h"
#include <iostream>

/**
 * [main ������]
 * ����HTTPServer�� �ڹ��캯�����Զ���ʼ����
 * ����һ��æ�ȴ���������startʱ����HTTPServer���߳�ѭ��
 * ����һ��æ�ȴ���������ABʱ����HTTPServer���߳�
 */
int main(int argc, char *argv[]){
	HTTPServer server;
	string s;
	
	while(true){
		cout << "����ָ��start����HTTPServer����" << endl;
		cin >> s;
		if(s=="start") break;
		else cout << "�ɹ�����HTTP����" << endl;
	}

	server.setMainLoopthread();
	while(1){
		//���ٰ���AB
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
