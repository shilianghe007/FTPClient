
//���ߣ�ʯ����
//���ڣ�2020-3-21
//ʵ�����ݣ�ͨ��ftp�ϴ��������ļ���ʵ�ֶϵ��������ܣ��Լ��ļ�Ŀ¼�ĸ��ֲ���������¼������־��
#include "pch.h"
#include "core.h"
#include <time.h>
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#pragma comment(lib, "ws2_32.lib")

core::core(const char* u, const char* p, const char* s, bool l) {
	username = u;
	password = p;
	//this->filename = f;
	serverip = s;
	//shuted = false;
	log = l;
	freopen_s(&stream, logFile, "a+", stdout);//�ض�����־�������׷�ӷ�ʽд��
	this->WSAinit();
	this->contact2server();
}
int core::creatFolder(const char foldername[]) {
	/* ���� ��MKD foldername\r\n�� */
	sprintf_s(send_buf, "MKD %s\r\n", foldername);
	/*�ͻ��˷����ļ��д�������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to create a new folder:%s\n", foldername);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}
int core::deleteFolder(const char foldername[]) {
	/* ���� ��RMD foldername\r\n�� */
	sprintf_s(send_buf, "RMD %s\r\n", foldername);
	/*�ͻ��˷����ļ��д�������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to delete a folder:%s\n", foldername);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}

int core::getFolderNow() {
	memset(filepath, 0, size(filepath));//�Ƚ���ǰ������
	/* ���� ��RMD foldername\r\n�� */
	sprintf_s(send_buf, "PWD\r\n");
	/*�ͻ��˷����ļ��д�������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to show the folder where you are right now.\n");
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��257 <path>�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	char filepath[200];
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	int i = 0, j = 0;
	while (recData[i] != '"') {
		i++;
	}
	i++;
	while (recData[i] != '"' && j < 254) {
		filepath[j] = recData[i];
		i++; j++;
	}
	filepath[j] = 0x00;//��Ϊ������
	printLog("FTP:%s\n", recData);
	return NULL;
}
int core::getList(const char foldername[], std::set<fileOrDir*>& dire, std::set<fileOrDir*>& file)
{
	/* ���� ��PASV\r\n�� */
	sprintf_s(send_buf, "PASV\r\n");
	/* �ͻ��˸��߷������ñ���ģʽ */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send PASSIVE MODE to FTPServer: %s", send_buf);
	/*�ͻ��˽��շ���������Ӧ����¿��Ķ˿ںţ�
	* ����Ϊ ��227 Entering passive mode (<h1,h2,h3,h4,p1,p2>)�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);

	//�����ص��¶˿ں���Ϣ�����������ݶ˿ڣ�
	int p[2] = { 0,0 };
	int point = 0;
	for (int i = 0; i < 4; i++) {
		while (recData[point] != ',') { point++; }
		point++;
	}
	for (int i = 0; i < 2; i++) {
		while (recData[point] != ',' && recData[point] != ')' && recData[point] != '>') {
			p[i] = p[i] * 10 + (recData[point] - '0');
			point++;
		}
		point++;
	}
	int port_server_data = p[0] * 256 + p[1];

	serDataAddr.sin_family = AF_INET;
	serDataAddr.sin_port = htons(port_server_data);
	//serDataAddr.sin_addr.S_un.S_addr = inet_addr("182.92.236.19");//������ip
	struct in_addr s2;
	inet_pton(AF_INET, serverip, (void*)&s2);
	serDataAddr.sin_addr.s_addr = s2.s_addr;

	//�ͻ������ӵ� FTP �����������ݶ˿�
	/* ���ӷ������¿������ݶ˿� */
	//connect(data_sock, (struct sockaddr*) & server, sizeof(server));
	/* ���ӵ��������� */
	if (connect(data_sock, (struct sockaddr*) & serDataAddr, sizeof(serDataAddr)) == SOCKET_ERROR)
	{  //����ʧ�� 
		printLog("data socket connect error !");
		closesocket(data_sock);
		return 0;
	}

	/* ���� ��LIST foldername\r\n�� */
	sprintf_s(send_buf, "LIST %s\r\n", foldername);
	/*�ͻ��˷����ļ����ļ��б��ѯ����������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to list folder information:%s\n", foldername);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	int rec_len = recv(data_sock, file_buf, max_length, 0);
	//printLog("FTP:%s\n", file_buf);

	//�����ص��б���Ϣ
	point = 0;
	while (point < rec_len) {
		fileOrDir* f = new fileOrDir();
		bool is_file;
		if (file_buf[point] == 'd') is_file = false;
		else is_file = true;
		for (int i = 0; i < 9; i++) {
			f->quanxian[i] = file_buf[++point];//��ȡȨ��
		}
		point += 29;//�����м�������Ϣ
		int i = 0;
		while (file_buf[point] != ' ') {
			f->size[i++] = file_buf[point++];//��ȡ��С��Ϣ
		}
		point++;//�����ո�
		i = 0;
		bool fenhao = false;
		while (1) {
			if ((f->creatTime[i++] = file_buf[point++]) == ':') {//��ȡ����ʱ����Ϣ
				fenhao = true;
			}
			if (fenhao && (file_buf[point] == ' ')) break;
		}
		point++;
		i = 0;
		while (file_buf[point] != '\r') {
			f->name[i++] = file_buf[point++];//��ȡ������Ϣ
		}

		if (is_file) file.insert(f);
		else dire.insert(f);
		point += 2;
	}
	return 0;
}
int core::gotoFolder(const char foldername[]) {
	/* ���� ��CWD foldername\r\n�� */
	sprintf_s(send_buf, "CWD %s\r\n", foldername);
	/*�ͻ��˷����ļ��д�������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to go to another folder:%s\n", foldername);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}

int core::renameFile(const char f1[], const char f2[]) {
	/* ���� ��RNFR filename\r\n�� */
	sprintf_s(send_buf, "RNFR %s\r\n", f1);
	/*�ͻ��˷����û������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a old filename %s to FTPServer\n", f1);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	/* ���� ��RNTO filename\r\n�� */
	sprintf_s(send_buf, "RNTO %s\r\n", f2);
	/*�ͻ��˷����û������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a new filename %s to FTPServer\n", f2);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}

void core::printLog(const char text[]) {
	// ���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
	time_t now = time(0);
	// �� now ת��Ϊ�ַ�����ʽ
	char dt[100];
	ctime_s(dt, 100, &now);
	printf("%s%s\n", dt, text);
}
void core::printLog(const char text0[], const char text1[]) {
	// ���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
	time_t now = time(0);
	// �� now ת��Ϊ�ַ�����ʽ
	char dt[100];
	ctime_s(dt, 100, &now);
	printf("%s", dt);
	printf(text0, text1);
}

int core::WSAinit() {
	//��ʼ��WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	return 1;
}

int core::contact2server() {
	/* ��ʼ��socket */
	control_sock = socket(AF_INET, SOCK_STREAM, 0);
	data_sock = socket(AF_INET, SOCK_STREAM, 0);
	//hp = gethostbyname(server_name);
	//memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	//server.sin_family = AF_INET;
	//server.sin_port = htons(21);
	serControlAddr.sin_family = AF_INET;
	serControlAddr.sin_port = htons(21);
	//serControlAddr.sin_addr.S_un.S_addr = inet_pton("182.92.236.19");//������ip
	struct in_addr s;
	inet_pton(AF_INET, serverip, (void*)&s);
	serControlAddr.sin_addr.s_addr = s.s_addr;

	/* ���ӵ��������� */
	if (connect(control_sock, (struct sockaddr*) & serControlAddr, sizeof(serControlAddr)) == SOCKET_ERROR)
	{  //����ʧ�� 
		printLog("connect error !\n");
		closesocket(control_sock);
		return 0;
	}
	/* �ͻ��˽��շ������˵�һЩ��ӭ��Ϣ */
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
		printLog(recData);
	}
	//read(control_sock, read_buf, read_len);

	//�嵥 2. �ͻ��˷����û��������룬���� FTP ������
	/* ���� ��USER username\r\n�� */
	sprintf_s(send_buf, "USER %s\r\n", username);
	/*�ͻ��˷����û������������� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send user name %s to FTPServer\n", username);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��331 User name okay, need password.�� */
	//read(control_sock, read_buf, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	/* ���� ��PASS password\r\n�� */
	sprintf_s(send_buf, "PASS %s\r\n", password);
	/* �ͻ��˷������뵽�������� */
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send user passwd %s to FTPServer\n", password);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��230 User logged in, proceed.�� */
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);

	return 1;
}

