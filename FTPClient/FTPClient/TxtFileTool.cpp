#include "pch.h"
#include "TxtFileTool.h"

int TxtFileTool::regist(struct UsrInfo* usr) {//注册程序
	char usrname[20];
	char psword[20];
	strcpy_s(usrname, usr->UsrName);
	strcpy_s(psword, usr->Psword);
	string temp;
	int flag = 0;
	int i = 0;
	ifstream fin(".\\UsrInfo.txt", ios::in);//在这个路径下读入文件
	ofstream fout(".\\UsrInfo.txt", ios::app);//在同一个路径下，如果注册成功则写入文件
	while (std::getline(fin, temp))//每次读一行的数据进入temp中。
	{
		i++;
		if (i % 2 == 0) continue;//访问的是密码这一行，跳出。
		if (!strcmp(usrname, temp.c_str())) flag = 1;//flag=1说明用户名已经被注册了
	}
	fin.close();
	if (flag) {
		return 0;//之前有重复的账户名
	}
	else {//没注册
		fout << usrname << endl;//向文件写入注册者的用户名，然后换一行
		fout << psword << endl;//写入密码，换行
		fout.close();
		return 1;//注册成功
	}
}
int TxtFileTool::login(struct UsrInfo* usr) {
	char usrname[20];
	char psword[20];
	strcpy_s(usrname, usr->UsrName);
	strcpy_s(psword, usr->Psword);
	string temp1;
	string temp2;
	int existname = 0;
	int match = 0;
	int i = 0;
	ifstream fin(".\\UsrInfo.txt", ios::in);
	while (std::getline(fin, temp1))
	{
		std::getline(fin, temp2);//一次读进去两行，分别是用户名和密码
		if (!strcmp(usrname, temp1.c_str())) {//有这个用户名了，接下来看看密码是不是相符的
			existname = 1;
			if (!strcmp(psword, temp2.c_str())) {//相符
				match = 1;
				break;
			}
		}
	}
	fin.close();
	if (!existname) {
		return 2;//没有账户名
	}
	else {
		if (match) return 1;
		else return 3;//用户名和密码不匹配
	}
}