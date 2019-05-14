#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include<WINSOCK2.H>
#include<STDIO.H>
#include<string>
using namespace std;
class SocketClient {
public:
	SocketClient(string ips) {
		ip = ips;
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA data;
		if (WSAStartup(sockVersion, &data) != 0)
		{
			throw("WSAStartup error");
		}
		
	}
	~SocketClient() {
		try {
			closesocket(sclient);
		}
		catch (exception & e) {}
		WSACleanup();
	}
	void connects() {
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			throw("invalid socket!");
		}
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		if (connect(sclient, (sockaddr*)& serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //¡¨Ω” ß∞‹ 
			closesocket(sclient);
			throw("connect error !");
		}
	}
	void sends(string s) {
		send(sclient, s.c_str(), s.length(), 0);
	}
	string receives() {
		char revData[1024];
		int ret = recv(sclient, revData, 1024, 0);
		revData[ret] = 0x00;
		string s = revData;
		return s;
	}
	void closes() {
		closesocket(sclient);
	}
	string autosends(string s) {
		connects();
		sends(s);
		string rec = receives();
		closes();
		return rec;
	}
private:
	sockaddr_in serAddr;
	SOCKET sclient;
	string ip;
};