int core::deletefile(char filename[]) {
	/* ���� ��DELE ***\r\n�� */
	sprintf_s(send_buf, "DELE %s\r\n", filename);
	/* �ͻ��˸��߷�����ɾ��ĳ���ļ� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send DELETE order to FTPServer: %s\n", send_buf);
	//����Ӧ����250
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	if (recData[0] == '2' && recData[1] == '5' && recData[2] == '0') return 1;
	else return 0;
}

int core::downloadOrUploadFile(int mode, char filename[]) {
	//mode��0�����ϴ���1��������
	//filename���ļ�����
	//Ĭ���ϴ�����������ǰĿ¼��������������ǰĿ¼
	//�嵥 3. �÷��������뱻��ģʽ�������ݶ˿ڼ���
	/* ���� ��PASV\r\n�� */
	sprintf_s(send_buf, "PASV\r\n");
	/* �ͻ��˸��߷������ñ���ģʽ */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send PASSIVE MODE to FTPServer: %s", send_buf);
	/*�ͻ��˽��շ���������Ӧ����¿��Ķ˿ںţ�
	* ����Ϊ ��227 Entering passive mode (<h1,h2,h3,h4,p1,p2>)�� */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);

	//�����ص��¶˿ں���Ϣ�����������ݶ˿ڣ�
	int p[2] = { 0,0 };
	int point = 0;
	for (int i = 0; i < 4; i++) {
		while (recData[point] != ',') { point++; }
		point++;
	}
	for (int i = 0; i < 2; i++) {
		while (recData[point] != ',' && recData[point] != ')' && recData[point] != '>') {
			p[i] = p[i] * 10 + (recData[point] - '0');
			point++;
		}
		point++;
	}
	int port_server_data = p[0] * 256 + p[1];

	serDataAddr.sin_family = AF_INET;
	serDataAddr.sin_port = htons(port_server_data);
	//serDataAddr.sin_addr.S_un.S_addr = inet_addr("182.92.236.19");//������ip
	struct in_addr s2;
	inet_pton(AF_INET, serverip, (void*)&s2);
	serDataAddr.sin_addr.s_addr = s2.s_addr;

	//�嵥 4. �ͻ������ӵ� FTP �����������ݶ˿ڲ�����/�ϴ��ļ�
	/* ���ӷ������¿������ݶ˿� */
	//connect(data_sock, (struct sockaddr*) & server, sizeof(server));
	/* ���ӵ��������� */
	if (connect(data_sock, (struct sockaddr*) & serDataAddr, sizeof(serDataAddr)) == SOCKET_ERROR)
	{  //����ʧ�� 
		printLog("data socket connect error !");
		closesocket(data_sock);
		return 0;
	}

	if (mode == 1) {
		//�����ļ���֧�ֶϵ�����
		//��ѯҪ���ص��ļ��ڷ������ϵĴ�С
		sprintf_s(send_buf, "SIZE %s\r\n", filename);
		/* �ͻ��˷�������ӷ������˵õ������ļ��Ĵ�С */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send order to server:%s\n", send_buf);
		/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��213 size�� */
		//read(control_sock, recData, read_len);
		ret = recv(control_sock, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
		}
		printLog("FTP:%s", recData);
		int file_size_server = 0;
		for (int i = 4;; i++) {
			if (recData[i] <= '9' && recData[i] >= '0') {
				file_size_server = file_size_server * 10 + (recData[i] - '0');
			}
			else
			{
				break;
			}
		}
		/* �ͻ��˴����ļ� */
		//FILE file_handle = open(disk_name, CRFLAGS, RWXALL);
		FILE* fin;
		fopen_s(&fin, filename, "ab+");//��д�ɶ�������׷����ʽд
		int size = _filelength(_fileno(fin));

		if (size < file_size_server) {
			/* ���� ��REST offset\r\n�� */
			sprintf_s(send_buf, "REST %ld\r\n", size);
			/* �ͻ��˷�������ָ�������ļ���ƫ���� */
			//write(control_sock, send_buf, strlen(send_buf));
			send(control_sock, send_buf, strlen(send_buf), 0);
			printLog("You send offset order to server:%s\n", send_buf);
			/* �ͻ��˽��շ���������Ӧ�����Ϣ��
			*����Ϊ ��350 Restarting at <position>. Send STORE or RETRIEVE to initiate transfer.�� */
			//read(control_sock, read_buf, read_len);
			ret = recv(control_sock, recData, 255, 0);
			if (ret > 0) {
				recData[ret] = 0x00;
			}
			printLog("FTP:%s", recData);
		}
		/* ���� ��RETR filename\r\n�� */
		sprintf_s(send_buf, "RETR %s\r\n", filename);
		/* �ͻ��˷�������ӷ������������ļ� */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send download order to server:%s\n", send_buf);
		/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��150 Opening data connection.�� */
		//read(control_sock, recData, read_len);
		ret = recv(control_sock, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
		}
		printLog("FTP:%s", recData);

		int rec_len;
		fseek(fin, 0, SEEK_END);
		while (rec_len = recv(data_sock, file_buf, max_length, 0)) {
			/* �ͻ���ͨ���������� �ӷ����������ļ����� */
			//offset += rec_len;
			//printLog("FTP:%s", recData);
			/* �ͻ���д�ļ� */
			//write(file_handle, read_buf, read_len);
			fwrite(file_buf, rec_len, 1, fin);
		}
		/* �ͻ��˹ر��ļ� */
		fclose(fin);
	}
	else if (mode == 0) {
		//�ϴ��ļ���֧�ֶϵ�����
		/* ���� ��SIZE filename\r\n�� */
		//char filename[] = "a.txt";

		//��ѯҪ�ϴ����ļ��ڷ������ϵ����д�С
		sprintf_s(send_buf, "SIZE %s\r\n", filename);
		/* �ͻ��˷�������ӷ������˵õ������ļ��Ĵ�С */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send SIZE order to server:%s\n", send_buf);
		/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��213 size�� */
		//read(control_sock, recData, read_len);
		ret = recv(control_sock, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
		}
		printLog("FTP:%s", recData);
		int file_size_server = 0;
		if (recData[0] == '2' && recData[1] == '1' && recData[2] == '3') {
			//���������д��ļ��������ϵ�����(׷��APPE)
			for (int i = 4;; i++) {
				if (recData[i] <= '9' && recData[i] >= '0') {
					file_size_server = file_size_server * 10 + (recData[i] - '0');
				}
				else
				{
					break;
				}
			}
		}
		/* �ͻ��˴����ļ� */
		//FILE file_handle = open(disk_name, CRFLAGS, RWXALL);
		FILE* fout;
		fopen_s(&fout, filename, "rb");
		int size = _filelength(_fileno(fout));//�����ļ���С

		/* ���� ��STOR/APPE filename\r\n�� */

		if (file_size_server > 0) {
			sprintf_s(send_buf, "APPE %s\r\n", filename);//�ϵ�����
		}
		else {
			sprintf_s(send_buf, "STOR %s\r\n", filename);
		}

		/* �ͻ��˷�������ӷ��������ϴ��ļ� */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send upload order to server:%s\n", send_buf);
		/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��150 OK to send data.�� */
		//read(control_sock, recData, read_len);
		ret = recv(control_sock, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
		}
		printLog("FTP:%s", recData);

		fseek(fout, file_size_server, SEEK_SET);
		for (int i = 0; i < (size - file_size_server) / max_length; i++) {
			fread(file_buf, max_length, 1, fout);
			send(data_sock, file_buf, max_length, 0);
		}
		fread(file_buf, (size - file_size_server) % max_length, 1, fout);
		send(data_sock, file_buf, (size - file_size_server) % max_length, 0);

		//fprintf(fout,"")
		/* �ͻ��˹ر��ļ� */
		fclose(fout);
	}
	else {
		printLog("wrong mode");
		return 0;
	}

	//�嵥 5. �ͻ��˹ر���������,�˳� FTP ���������رտ�������
	/* �ͻ��˹ر��������� */
	closesocket(data_sock);
	/* �ͻ��˽��շ���������Ӧ�����Ϣ������Ϊ ��226 Transfer complete.�� */
	//read(control_sock, recData, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s", recData);
	return 1;
}
int core::closeConnect() {
	/* ���� ��QUIT\r\n�� */
	sprintf_s(send_buf, "QUIT\r\n");
	/* �ͻ��˽��Ͽ���������˵����� */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send QUIT order to server:%s\n", send_buf);
	/* �ͻ��˽��շ���������Ӧ�룬����Ϊ ��200 Closes connection.�� */
	//read(control_sock, recData, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s", recData);
	printLog("=============================================");
	/* �ͻ��˹رտ������� */
	closesocket(control_sock);
	WSACleanup();
	return 1;
}