#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>  
#include <winsock2.h>  
#include<iostream>
#include<string>
#pragma comment(lib,"ws2_32.lib")  
using namespace std;

class SocketServer {
public:
	SocketServer() {
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			throw("error wsastartup");
		}
		//创建套接字  
		slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (slisten == INVALID_SOCKET)
		{
			throw("socket error !");
		}
		//绑定IP和端口  
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(8888);
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(slisten, (LPSOCKADDR)& sin, sizeof(sin)) == SOCKET_ERROR)
		{
			throw("bind error !");
		}

		//开始监听  
		if (listen(slisten, 5) == SOCKET_ERROR)
		{
			throw("listen error !");
		}

		//循环接收数据  
		SOCKET sClient;
		
		char revData[255];
	}   

	~SocketServer() {
		closesocket(slisten);
		WSACleanup();
	}
	SOCKET accepts() {
		sockaddr_in remoteAddr;
		int nAddrlen = sizeof(remoteAddr);
		SOCKET sClient= accept(slisten, (SOCKADDR*)& remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			throw("accept error !");
		}
		cout<<"from ip"<<inet_ntoa(remoteAddr.sin_addr)<<endl;

		return sClient;
	}
	string receives(SOCKET sClient) {
		char revData[1024];
		int ret = recv(sClient, revData, 1024, 0);
		revData[ret] = 0x00;
		string s = revData;
		return s;
	}
	void sends(SOCKET sClient, string s) {
		send(sClient, s.c_str(), s.length(), 0);
	}
	void closes(SOCKET sClient) {
		closesocket(sClient);
	}


private:
	SOCKET slisten; //套接字

};