
#include "Server.h"

using namespace std;

#define BUFFER_SIZE 1000

HTTPServer::HTTPServer(){
	this->ServerStartup();
}

int HTTPServer::WinsockStartup(void){
	WSADATA  wsaData;
	int nRc = WSAStartup(0x0101, & wsaData);
	if(nRc){
		//Winsock��ʼ������
		return -1;
	}
	if(wsaData.wVersion != 0x0101){
		//�汾֧�ֲ���
		//���������û������Winsock������
		WSACleanup();
		return -1;
	}

	return 0;
}

int HTTPServer::ServerStartup(){
	printf("���ڽ���HTTP������...\n");
	printf("�����������ַ\n");
	cin >> LISTEN_ADDR;
	printf("%s\n", LISTEN_ADDR.c_str());
	printf("����������˿�\n");
	cin >> LISTEN_PORT;
	printf("%s\n", LISTEN_PORT.c_str());
	printf("����������·��\n");
	cin >> ROOT;
	printf("%s\n", ROOT.c_str());

	if(WinsockStartup()==-1){
		printf("WinsockStartup() Failed!\n");
		return -1;
	}
	printf("WinsockStartup() Succeed!\n");
	//����socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(ListenStartup()==-1) return -1;
	return 0;
}

int HTTPServer::ListenStartup(){
	//���õ�ַ
	serveraddr.sin_family = AF_INET;
	//htons��htonl�����������ֽ�˳��ת��Ϊ�����ֽ�˳�򣬷ֱ�����//�����ͺͳ���������
	serveraddr.sin_port = htons(atoi(LISTEN_PORT.c_str()));
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// LPSOCKADDR����ת���Ǳ����
	if(SOCKET_ERROR == bind(srvSocket, (LPSOCKADDR)&serveraddr, sizeof(serveraddr))){
		printf("HTTPServer Bind() failed!\n");
		return -1;
	}

	if(listen(srvSocket, 5) == SOCKET_ERROR){
		printf("HTTPServer listen() failed!\n");
		return -1;
	}
	return 0;
}

void HTTPServer::Main_Loop(void){
	//condition
	DWORD messageid=0;
	while(true){
		printf("\n\nMain_LOOP Listening...\n\n");

		//client_info will return the IP info of client
		SOCKADDR_IN client_info = {0};
		int addrsize=sizeof(client_info);
		SOCKET socketClient = accept(srvSocket, (sockaddr*)&client_info, &addrsize);

		
		if(socketClient == SOCKET_ERROR){
			printf("Accept Failed\n");
			continue;
		}else{
			Message * mesp = new Message;
			mesp->clientSocket = socketClient;
			mesp->id = messageid++;
			mesp->myserver = this;
			mesp->client_info = client_info;

			char buffer[BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			if(recv(socketClient, buffer, BUFFER_SIZE, 0)==SOCKET_ERROR){
				printf("Recive message failed!\n");
			}else{
				mesp->data = string(buffer);
				DWORD threadid;
				HANDLE mThread = CreateThread(NULL, 0, Message_thread, (LPVOID)mesp, 0, &threadid);  //�����߳�01
				CloseHandle(mThread);
			}
		}
	}
}

DWORD WINAPI HTTPServer::Message_thread(LPVOID lvParamter){
	Message &mes= *(Message *)lvParamter;
	DWORD curid = GetCurrentThreadId();
	printf("===============��ǰ�߳�ID:%-6lu ��ǰ����ID:%-6lu================\n", curid,mes.id);
	mes.myserver->Show_ClientInfo(mes);
	mes.myserver->Message_Analysis(mes);
	mes.myserver->Send_Message(mes);
	closesocket(mes.clientSocket);
	delete &mes;
	return 233;
}

void HTTPServer::Message_Analysis(Message &mes){
	// printf("Recive:\n%s", mes.data.c_str());
	char method[5], obj[100], protocal[10];
	sscanf(mes.data.c_str(), "%s %s %s", method, obj, protocal);
	printf("������:%s %s %s\n", method, obj, protocal);

	if(strcmp(method, "GET")==0) mes.method="GET";

	mes.path=ROOT+string(obj);

	string url(obj),postfix;
	for(unsigned i=url.find(".")+1;i<url.length();i++){
		postfix+=url[i];
	}
	
	if(postfix=="html") mes.filetype="text/html";
	else if(postfix=="jpg"||postfix=="png") mes.filetype="image/png";
	else if(postfix=="ico") {
		mes.filetype="x-ico";
		if(strcmp(obj, "/favicon.ico")==0) mes.path=ROOT+string("/google.ico");
	}
	return;
}

void HTTPServer::Send_Message(const Message &m){

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    string statusline;
    ifstream in(m.path.c_str(), ios_base::in|ios_base::binary);

    printf("Ŀ���ļ�:%s\n", m.path.c_str());
    if(!in.is_open()){
    	cout << "�ļ�δ�ҵ� ����404" << endl;
    	statusline="HTTP/1.1 404 Not Found\n";
    	in.close();
    	in.open((ROOT+string("/error404.html")).c_str(), ios_base::in|ios_base::binary);
    }else{
    	printf("�ļ��ɹ��ҵ�\n");
    	statusline="HTTP/1.1 200 OK\n";
    }

    printf("����״̬��:%s\n", statusline.c_str());
    int filesize=in.seekg(0, ios_base::end).tellg();

    strcpy(buffer, statusline.c_str());
    sprintf(buffer+strlen(buffer), "Content-Type: %s\n", m.filetype.c_str());
    sprintf(buffer+strlen(buffer), "Content-Length: %d\n", filesize);
    sprintf(buffer+strlen(buffer), "Server: Sizheree\n");
    sprintf(buffer+strlen(buffer), "Cache-Control: no-cache\n");
    sprintf(buffer+strlen(buffer), "Connection: close\n");
    strcat(buffer, "\n");

    // printf("SEND:\n%s", buffer);

    if(send(m.clientSocket, buffer, strlen(buffer), 0)==SOCKET_ERROR){
    	printf("Send buffer Failed\n");
    }

    in.clear();
    in.seekg(0, ios_base::beg);

	int fileremain=filesize;
	while(fileremain>0){
		int sendwindow = sizeof(buffer) < fileremain ? sizeof(buffer) : fileremain;
		in.read(buffer, sizeof(buffer));
		if(send(m.clientSocket, buffer, sendwindow, 0)==SOCKET_ERROR){
			printf("Send buffer Failed\n");
		}
		fileremain -= sizeof(buffer);
	}
	return;
}

void HTTPServer::Show_ClientInfo(const Message & m){
	SOCKADDR_IN client_info = m.client_info;
	char * client_ipp = (char *)&client_info.sin_addr;
	printf("�ͻ�����Ϣ:\tIP��ַ:");
	for(int i=0;i<4;i++){
		printf(i?".%d":"%d", *(client_ipp+i));
	}
	printf("\t�˿�:%d\n", client_info.sin_port);
}

HTTPServer::~HTTPServer(){
	printf("��������HTTP������\n");
}

