#include <winsock2.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#define BUFFER_SIZE 1000
#define HOST "127.0.0.1"

int WinscokStartup(void){
	WSADATA  wsaData;
	int nRc = WSAStartup(0x0101, & wsaData);
	if(nRc)
	{
		//Winsock初始化错误
		return -1;
	}
	if(wsaData.wVersion != 0x0101)
	{
		//版本支持不够
		//报告错误给用户，清除Winsock，返回
		WSACleanup();
		return -1;
	}
	return 0;
}

struct Message{
	string path;
	string type;
	int method;
	int clientSocket;
	int messageid;
};

string ROOT;
string LISTEN_ADDR;
string LISTEN_PORT;

void Message_Analysis(const char *s, struct Message &m);
void Send_Message(SOCKET & socketClient, struct Message &m);

void ServerStartup(){
	int res;
	res=WinscokStartup();
	printf("请输入监听地址\n");
	cin >> LISTEN_ADDR;
	printf("%s\n", LISTEN_ADDR.c_str());
	printf("请输入监听端口\n");
	cin >> LISTEN_PORT;
	printf("%s\n", LISTEN_PORT.c_str());
	printf("请输入虚拟路径\n");
	cin >> ROOT;
	printf("%s\n", ROOT.c_str());

	if(res==-1){
		printf("Error in WinsockStartup()\n");
		return;
	}

	SOCKET sServSock;
	sockaddr_in addr;
	//创建socket
	sServSock = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	//htons和htonl函数把主机字节顺序转换为网络字节顺序，分别用于//短整型和长整型数据
	addr.sin_port = htons(atoi(LISTEN_PORT.c_str()));
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// LPSOCKADDR类型转换是必须的
	if(SOCKET_ERROR == bind(sServSock, (LPSOCKADDR)&addr, sizeof(addr))){
		printf("Bind server host failed\n");
	}

	if(listen(sServSock, 5) == SOCKET_ERROR){
		printf("Error in listen()\n");
		return;
	}

	while(true){
		printf("Listening\n");
		sockaddr_in addrClient;
        int nClientAddrLen = sizeof(addrClient);

		SOCKET socketClient = accept(sServSock, (sockaddr *)&addr, &nClientAddrLen);

		if(socketClient == SOCKET_ERROR){
			printf("Accept Failed\n");
			continue;
		}else{
			char buffer[BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			Message m;
			if(recv(socketClient, buffer, BUFFER_SIZE, 0)==SOCKET_ERROR){
				printf("Recive Failed!\n");
			}else{
				Message_Analysis(buffer, m);
				printf("Recive:\n%s", buffer);
			}

			Send_Message(socketClient, m);

			closesocket(socketClient);
		}
		
	}
}

void Message_Analysis(const char *s, struct Message &m){
	char method[5], obj[100], protocal[10];

	sscanf(s, "%s %s %s", method, obj, protocal);
	if(strcmp(method, "GET")==0) m.method=0;

	m.path=ROOT+string(obj);

	string url(obj),postfix;
	for(int i=url.find(".")+1;i<url.length();i++){
		postfix+=url[i];
	}
	
	if(postfix=="html") m.type="text/html";
	else if(postfix=="jpg"||postfix=="png") m.type="image/png";
	else if(postfix=="ico") {
		m.type="x-ico";
		if(strcmp(obj, "/favicon.ico")==0) m.path=ROOT+string("/google.ico");
	}
	return;
}

void Send_Message(SOCKET & socketClient, struct Message &m){
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ifstream in(m.path.c_str(), ios_base::in|ios_base::binary);

    if(!in.is_open()){
    	cout << "File Open Failed!" << endl;
    	return;
    }

    int filesize=in.seekg(0, ios_base::end).tellg();

    strcpy(buffer, "HTTP/1.1 200 OK\n");
    sprintf(buffer+strlen(buffer), "Content-Type: %s\n", m.type.c_str());
    sprintf(buffer+strlen(buffer), "Content-Length: %d\n", filesize);
    sprintf(buffer+strlen(buffer), "Server: Sizheree\n");
    sprintf(buffer+strlen(buffer), "Cache-Control: no-cache\n");
    strcat(buffer, "\n");

    printf("SEND:\n%s", buffer);

    if(send(socketClient, buffer, strlen(buffer), 0)==SOCKET_ERROR){
    	printf("Send buffer Failed\n");
    }

    in.clear();
    in.seekg(0, ios_base::beg);

	int fileremain=filesize;
	while(fileremain>0){
		int sendwindow = sizeof(buffer) < fileremain ? sizeof(buffer) : fileremain;
		in.read(buffer, sizeof(buffer));
		if(send(socketClient, buffer, sendwindow, 0)==SOCKET_ERROR){
			printf("Send buffer Failed\n");
		}
		fileremain -= sizeof(buffer);
	}
	return;
}

int main(){
	ServerStartup();
	return 0;
}
