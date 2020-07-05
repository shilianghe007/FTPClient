#pragma once
#include<WINSOCK2.H>
#include<STDIO.H>
#include<iostream>
#include<string>
#include <ws2tcpip.h>  
#include <io.h>
#include <stdio.h>
#include <set>
//#include <string>
using namespace std;
const int max_length = 1024 * 5;
struct fileOrDir {
	char name[50];
	char creatTime[40];
	char quanxian[9];
	char size[10];
};
class core
{
private:
	char file_buf[max_length];//文件缓冲区
	char send_buf[255];//发送指令缓冲区
	char recData[255];//接受回复缓冲区
	char filepath[255];//路径字符串
	//bool shuted;
	bool log;//为true时将日志输出至日志文件，否则输出至命令行
	const char* username;//ftp用户名
	const char* password;//ftp口令
	//string filename;
	const char* serverip;//服务器ip
	SOCKET control_sock, data_sock;//控制/数据socket
	//hostent* hp;
	sockaddr_in serControlAddr, serDataAddr;
	//memset(&server, 0, sizeof(struct sockaddr_in));
	const char* logFile = "log.txt";//日志文件名
	FILE* stream;
public:
	int WSAinit();//WSA初始化
	int contact2server();//建立与服务器的控制连接
	int downloadOrUploadFile(int mode, char filename[]);//建立数据连接并上传/下载文件
	int deletefile(char filename[]);//删除文件
	int renameFile(const char filename1[], const char filename2[]);//文件重命名
	int creatFolder(const char foldername[]);//创建新文件夹
	int deleteFolder(const char foldername[]);//删除文件夹
	int gotoFolder(const char foldername[]);//跳转至另一目录
	int getFolderNow();//获得当前目录
	int getList(const char foldername[], std::set<fileOrDir*>& dire, std::set<fileOrDir*>& file);//获取制定文件目录下的文件列表
	int closeConnect();//关闭连接，最后善后处理
	core(const char* u, const char* p, const char* s, bool l);
	core() {}
	void printLog(const char text[]);//输出日志
	void printLog(const char text0[], const char text1[]);//输出日志
};

