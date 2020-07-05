#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iomanip>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::cerr;
using std::string;
using std::setw;

struct UsrInfo//用户名的账户和密码信息
{
	char UsrName[20];
	char Psword[20];
};


class TxtFileTool
{
private:
	//TxtFileTool();
public:
	int regist(struct UsrInfo* usr);
	int login(struct UsrInfo* usr);
};

