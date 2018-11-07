
#include "Server.h"
#include "util.h"

using namespace std;

#define BUFFER_SIZE 1000

/**
 * HTTPServer构造函数
 * 参数：无
 * 功能：在构造函数时初始化HTTPServer
 */
HTTPServer::HTTPServer(){
	this->ServerStartup();
}
/**
 * Winsock初始化
 * API
 */
int HTTPServer::WinsockStartup(void){
	WSADATA  wsaData;
	int nRc = WSAStartup(0x0101, & wsaData);
	if(nRc){
		//Winsock初始化错误
		return -1;
	}
	if(wsaData.wVersion != 0x0101){
		//版本支持不够
		//报告错误给用户，清除Winsock，返回
		WSACleanup();
		return -1;
	}
	return 0;
}
/**
 * HTTPServer服务器初始化
 * 功能：设置服务器地址，端口，虚拟路径，winsock初始化
 * 
 */
int HTTPServer::ServerStartup(){
	printf("正在建立HTTP服务器...\n");
	printf("请输入服务器地址\n");
	cin >> LISTEN_ADDR;
	printf("%s\n", LISTEN_ADDR.c_str());
	printf("请输入监听端口\n");
	cin >> LISTEN_PORT;
	printf("%s\n", LISTEN_PORT.c_str());
	printf("请输入虚拟路径\n");
	cin >> ROOT;
	printf("%s\n", ROOT.c_str());

	if(WinsockStartup()==-1){
		printf("WinsockStartup() Failed!\n");
		return -1;
	}
	printf("WinsockStartup() Succeed!\n");
	//创建socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(ListenStartup()==-1) return -1;
	return 0;
}

/**
 * HTTPServer 监听初始化
 * API
 */
int HTTPServer::ListenStartup(){
	//设置地址
	serveraddr.sin_family = AF_INET;
	//htons和htonl函数把主机字节顺序转换为网络字节顺序，分别用于//短整型和长整型数据
	serveraddr.sin_port = htons(atoi(LISTEN_PORT.c_str()));
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// LPSOCKADDR类型转换是必须的
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
 * HTTPServer 主线程主循环
 * 功能：socket一直等待accept（阻塞）当返回Message时创建一个新线程处理它，并且返回时client_info
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

			//buffer存储报文
			char buffer[BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			if(recv(socketClient, buffer, BUFFER_SIZE, 0)==SOCKET_ERROR){
				printf("Recive message failed!\n");
			}else{
				mesp->data = string(buffer);
				DWORD threadid;
				HANDLE mThread = CreateThread(NULL, 0, Message_thread, (LPVOID)mesp, 0, &threadid);  //创建线程01
				CloseHandle(mThread);
			}
		}
	}
}

/**
 * [HTTPServer::Message_thread Message线程]
 * 功能：1：显示当前进程和报文ID 2：显示客户端信息 3：分析报文 4：发送新报文
 * @param  lvParamter [thread形式参数]
 * @return            [thread形式返回值]
 */
DWORD WINAPI HTTPServer::Message_thread(LPVOID lvParamter){
	Message &mes= *(Message *)lvParamter;
	DWORD curid = GetCurrentThreadId();
	printf("===============当前线程ID:%-6lu 当前报文ID:%-6lu================\n", curid,mes.id);
	mes.myserver->Show_ClientInfo(mes);
	mes.myserver->Message_Analysis(mes);
	mes.myserver->Send_Message(mes);
	closesocket(mes.clientSocket);
	delete &mes;
	return 233;
}
/**
 * [HTTPServer::Message_Analysis 报文分析]
 * 功能：分析报文请求行获得一系列信息
 * @param mes [报文message]
 */

void HTTPServer::Message_Analysis(Message &mes){

	char method[5], obj[100], protocal[10];
	//得到方法，对象，协议类型
	sscanf(mes.data.c_str(), "%s %s %s", method, obj, protocal);
	printf("请求行: %s %s %s\n", method, obj, protocal);
	//如果方法是POST 得到两个字段username passwd保存在message里
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
	//如果方法是GET，得到请求的对象并且分析对象的类型，将所有信息保存在message中
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
 * [HTTPServer::Send_Message 发送报文]
 * 功能：
 * statusline 状态行由异常决定
 * 对于GET方法 找到目标文件，如果未找到返回error404文件
 * 对于POST方法 首先将format格式串提取并格式化写入info.html
 * 构造header并且发送
 * 发送对象文件，这个文件以大小为1000的buffer分段发送
 * @param m [报文message]
 */
void HTTPServer::Send_Message(const Message &m){

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    string statusline;	//状态行
    ifstream in;	//对象文件流
    if(m.method=="GET"){
	    in.open(m.path.c_str(), ios_base::in|ios_base::binary);
	    printf("GET 目标文件:%s\n", m.path.c_str());
	    if(!in.is_open()){
	    	cout << "文件未找到 返回404" << endl;
	    	statusline="HTTP/1.1 404 Not Found\n";
	    	in.close();
	    	in.open((ROOT+string("/error404.html")).c_str(), ios_base::in|ios_base::binary);
	    }else{
	    	printf("文件成功找到\n");
	    	statusline="HTTP/1.1 200 OK\n";
	    }
	    
	}
	//将format.html 读作格式化串 写入info.html
	if(m.method == "POST"){
		char formatbuf[1000];
		in.open((ROOT+string("/format.html")).c_str(), ios_base::in);
		streamsize tfilesize=in.seekg(0, ios_base::end).tellg();
		in.seekg(ios_base::beg);
		in.read(formatbuf, tfilesize);
		in.close();
		//format字符串替换
		string format(formatbuf);
		string_replace(format,"$username", m.postinfo.username);
		string_replace(format,"$passwd", m.postinfo.passwd);
		//将字符串写入info.html
		ofstream out((ROOT+string("/info.html")).c_str());
		out << format.c_str() << endl;
		out.close();
		//文件句柄重新打开
		in.open((ROOT+string("/info.html")).c_str(), ios_base::in);
		statusline="HTTP/1.1 200 OK\n";
	}

	//构造状态行
    printf("返回状态行:%s\n", statusline.c_str());
    int filesize=in.seekg(0, ios_base::end).tellg();
    //构造header
    strcpy(buffer, statusline.c_str());
    sprintf(buffer+strlen(buffer), "Content-Type: %s\n", m.filetype.c_str());
    sprintf(buffer+strlen(buffer), "Content-Length: %d\n", filesize);
    sprintf(buffer+strlen(buffer), "Server: Sizheree\n");
    sprintf(buffer+strlen(buffer), "Cache-Control: no-cache\n");
    sprintf(buffer+strlen(buffer), "Connection: close\n");
    strcat(buffer, "\n");

    // printf("SEND:\n%s", buffer);
    //发送header
    if(send(m.clientSocket, buffer, strlen(buffer), 0)==SOCKET_ERROR){
    	printf("Send buffer Failed\n");
    }

    //分段发送对象
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
 * [HTTPServer::Show_ClientInfo 显示客户端信息]
 * @param m [message]
 */
void HTTPServer::Show_ClientInfo(const Message & m){
	SOCKADDR_IN client_info = m.client_info;
	char * client_ipp = (char *)&client_info.sin_addr;
	printf("客户端信息:\tIP地址:");
	for(int i=0;i<4;i++){
		printf(i?".%d":"%d", *(client_ipp+i));
	}
	printf("\t端口:%d\n", client_info.sin_port);
}
/**
 * HTTP 析构函数
 */
HTTPServer::~HTTPServer(){
	printf("正在销毁HTTP服务器\n");
}

