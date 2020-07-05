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
	char file_buf[max_length];//�ļ�������
	char send_buf[255];//����ָ�����
	char recData[255];//���ܻظ�������
	char filepath[255];//·���ַ���
	//bool shuted;
	bool log;//Ϊtrueʱ����־�������־�ļ������������������
	const char* username;//ftp�û���
	const char* password;//ftp����
	//string filename;
	const char* serverip;//������ip
	SOCKET control_sock, data_sock;//����/����socket
	//hostent* hp;
	sockaddr_in serControlAddr, serDataAddr;
	//memset(&server, 0, sizeof(struct sockaddr_in));
	const char* logFile = "log.txt";//��־�ļ���
	FILE* stream;
public:
	int WSAinit();//WSA��ʼ��
	int contact2server();//������������Ŀ�������
	int downloadOrUploadFile(int mode, char filename[]);//�����������Ӳ��ϴ�/�����ļ�
	int deletefile(char filename[]);//ɾ���ļ�
	int renameFile(const char filename1[], const char filename2[]);//�ļ�������
	int creatFolder(const char foldername[]);//�������ļ���
	int deleteFolder(const char foldername[]);//ɾ���ļ���
	int gotoFolder(const char foldername[]);//��ת����һĿ¼
	int getFolderNow();//��õ�ǰĿ¼
	int getList(const char foldername[], std::set<fileOrDir*>& dire, std::set<fileOrDir*>& file);//��ȡ�ƶ��ļ�Ŀ¼�µ��ļ��б�
	int closeConnect();//�ر����ӣ�����ƺ���
	core(const char* u, const char* p, const char* s, bool l);
	core() {}
	void printLog(const char text[]);//�����־
	void printLog(const char text0[], const char text1[]);//�����־
};

