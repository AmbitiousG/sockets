#include <WINSOCK2.H>
#include <windows.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

typedef struct info{
	int length;
	int dots[1000];		
}info;

void main()
{
	int err;
	int number;
	ULONG NonBlock = 1;

	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);//协议库的版本信息
	if (!err)
	{
		printf("客户端嵌套字已经打开!\n");
	}
	else
	{
		printf("客户端的嵌套字打开失败!\n");
		return;//结束
	}
	SOCKET clientSocket=socket(AF_INET,SOCK_STREAM,0);

	//ioctlsocket(clientSocket, FIONBIO, &NonBlock);

	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(6666);
	//bind(clientSocket,(SOCKADDR*)&clientsock_in,strlen(SOCKADDR));//注意第三个参数
	//listen(clientSocket,5);
	if(SOCKET_ERROR==connect(clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR)))
	cout << ("accept failed!") << endl << "出错代码：" << WSAGetLastError() << endl << "出错文件：" << __FILE__ << endl << "出错行数：" << __LINE__ << endl;//开始连接
	else printf("success!\n");
	char receiveBuf[100];
	/* recv(clientSocket,receiveBuf,101,0);
printf("%s\n",receiveBuf);*/
	
	info struct_dots;
	cout<<"input dots(end with 0):"<<endl;
	for (number=0 ; number<1000;number++)
	{
		int j;
		cin>>j;
		if(j==0) break;
		struct_dots.dots[number]=j;
	}
	struct_dots.length=number;
	cout<< "number of dots: "<<struct_dots.length<<endl;
	cout << "length of struct_dots: "<< sizeof(struct_dots)<<endl;
	for (int i=0 ; i<struct_dots.length;i++)
	{
		cout<< struct_dots.dots[i]<<endl;
	}
	
	char msg[10240];
	memcpy(msg, &struct_dots, sizeof(struct_dots));
	
	send(clientSocket,msg,sizeof(struct_dots),0); 
	Sleep(100000);
	closesocket(clientSocket);
	WSACleanup();
}