#include <winsock2.h>
#pragma comment(lib, "WS2_32")

#include <windows.h>

#define OutErr(a) cout << (a) << endl << "������룺" << WSAGetLastError() << endl << "�����ļ���" << __FILE__ << endl << "����������" << __LINE__ << endl 
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

//ȫ�ֵ�λ���б�����Ϊ1000�����飬����Ԫ��Ϊָ�����40��SOCKET�������ݵ������ָ�룬�����鱣��ע��õ�λ�Ŀͻ����б�ÿһ�еĵ�һλ��ΪINVALID_SOCKET
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
	
	//��ʼ��winsock
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2,2),&wsd)!=0)
	{
		cout << ("WSAStartup()") << endl << "������룺" << WSAGetLastError() << endl << "�����ļ���" << __FILE__ << endl << "����������" << __LINE__ << endl;
	}
	else
	{
		// ����socket�׽���
		
		if (SOCKET_ERROR == (sServer=socket(AF_INET, SOCK_STREAM, 0)))
		{
			printf("Init Socket Error!\n");
			return -1;
		}
		//�󶨶˿�
		struct sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(6666);
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
		if(bind(sServer, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		{
			cout << ("bind Failed!") << endl << "������룺" << WSAGetLastError() << endl << "�����ļ���" << __FILE__ << endl << "����������" << __LINE__ << endl;
			return -1;
		}
		
		// ���ü�������Ϊ40
		if(listen(sServer, 40) != 0)
		{
			cout << ("listen Failed!") << endl << "������룺" << WSAGetLastError() << endl << "�����ļ���" << __FILE__ << endl << "����������" << __LINE__ << endl;
			return -1;
		}
		printf("��ʼ������\n");
		
		WSAEVENT event = WSACreateEvent();
		WSAEventSelect(sServer,event, FD_ACCEPT|FD_CLOSE);
		eventArray[nEventTotal]= event;
		sockArray[nEventTotal] = sServer;
		nEventTotal++;
	}
	
	while (true)
	{
		//�������¼������ϵȴ�
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
				{//�������������֪ͨ
					
					if (event.iErrorCode[FD_ACCEPT_BIT]==0)
					{
						if (nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
						{
							cout <<"̫�������ˣ�" << endl;
							continue;
						}
						struct sockaddr_in cliAddr;
						int cliAddrSize = sizeof(struct sockaddr_in);
						SOCKET sClient = accept(sServer,(struct sockaddr *)&cliAddr,&cliAddrSize);
						if (sClient == INVALID_SOCKET)
						{
							cout << ("accept failed!") << endl << "������룺" << WSAGetLastError() << endl << "�����ļ���" << __FILE__ << endl << "����������" << __LINE__ << endl;
							return -1;
						}
						printf("�пͻ������ӽ�����\n");
						
						//����ɵõ��ͻ��˵�IP��ַ cliAddr.sin_addr
						ioctlsocket(sClient, FIONBIO, &NonBlock);//������
						//��ӵ�list��
						sClient_info sClient1;
						sClient1.sockClient=sClient;
						sClient_info_list.push_back(sClient1);
						
						WSAEVENT event = WSACreateEvent();
						WSAEventSelect(sClient, event, FD_READ|FD_CLOSE|FD_WRITE);
						// ��ӵ�����
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
				{//���������Ϣ��֪ͨ
					++a;//����һ��fd_read
					cout<< "a "<< a<<endl;
					if(event.iErrorCode[FD_READ_BIT] == 0)
					{//���ж�һ���Ƿ�����Ϣ�ɹ�
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
								cout <<  "������룺" << WSAGetLastError() << endl;
								closesocket(sockArray[i]);

								cout << "Find the client closing."<< endl;
								//��list��ɾȥ�ÿͻ���					
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
						// û���κδ���
						else{
							dots_info struct_dots;
							memcpy(&struct_dots, szText, sizeof(struct_dots));
							//printf("���յ����ݣ�%s \n", szText);
							cout<< "number of dots: "<<struct_dots.length<<endl;
							cout << "length of struct_dots: "<< sizeof(struct_dots)<<endl;
							for (int j=0 ; j<struct_dots.length;j++)
							{
								cout<< struct_dots.dots[j]<<endl;
							}
							//�ѿͻ��˴������ĵ�λ�Ŵ���list�еĵ�λ�����У������˿ͻ��˼��뵽ȫ�ֵ�λ�б��У������һλ�±�Ϊ0��������ʵ�ʵ�λֵ��1
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
				{//�������ӹرյ�֪ͨ
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
						//��list��ɾȥ�ÿͻ���					
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
				{//����FD_WRITE֪ͨ��Ϣ
					b++;
					cout<<"FD_WRITE����һ�� b++"<<b<< endl;
				}
				
			}
			
		}
	}
	WSACleanup();//�ͷ���Դ�Ĳ���
	return 1;
}
