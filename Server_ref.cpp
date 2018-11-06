#include <cstdio>
#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

#define HEADER "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=UTF-8\r\n\
Server: tomokazu_v1.0.1\r\n\
Content-Length: %d\r\n\r\n%s\
"
#define HTML "\
<!DOCTYPE html>\
<html>\
    <head>\
		<meta charset = \"utf-8\">\
        <title>tomokazu.com</title>\
    <head>\
    <body>\
        <h1>Hello, world!</h1>\
		<p><b>Everyone wants people they like to be right.</b></p>\
		<p><b>That's why popular people are fucking dumb.</b></p>\
    </body>\
</html>\
"
#define BUFFER_SIZE 2048
#define HOST "127.0.0.1"
string PORT;
string ROOT;

#pragma comment(lib, "WS2_32")

//DWORD  WINAPI  AnswerThread(LPVOID  lparam){
	//SOCKET socketClient = (SOCKET)(LPVOID)lparam; 
	
//}
string getURL(char *buffer){
	string res;
	while(*buffer != ' '){
		res.push_back(*buffer);
	}
	return res;
}

int main()
{
	printf("请指定端口:\n");
	unsigned short PORT; cin >> PORT;
	printf("请指定虚拟路径根目录:\n");
	cin >> ROOT;
    //服务器socket初始化
    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(PORT);

    //初始化socket dll
    WSADATA wsaData;
    if (WSAStartup(0x0002, &wsaData) != 0)
    {
        printf("Init socket dll failed!\n");
        exit(1);
    }
    // 创建socket  SOKET_ERROR == -1
    SOCKET socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR == socketServer)
    {
        printf("Create socket error!\n");
        exit(1);
    }
    // 绑定服务器socket host
    if (SOCKET_ERROR == bind(socketServer, (LPSOCKADDR)&addrServer, sizeof(addrServer)))
    {
        printf("Bind server host failed!\n");
        exit(1);
    }
    // 监听
    if (SOCKET_ERROR == listen(socketServer, 10))
    {
        printf("Listen failed!\n");
        exit(1);
    }
    while (true)
    {
        printf("Listening ... \n");
        sockaddr_in addrClient;
        int nClientAddrLen = sizeof(addrClient);
        SOCKET socketClient = accept(socketServer, (sockaddr*)&addrClient, &nClientAddrLen);
        if (SOCKET_ERROR == socketClient)
        {
            printf("Accept failed!\n");
            break;
        }

		//DWORD dwThreadId;
		//HANDLE hThread=CreateThread(NULL,NULL,AnswerThread,(LPVOID)socketClient,0,&dwThreadId);  
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(socketClient, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("Receive data failed!\n");
			return 0;
		}
		printf("Recv data : \n%s\n", buffer);
		
		string url = ROOT + getURL(buffer);


		// 响应
		//清空缓冲区后装载html文件数据
		memset(buffer, 0, BUFFER_SIZE);
		sprintf_s(buffer, HEADER, strlen(HTML), HTML);
		if (send(socketClient, buffer, strlen(buffer), 0) < 0)
		{
			printf("Send data failed!\n");
			return 0;
		}

		printf("Send data : \n%s\n", buffer);
			closesocket(socketClient);
		}
        
		closesocket(socketServer);
    WSACleanup();
    return 0;
}