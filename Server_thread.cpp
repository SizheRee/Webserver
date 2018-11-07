
#include "Server.h"
#include "util.h"

using namespace std;

#define BUFFER_SIZE 1000

/**
 * HTTPServer���캯��
 * ��������
 * ���ܣ��ڹ��캯��ʱ��ʼ��HTTPServer
 */
HTTPServer::HTTPServer(){
	this->ServerStartup();
}
/**
 * Winsock��ʼ��
 * API
 */
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
/**
 * HTTPServer��������ʼ��
 * ���ܣ����÷�������ַ���˿ڣ�����·����winsock��ʼ��
 * 
 */
int HTTPServer::ServerStartup(){
	printf("���ڽ���HTTP������...\n");
	printf("�������������ַ\n");
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

/**
 * HTTPServer ������ʼ��
 * API
 */
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

/**
 * HTTPServer ���߳���ѭ��
 * ���ܣ�socketһֱ�ȴ�accept��������������Messageʱ����һ�����̴߳����������ҷ���ʱclient_info
 */

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

			//buffer�洢����
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

/**
 * [HTTPServer::Message_thread Message�߳�]
 * ���ܣ�1����ʾ��ǰ���̺ͱ���ID 2����ʾ�ͻ�����Ϣ 3���������� 4�������±���
 * @param  lvParamter [thread��ʽ����]
 * @return            [thread��ʽ����ֵ]
 */
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
/**
 * [HTTPServer::Message_Analysis ���ķ���]
 * ���ܣ��������������л��һϵ����Ϣ
 * @param mes [����message]
 */

void HTTPServer::Message_Analysis(Message &mes){

	char method[5], obj[100], protocal[10];
	//�õ�����������Э������
	sscanf(mes.data.c_str(), "%s %s %s", method, obj, protocal);
	printf("������: %s %s %s\n", method, obj, protocal);
	//���������POST �õ������ֶ�username passwd������message��
	if(strcmp(method, "POST")==0){
		// printf("POST:\n%s", mes.data.c_str());
		mes.method="POST";	
		string username;
		mes.postinfo.username.clear();
		mes.postinfo.passwd.clear();
		unsigned pos=mes.data.find("username=")+sizeof("username");
		while(mes.data[pos]!='&'&&mes.data[pos]!='\n'&&mes.data[pos]) mes.postinfo.username+=mes.data[pos++];
		pos=mes.data.find("passwd=")+sizeof("passwd");
		while(mes.data[pos]!='&'&&mes.data[pos]!='\n'&&mes.data[pos]) mes.postinfo.passwd+=mes.data[pos++];
		cout << "POST RECIVE USERNAME:" << mes.postinfo.username << "\tPASSWD:" << mes.postinfo.passwd << endl;
		// printf("POST RECIVE: username:%s passwd:%s\n", mes.postinfo.username.c_str(), mes.postinfo.passwd.c_str());
		return;
	} 
	//���������GET���õ�����Ķ����ҷ�����������ͣ���������Ϣ������message��
	if(strcmp(method, "GET")==0){
		mes.method="GET";
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
	return;
}
/**
 * [HTTPServer::Send_Message ���ͱ���]
 * ���ܣ�
 * statusline ״̬�����쳣����
 * ����GET���� �ҵ�Ŀ���ļ������δ�ҵ�����error404�ļ�
 * ����POST���� ���Ƚ�format��ʽ����ȡ����ʽ��д��info.html
 * ����header���ҷ���
 * ���Ͷ����ļ�������ļ��Դ�СΪ1000��buffer�ֶη���
 * @param m [����message]
 */
void HTTPServer::Send_Message(const Message &m){

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    string statusline;	//״̬��
    ifstream in;	//�����ļ���
    if(m.method=="GET"){
	    in.open(m.path.c_str(), ios_base::in|ios_base::binary);
	    printf("GET Ŀ���ļ�:%s\n", m.path.c_str());
	    if(!in.is_open()){
	    	cout << "�ļ�δ�ҵ� ����404" << endl;
	    	statusline="HTTP/1.1 404 Not Found\n";
	    	in.close();
	    	in.open((ROOT+string("/error404.html")).c_str(), ios_base::in|ios_base::binary);
	    }else{
	    	printf("�ļ��ɹ��ҵ�\n");
	    	statusline="HTTP/1.1 200 OK\n";
	    }
	    
	}
	//��format.html ������ʽ���� д��info.html
	if(m.method == "POST"){
		char formatbuf[1000];
		in.open((ROOT+string("/format.html")).c_str(), ios_base::in);
		streamsize tfilesize=in.seekg(0, ios_base::end).tellg();
		in.seekg(ios_base::beg);
		in.read(formatbuf, tfilesize);
		in.close();
		//format�ַ����滻
		string format(formatbuf);
		string_replace(format,"$username", m.postinfo.username);
		string_replace(format,"$passwd", m.postinfo.passwd);
		//���ַ���д��info.html
		ofstream out((ROOT+string("/info.html")).c_str());
		out << format.c_str() << endl;
		out.close();
		//�ļ�������´�
		in.open((ROOT+string("/info.html")).c_str(), ios_base::in);
		statusline="HTTP/1.1 200 OK\n";
	}

	//����״̬��
    printf("����״̬��:%s\n", statusline.c_str());
    int filesize=in.seekg(0, ios_base::end).tellg();
    //����header
    strcpy(buffer, statusline.c_str());
    sprintf(buffer+strlen(buffer), "Content-Type: %s\n", m.filetype.c_str());
    sprintf(buffer+strlen(buffer), "Content-Length: %d\n", filesize);
    sprintf(buffer+strlen(buffer), "Server: Sizheree\n");
    sprintf(buffer+strlen(buffer), "Cache-Control: no-cache\n");
    sprintf(buffer+strlen(buffer), "Connection: close\n");
    strcat(buffer, "\n");

    // printf("SEND:\n%s", buffer);
    //����header
    if(send(m.clientSocket, buffer, strlen(buffer), 0)==SOCKET_ERROR){
    	printf("Send buffer Failed\n");
    }

    //�ֶη��Ͷ���
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
	in.close();
	return;

}
/**
 * [HTTPServer::Show_ClientInfo ��ʾ�ͻ�����Ϣ]
 * @param m [message]
 */
void HTTPServer::Show_ClientInfo(const Message & m){
	SOCKADDR_IN client_info = m.client_info;
	char * client_ipp = (char *)&client_info.sin_addr;
	printf("�ͻ�����Ϣ:\tIP��ַ:");
	for(int i=0;i<4;i++){
		printf(i?".%d":"%d", *(client_ipp+i));
	}
	printf("\t�˿�:%d\n", client_info.sin_port);
}
/**
 * HTTP ��������
 */
HTTPServer::~HTTPServer(){
	printf("��������HTTP������\n");
}

