#include <winsock2.h>
#pragma comment(lib, "WS2_32")

#include <windows.h>

#define OutErr(a) cout << (a) << endl << "出错代码：" << WSAGetLastError() << endl << "出错文件：" << __FILE__ << endl << "出错行数：" << __LINE__ << endl 
#define OutMsg(a) cout << (a) << endl;

#pragma warning(disable: 4786)
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

typedef struct{
	SOCKET sockClient;
	int num;
	int dot[1000];
}sClient_info,*p_sClient_info;

typedef struct info{
	int length;
	int dots[1000];		
}dots_info;

list<sClient_info> sClient_info_list;
list<sClient_info>::iterator sClient_iterator;

int a=0;
int b=0;

//全局点位号列表：长度为1000的数组，数组元素为指向包含40个SOCKET类型数据的数组的指针，该数组保存注册该点位的客户端列表。每一行的第一位均为INVALID_SOCKET
SOCKET all_dots[1000][30];

int main(int argc, char* argv[])
{
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET  sockArray[WSA_MAXIMUM_WAIT_EVENTS];
	int nEventTotal = 0;
	
	ULONG NonBlock = 1;
	SOCKET sServer;
	
	for (int j=0;j<1000;j++)
	{
		all_dots[j][0]=INVALID_SOCKET;
	}
	
	//初始化winsock
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2,2),&wsd)!=0)
	{
		cout << ("WSAStartup()") << endl << "出错代码：" << WSAGetLastError() << endl << "出错文件：" << __FILE__ << endl << "出错行数：" << __LINE__ << endl;
	}
	else
	{
		// 创建socket套接字
		
		if (SOCKET_ERROR == (sServer=socket(AF_INET, SOCK_STREAM, 0)))
		{
			printf("Init Socket Error!\n");
			return -1;
		}
		//绑定端口
		struct sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(6666);
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
		if(bind(sServer, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		{
			cout << ("bind Failed!") << endl << "出错代码：" << WSAGetLastError() << endl << "出错文件：" << __FILE__ << endl << "出错行数：" << __LINE__ << endl;
			return -1;
		}
		
		// 设置监听队列为40
		if(listen(sServer, 40) != 0)
		{
			cout << ("listen Failed!") << endl << "出错代码：" << WSAGetLastError() << endl << "出错文件：" << __FILE__ << endl << "出错行数：" << __LINE__ << endl;
			return -1;
		}
		printf("开始监听！\n");
		
		WSAEVENT event = WSACreateEvent();
		WSAEventSelect(sServer,event, FD_ACCEPT|FD_CLOSE);
		eventArray[nEventTotal]= event;
		sockArray[nEventTotal] = sServer;
		nEventTotal++;
	}
	
	while (true)
	{
		//在所有事件对象上等待
		int nIndex = WSAWaitForMultipleEvents(nEventTotal,eventArray,false,WSA_INFINITE,false);
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		for (int i=nIndex;i<nEventTotal;i++)
		{				
			nIndex = WSAWaitForMultipleEvents(1,&eventArray[i],true,1000,false);
			if ((nIndex==WSA_WAIT_FAILED)||(nIndex ==WSA_WAIT_TIMEOUT))
			{
				continue;
			}
			else
			{
				WSANETWORKEVENTS event;
				WSAEnumNetworkEvents(sockArray[i],eventArray[i],&event);
				if (event.lNetworkEvents & FD_ACCEPT)
				{//处理连接请求的通知
					
					if (event.iErrorCode[FD_ACCEPT_BIT]==0)
					{
						if (nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
						{
							cout <<"太多连接了！" << endl;
							continue;
						}
						struct sockaddr_in cliAddr;
						int cliAddrSize = sizeof(struct sockaddr_in);
						SOCKET sClient = accept(sServer,(struct sockaddr *)&cliAddr,&cliAddrSize);
						if (sClient == INVALID_SOCKET)
						{
							cout << ("accept failed!") << endl << "出错代码：" << WSAGetLastError() << endl << "出错文件：" << __FILE__ << endl << "出错行数：" << __LINE__ << endl;
							return -1;
						}
						printf("有客户端连接进来！\n");
						
						//这里可得到客户端的IP地址 cliAddr.sin_addr
						ioctlsocket(sClient, FIONBIO, &NonBlock);//非阻塞
						//添加到list中
						sClient_info sClient1;
						sClient1.sockClient=sClient;
						sClient_info_list.push_back(sClient1);
						
						WSAEVENT event = WSACreateEvent();
						WSAEventSelect(sClient, event, FD_READ|FD_CLOSE|FD_WRITE);
						// 添加到表中
						eventArray[nEventTotal] = event;
						sockArray[nEventTotal] = sClient;
						nEventTotal++;
						cout << "No. of client: " << sClient_info_list.size() <<" size of array: "<<nEventTotal<<endl;
						/* cout << "sockList"<< endl;
						for (sClient_iterator=sClient_info_list.begin();sClient_iterator!=sClient_info_list.end();sClient_iterator++)
						{
							cout << (*sClient_iterator).sockClient <<endl;
						}						
						cout << "sockArray:" <<endl;
						
						for (int k=0;k<nEventTotal;k++)
						{
							cout << sockArray[k]<< endl;
						}
						cout<< endl; */
					}
				}
				else if (event.lNetworkEvents & FD_READ)
				{//处理接收消息的通知
					++a;//触发一次fd_read
					cout<< "a "<< a<<endl;
					if(event.iErrorCode[FD_READ_BIT] == 0)
					{//需判断一下是否处理消息成功
						//.......
						//ProcessMsg(sockArray[i],cBuff);
						char szText[10240];
						memset(szText, 0, 10240);
						int nRecv = recv(sockArray[i], szText, 10240, 0);
						
						cout <<"nRecv " <<nRecv<<endl;
						if (nRecv == 0 || (nRecv == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
						{
							if(WSAGetLastError() != WSAEWOULDBLOCK)
							{
								cout <<  "出错代码：" << WSAGetLastError() << endl;
								closesocket(sockArray[i]);

								cout << "Find the client closing."<< endl;
								//从list中删去该客户端					
								for (sClient_iterator=sClient_info_list.begin();sClient_iterator!=sClient_info_list.end();sClient_iterator++)
								{
									if((*sClient_iterator).sockClient==sockArray[i])
									{
										cout << "Clinet " << i << " closed."<< endl;
										sClient_info_list.erase(sClient_iterator);
										break;
									}
								}
								
								for (int j=i;j<nEventTotal-1;j++)
								{
									sockArray[j]=sockArray[j+1];
									eventArray[j] = eventArray[j+1];
								}
								nEventTotal--;
								cout<< sClient_info_list.size() <<" client in the list. " << nEventTotal <<" socket in the array." << endl;
								break;
							}
						}
						// 没出任何错误
						else{
							dots_info struct_dots;
							memcpy(&struct_dots, szText, sizeof(struct_dots));
							//printf("接收到数据：%s \n", szText);
							cout<< "number of dots: "<<struct_dots.length<<endl;
							cout << "length of struct_dots: "<< sizeof(struct_dots)<<endl;
							for (int j=0 ; j<struct_dots.length;j++)
							{
								cout<< struct_dots.dots[j]<<endl;
							}
							//把客户端传过来的点位号传到list中的点位数组中，并将此客户端加入到全局点位列表中，数组第一位下标为0，所以与实际点位值差1
							for (sClient_iterator=sClient_info_list.begin();sClient_iterator!=sClient_info_list.end();sClient_iterator++)
							{
								if((*sClient_iterator).sockClient==sockArray[i])
								{
									cout << "find the client in the list"<< endl;
									(*sClient_iterator).num=struct_dots.length;
									for (int j=0 ,tmp_dot=0; j<(*sClient_iterator).num; j++)
									{
										tmp_dot=(*sClient_iterator).dot[j]=struct_dots.dots[j];
										for (int k=0; k<30; k++)
										{
											if (all_dots[tmp_dot-1][k]==INVALID_SOCKET)
											{
												all_dots[tmp_dot-1][k]=sockArray[i];
												all_dots[tmp_dot-1][k+1]=INVALID_SOCKET;
												break;
											}
										}
									}									
									break;
								}
							}
							cout<<"all_dots:"<<endl;
							for (int j=0;j<1000;j++)
							{
								if (all_dots[j][0]!=INVALID_SOCKET)
								{
									cout<<"dot "<<j+1<<" ";
									for (int k=0;k<30;k++)
									{									
										if (all_dots[j][k]==INVALID_SOCKET)
										{
											cout<<all_dots[j][k]<<" ";
											break;
										}
										else cout<<all_dots[j][k]<<" ";
									}
									cout<<endl;
								}
							}
							/* cout << "sockList dots:"<< endl;
							for (sClient_iterator=sClient_info_list.begin();sClient_iterator!=sClient_info_list.end();sClient_iterator++)
							{
								for (int j=0 ; j<(*sClient_iterator).num; j++)
								{
									cout<<(*sClient_iterator).dot[j]<< endl;
								}
								cout<<endl;
							} */
						}
					}
				}				
				else if (event.lNetworkEvents & FD_CLOSE)
				{//处理连接关闭的通知
					if(event.iErrorCode[FD_CLOSE_BIT] == 0)
					{
						closesocket(sockArray[i]);
						
						/* cout << "sockList:"<< endl;
						for (sClient_iterator=sClient_info_list.begin();sClient_iterator!=sClient_info_list.end();sClient_iterator++)
						{
							cout << (*sClient_iterator).sockClient <<endl;
						}						
						cout << "sockArray:" <<endl;
						
						for (int k=0;k<nEventTotal;k++)
						{
							cout << sockArray[k]<< endl;
						}
						cout<< endl; */
						
						cout << "Find the client closing."<< endl;
						//从list中删去该客户端					
						for (sClient_iterator=sClient_info_list.begin();sClient_iterator!=sClient_info_list.end();sClient_iterator++)
						{
							if((*sClient_iterator).sockClient==sockArray[i])
							{
								cout << "Clinet " << i << " closed."<< endl;
								for (int j=0,tmp_dot=0; j<(*sClient_iterator).num; j++)
								{
									tmp_dot=(*sClient_iterator).dot[j];
									for (int k=0; k<30; k++)
									{
										if (all_dots[tmp_dot-1][k]==sockArray[i])
										{
											for (;k<29;k++)
											{
												all_dots[tmp_dot-1][k]=all_dots[tmp_dot-1][k+1];
											}
											break;
										}
									}
								}
								sClient_info_list.erase(sClient_iterator);
								break;
							}
						}
						
						cout<<"all_dots:"<<endl;
						for (int j=0;j<1000;j++)
						{
							if (all_dots[j][0]!=INVALID_SOCKET)
							{
								cout<<"dot "<<j+1<<" ";
								for (int k=0;k<30;k++)
								{									
									if (all_dots[j][k]==INVALID_SOCKET)
									{
										cout<<all_dots[j][k]<<" ";
										break;
									}
									else cout<<all_dots[j][k]<<" ";
								}
								cout<<endl;
							}
						}
						
						for (int j=i;j<nEventTotal-1;j++)
						{
							sockArray[j]=sockArray[j+1];
							eventArray[j] = eventArray[j+1];
						}
						nEventTotal--;
						
						cout<< sClient_info_list.size() <<" client in the list. " << nEventTotal <<" socket in the array." << endl;
					}
				}
				else if (event.lNetworkEvents & FD_WRITE)
				{//处理FD_WRITE通知消息
					b++;
					cout<<"FD_WRITE触发一次 b++"<<b<< endl;
				}
				
			}
			
		}
	}
	WSACleanup();//释放资源的操作
	return 1;
}
