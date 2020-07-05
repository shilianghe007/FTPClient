#include "pch.h"
#include "TxtFileTool.h"

int TxtFileTool::regist(struct UsrInfo* usr) {//ע�����
	char usrname[20];
	char psword[20];
	strcpy_s(usrname, usr->UsrName);
	strcpy_s(psword, usr->Psword);
	string temp;
	int flag = 0;
	int i = 0;
	ifstream fin(".\\UsrInfo.txt", ios::in);//�����·���¶����ļ�
	ofstream fout(".\\UsrInfo.txt", ios::app);//��ͬһ��·���£����ע��ɹ���д���ļ�
	while (std::getline(fin, temp))//ÿ�ζ�һ�е����ݽ���temp�С�
	{
		i++;
		if (i % 2 == 0) continue;//���ʵ���������һ�У�������
		if (!strcmp(usrname, temp.c_str())) flag = 1;//flag=1˵���û����Ѿ���ע����
	}
	fin.close();
	if (flag) {
		return 0;//֮ǰ���ظ����˻���
	}
	else {//ûע��
		fout << usrname << endl;//���ļ�д��ע���ߵ��û�����Ȼ��һ��
		fout << psword << endl;//д�����룬����
		fout.close();
		return 1;//ע��ɹ�
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
		std::getline(fin, temp2);//һ�ζ���ȥ���У��ֱ����û���������
		if (!strcmp(usrname, temp1.c_str())) {//������û����ˣ����������������ǲ��������
			existname = 1;
			if (!strcmp(psword, temp2.c_str())) {//���
				match = 1;
				break;
			}
		}
	}
	fin.close();
	if (!existname) {
		return 2;//û���˻���
	}
	else {
		if (match) return 1;
		else return 3;//�û��������벻ƥ��
	}
}