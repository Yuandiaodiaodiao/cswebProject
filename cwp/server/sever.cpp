#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include"SocketServer.h"
#include<cstring>
#include<string>
#include<vector>
#include<ctime>
#include<map>
using namespace std;
string opt[5] = {"login", "logout", "send" ,"getmsg","getonline"};
/*
	login logout send getmessage getonline
	login$username space
	logout$username space
	send$from$to$msg space
	getmsg$username from$msg$from$msg$
	getonline$usename username$username...$
*/
struct account
{
	bool isLog;
	int lastTime;
};
map < string ,account > onlineList;
struct Msg
{
	string from,to,msg;
};
vector < Msg > msgList;
int find$(string s,int pos)
{
	int ans = -1;
	for(int i = pos ; i < s.length() ; i ++)
		if(s[i] == '$')
		{
			ans = i;
			break;
		}
	return ans;
}
int check(string req)
{
	int pos = find$(req,0);
	string ins = req.substr(0,pos);
	cout << "#" << ins << endl;
	for(int i = 0 ; i < 5 ; i ++)
		if(ins == opt[i])
			return i;
	return -1;
}
void solve()
{
	SocketServer server;
	SOCKET id = server.accepts();
	string req = server.receives(id);
	int inst = check(req);
	printf("#%d\n",inst);
	if(inst == 0)//login
	{
		int pos = find$(req,0);
		string username = req.substr(pos+1,req.length()-pos-1);
		if(onlineList[username].isLog == true)
			server.sends(id,"Failed: already logged in");
		else
		{
			onlineList[username].isLog = true;
			server.sends(id,"login success");
			puts("login success");
		}
		return ;
	}
	else if(inst == 1)//logout
	{
		int pos = find$(req,0);
		string username = req.substr(pos+1,req.length()-pos-1);
		cout << "#" << username << endl;
		if(onlineList[username].isLog == false)
			server.sends(id,"Failed: user not online");
		else
		{
			onlineList[username].isLog = false;
			server.sends(id,"logout success");
		}
		return ;
	}
	else if(inst == 2)//send
	{
		int pos = find$(req,0);
		int pos_ = find$(req,pos+1);
		int pos__ = find$(req,pos_+1);
		string from = req.substr(pos+1,pos_-pos-1);
		string to = req.substr(pos_+1,pos__-pos_-1);
		string msg = req.substr(pos__+1,req.length()-pos__-1);
		cout << "#" << from << " " << to << " " << msg << endl;
		Msg newmsg;
		newmsg.from = from;
		newmsg.to = to;
		newmsg.msg = msg;
		server.sends(id,"send success");
		msgList.push_back(newmsg);
	}
	else if(inst == 3)//getmsg
	{
		int pos = find$(req,0);
		string username = req.substr(pos+1,req.length()-pos-1);
		vector < Msg > ::iterator it;
		string res = "$";
		
		for(it = msgList.begin() ; it != msgList.end() ;)
		{
			if((*it).to == username)
			{
				res += (*it).from + "$" + (*it).msg + "\n";
				it = msgList.erase(it);
			}
			else
			{
				++ it;
			}
		}
		cout << "#" << res << endl;
		server.sends(id,res);
	}
	else if(inst == 4)//getonline
	{
		int pos = find$(req,0);
		string username = req.substr(pos+1,req.length()-pos-1);
		string res = "$";
		map < string ,account > :: iterator it;
		for(it = onlineList.begin() ; it != onlineList.end() ; ++ it)
		{
			if((it -> second).isLog == true && (it -> first) != username)
				res += it -> first + "$";
		}
		cout << "#" << res << endl;
		server.sends(id,res);
	}
	server.closes(id);
	return ;
}
int main()
{
	while(true)
		solve();
	return 0;
}
