#ifndef __SERVER_H
#define __SERVER_H

#include <winsock2.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <Windows.h>

#include "util.h"

using namespace std;

class HTTPServer;
/**
 * 报文数据结构Message
 * 注意到为了方便访问存储了客户端对象信息
 * 存储了socket连接
 * HTTPServer 指针
 * 存储POST信息
 * ID
 */
struct Message{
	string data; 	//报文头内容
	string path;	//GET 对象路径
	string filetype;	//GET 对象类型
	string method;	// 请求方法 GET | POST
	SOCKET clientSocket;	// socket连接
	SOCKADDR_IN client_info;	//客户端信息
	HTTPServer * myserver; 
	struct{
		string username;
		string passwd;
	}postinfo;
	int id;
};



class HTTPServer{
public:
	HTTPServer();
	virtual ~HTTPServer(void);
	virtual int WinsockStartup();
	virtual int ServerStartup();
	virtual int ListenStartup();
	void setMainLoopthread(void);
	void closeServer(void);
protected:
	void Main_Loop(void); 		//主循环
	void Message_Analysis(Message &m); 		//报文分析
	void Send_Message(const Message &m); 	//发送新报文，传入报文用于分析
	void Show_ClientInfo(const Message & m);	//显示客户端信息
	static DWORD WINAPI Message_thread(LPVOID lvParamter); 	//静态定义，否则此函数会新增this参数从而无法被API调用
	static DWORD WINAPI MainLoopthread(LPVOID lvParamter);

private:
	SOCKET srvSocket; 	//服务器主socket
	sockaddr_in serveraddr; 	//没用
	int nAddrLen; 		
	string ROOT;		//虚拟地址
	string LISTEN_ADDR;	//监听地址
	string LISTEN_PORT;	//监听端口
	bool ServerState;
	HANDLE mainThread;
};

#endif