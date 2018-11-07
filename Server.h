#ifndef __SERVER_H
#define __SERVER_H

#include <winsock2.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <Windows.h>

using namespace std;

class HTTPServer;

struct Message{
	string data;
	string path;
	string filetype;
	string method;
	SOCKET clientSocket;
	SOCKADDR_IN client_info;
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
	virtual void Main_Loop(void);
protected:
	void Message_Analysis(Message &m);
	void Send_Message(const Message &m);
	void Show_ClientInfo(const Message & m);
	static DWORD WINAPI Message_thread(LPVOID lvParamter);
private:
	SOCKET srvSocket;
	sockaddr_in serveraddr;
	int nAddrLen;
	string ROOT;
	string LISTEN_ADDR;
	string LISTEN_PORT;
};

#endif