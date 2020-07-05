
//作者：石亮禾
//日期：2020-3-21
//实现内容：通过ftp上传、下载文件，实现断点续传功能，以及文件目录的各种操作，并记录操作日志。
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
	freopen_s(&stream, logFile, "a+", stdout);//重定向日志输出，以追加方式写入
	this->WSAinit();
	this->contact2server();
}
int core::creatFolder(const char foldername[]) {
	/* 命令 ”MKD foldername\r\n” */
	sprintf_s(send_buf, "MKD %s\r\n", foldername);
	/*客户端发送文件夹创建命令到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to create a new folder:%s\n", foldername);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}
int core::deleteFolder(const char foldername[]) {
	/* 命令 ”RMD foldername\r\n” */
	sprintf_s(send_buf, "RMD %s\r\n", foldername);
	/*客户端发送文件夹创建命令到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to delete a folder:%s\n", foldername);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}

int core::getFolderNow() {
	memset(filepath, 0, size(filepath));//先将以前的清零
	/* 命令 ”RMD foldername\r\n” */
	sprintf_s(send_buf, "PWD\r\n");
	/*客户端发送文件夹创建命令到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to show the folder where you are right now.\n");
	/* 客户端接收服务器的响应码和信息，正常为 ”257 <path>” */
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
	filepath[j] = 0x00;//作为结束符
	printLog("FTP:%s\n", recData);
	return NULL;
}
int core::getList(const char foldername[], std::set<fileOrDir*>& dire, std::set<fileOrDir*>& file)
{
	/* 命令 ”PASV\r\n” */
	sprintf_s(send_buf, "PASV\r\n");
	/* 客户端告诉服务器用被动模式 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send PASSIVE MODE to FTPServer: %s", send_buf);
	/*客户端接收服务器的响应码和新开的端口号，
	* 正常为 ”227 Entering passive mode (<h1,h2,h3,h4,p1,p2>)” */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);

	//处理返回的新端口号信息（服务器数据端口）
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
	//serDataAddr.sin_addr.S_un.S_addr = inet_addr("182.92.236.19");//服务器ip
	struct in_addr s2;
	inet_pton(AF_INET, serverip, (void*)&s2);
	serDataAddr.sin_addr.s_addr = s2.s_addr;

	//客户端连接到 FTP 服务器的数据端口
	/* 连接服务器新开的数据端口 */
	//connect(data_sock, (struct sockaddr*) & server, sizeof(server));
	/* 连接到服务器端 */
	if (connect(data_sock, (struct sockaddr*) & serDataAddr, sizeof(serDataAddr)) == SOCKET_ERROR)
	{  //连接失败 
		printLog("data socket connect error !");
		closesocket(data_sock);
		return 0;
	}

	/* 命令 ”LIST foldername\r\n” */
	sprintf_s(send_buf, "LIST %s\r\n", foldername);
	/*客户端发送文件夹文件列表查询命令到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to list folder information:%s\n", foldername);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	int rec_len = recv(data_sock, file_buf, max_length, 0);
	//printLog("FTP:%s\n", file_buf);

	//处理返回的列表信息
	point = 0;
	while (point < rec_len) {
		fileOrDir* f = new fileOrDir();
		bool is_file;
		if (file_buf[point] == 'd') is_file = false;
		else is_file = true;
		for (int i = 0; i < 9; i++) {
			f->quanxian[i] = file_buf[++point];//读取权限
		}
		point += 29;//跳过中间无用信息
		int i = 0;
		while (file_buf[point] != ' ') {
			f->size[i++] = file_buf[point++];//读取大小信息
		}
		point++;//跳过空格
		i = 0;
		bool fenhao = false;
		while (1) {
			if ((f->creatTime[i++] = file_buf[point++]) == ':') {//读取日期时间信息
				fenhao = true;
			}
			if (fenhao && (file_buf[point] == ' ')) break;
		}
		point++;
		i = 0;
		while (file_buf[point] != '\r') {
			f->name[i++] = file_buf[point++];//读取名称信息
		}

		if (is_file) file.insert(f);
		else dire.insert(f);
		point += 2;
	}
	return 0;
}
int core::gotoFolder(const char foldername[]) {
	/* 命令 ”CWD foldername\r\n” */
	sprintf_s(send_buf, "CWD %s\r\n", foldername);
	/*客户端发送文件夹创建命令到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a order to go to another folder:%s\n", foldername);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}

int core::renameFile(const char f1[], const char f2[]) {
	/* 命令 ”RNFR filename\r\n” */
	sprintf_s(send_buf, "RNFR %s\r\n", f1);
	/*客户端发送用户名到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a old filename %s to FTPServer\n", f1);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	/* 命令 ”RNTO filename\r\n” */
	sprintf_s(send_buf, "RNTO %s\r\n", f2);
	/*客户端发送用户名到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send a new filename %s to FTPServer\n", f2);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	return 1;
}

void core::printLog(const char text[]) {
	// 基于当前系统的当前日期/时间
	time_t now = time(0);
	// 把 now 转换为字符串形式
	char dt[100];
	ctime_s(dt, 100, &now);
	printf("%s%s\n", dt, text);
}
void core::printLog(const char text0[], const char text1[]) {
	// 基于当前系统的当前日期/时间
	time_t now = time(0);
	// 把 now 转换为字符串形式
	char dt[100];
	ctime_s(dt, 100, &now);
	printf("%s", dt);
	printf(text0, text1);
}

int core::WSAinit() {
	//初始化WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	return 1;
}

int core::contact2server() {
	/* 初始化socket */
	control_sock = socket(AF_INET, SOCK_STREAM, 0);
	data_sock = socket(AF_INET, SOCK_STREAM, 0);
	//hp = gethostbyname(server_name);
	//memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	//server.sin_family = AF_INET;
	//server.sin_port = htons(21);
	serControlAddr.sin_family = AF_INET;
	serControlAddr.sin_port = htons(21);
	//serControlAddr.sin_addr.S_un.S_addr = inet_pton("182.92.236.19");//服务器ip
	struct in_addr s;
	inet_pton(AF_INET, serverip, (void*)&s);
	serControlAddr.sin_addr.s_addr = s.s_addr;

	/* 连接到服务器端 */
	if (connect(control_sock, (struct sockaddr*) & serControlAddr, sizeof(serControlAddr)) == SOCKET_ERROR)
	{  //连接失败 
		printLog("connect error !\n");
		closesocket(control_sock);
		return 0;
	}
	/* 客户端接收服务器端的一些欢迎信息 */
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
		printLog(recData);
	}
	//read(control_sock, read_buf, read_len);

	//清单 2. 客户端发送用户名和密码，登入 FTP 服务器
	/* 命令 ”USER username\r\n” */
	sprintf_s(send_buf, "USER %s\r\n", username);
	/*客户端发送用户名到服务器端 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send user name %s to FTPServer\n", username);
	/* 客户端接收服务器的响应码和信息，正常为 ”331 User name okay, need password.” */
	//read(control_sock, read_buf, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	/* 命令 ”PASS password\r\n” */
	sprintf_s(send_buf, "PASS %s\r\n", password);
	/* 客户端发送密码到服务器端 */
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send user passwd %s to FTPServer\n", password);
	/* 客户端接收服务器的响应码和信息，正常为 ”230 User logged in, proceed.” */
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);

	return 1;
}

int core::deletefile(char filename[]) {
	/* 命令 ”DELE ***\r\n” */
	sprintf_s(send_buf, "DELE %s\r\n", filename);
	/* 客户端告诉服务器删除某个文件 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send DELETE order to FTPServer: %s\n", send_buf);
	//正常应返回250
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);
	if (recData[0] == '2' && recData[1] == '5' && recData[2] == '0') return 1;
	else return 0;
}

int core::downloadOrUploadFile(int mode, char filename[]) {
	//mode：0代表上传，1代表下载
	//filename：文件名称
	//默认上传至服务器当前目录，下载至本机当前目录
	//清单 3. 让服务器进入被动模式，在数据端口监听
	/* 命令 ”PASV\r\n” */
	sprintf_s(send_buf, "PASV\r\n");
	/* 客户端告诉服务器用被动模式 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send PASSIVE MODE to FTPServer: %s", send_buf);
	/*客户端接收服务器的响应码和新开的端口号，
	* 正常为 ”227 Entering passive mode (<h1,h2,h3,h4,p1,p2>)” */
	//read(control_sock, read_buf, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s\n", recData);

	//处理返回的新端口号信息（服务器数据端口）
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
	//serDataAddr.sin_addr.S_un.S_addr = inet_addr("182.92.236.19");//服务器ip
	struct in_addr s2;
	inet_pton(AF_INET, serverip, (void*)&s2);
	serDataAddr.sin_addr.s_addr = s2.s_addr;

	//清单 4. 客户端连接到 FTP 服务器的数据端口并下载/上传文件
	/* 连接服务器新开的数据端口 */
	//connect(data_sock, (struct sockaddr*) & server, sizeof(server));
	/* 连接到服务器端 */
	if (connect(data_sock, (struct sockaddr*) & serDataAddr, sizeof(serDataAddr)) == SOCKET_ERROR)
	{  //连接失败 
		printLog("data socket connect error !");
		closesocket(data_sock);
		return 0;
	}

	if (mode == 1) {
		//下载文件，支持断点续传
		//查询要下载的文件在服务器上的大小
		sprintf_s(send_buf, "SIZE %s\r\n", filename);
		/* 客户端发送命令从服务器端得到下载文件的大小 */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send order to server:%s\n", send_buf);
		/* 客户端接收服务器的响应码和信息，正常为 ”213 size” */
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
		/* 客户端创建文件 */
		//FILE file_handle = open(disk_name, CRFLAGS, RWXALL);
		FILE* fin;
		fopen_s(&fin, filename, "ab+");//可写可读，且以追加形式写
		int size = _filelength(_fileno(fin));

		if (size < file_size_server) {
			/* 命令 ”REST offset\r\n” */
			sprintf_s(send_buf, "REST %ld\r\n", size);
			/* 客户端发送命令指定下载文件的偏移量 */
			//write(control_sock, send_buf, strlen(send_buf));
			send(control_sock, send_buf, strlen(send_buf), 0);
			printLog("You send offset order to server:%s\n", send_buf);
			/* 客户端接收服务器的响应码和信息，
			*正常为 ”350 Restarting at <position>. Send STORE or RETRIEVE to initiate transfer.” */
			//read(control_sock, read_buf, read_len);
			ret = recv(control_sock, recData, 255, 0);
			if (ret > 0) {
				recData[ret] = 0x00;
			}
			printLog("FTP:%s", recData);
		}
		/* 命令 ”RETR filename\r\n” */
		sprintf_s(send_buf, "RETR %s\r\n", filename);
		/* 客户端发送命令从服务器端下载文件 */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send download order to server:%s\n", send_buf);
		/* 客户端接收服务器的响应码和信息，正常为 ”150 Opening data connection.” */
		//read(control_sock, recData, read_len);
		ret = recv(control_sock, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
		}
		printLog("FTP:%s", recData);

		int rec_len;
		fseek(fin, 0, SEEK_END);
		while (rec_len = recv(data_sock, file_buf, max_length, 0)) {
			/* 客户端通过数据连接 从服务器接收文件内容 */
			//offset += rec_len;
			//printLog("FTP:%s", recData);
			/* 客户端写文件 */
			//write(file_handle, read_buf, read_len);
			fwrite(file_buf, rec_len, 1, fin);
		}
		/* 客户端关闭文件 */
		fclose(fin);
	}
	else if (mode == 0) {
		//上传文件，支持断点续传
		/* 命令 ”SIZE filename\r\n” */
		//char filename[] = "a.txt";

		//查询要上传的文件在服务器上的已有大小
		sprintf_s(send_buf, "SIZE %s\r\n", filename);
		/* 客户端发送命令从服务器端得到下载文件的大小 */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send SIZE order to server:%s\n", send_buf);
		/* 客户端接收服务器的响应码和信息，正常为 ”213 size” */
		//read(control_sock, recData, read_len);
		ret = recv(control_sock, recData, 255, 0);
		if (ret > 0) {
			recData[ret] = 0x00;
		}
		printLog("FTP:%s", recData);
		int file_size_server = 0;
		if (recData[0] == '2' && recData[1] == '1' && recData[2] == '3') {
			//服务器已有此文件，开启断点续传(追加APPE)
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
		/* 客户端创建文件 */
		//FILE file_handle = open(disk_name, CRFLAGS, RWXALL);
		FILE* fout;
		fopen_s(&fout, filename, "rb");
		int size = _filelength(_fileno(fout));//本地文件大小

		/* 命令 ”STOR/APPE filename\r\n” */

		if (file_size_server > 0) {
			sprintf_s(send_buf, "APPE %s\r\n", filename);//断点续传
		}
		else {
			sprintf_s(send_buf, "STOR %s\r\n", filename);
		}

		/* 客户端发送命令从服务器端上传文件 */
		//write(control_sock, send_buf, strlen(send_buf));
		send(control_sock, send_buf, strlen(send_buf), 0);
		printLog("You send upload order to server:%s\n", send_buf);
		/* 客户端接收服务器的响应码和信息，正常为 ”150 OK to send data.” */
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
		/* 客户端关闭文件 */
		fclose(fout);
	}
	else {
		printLog("wrong mode");
		return 0;
	}

	//清单 5. 客户端关闭数据连接,退出 FTP 服务器并关闭控制连接
	/* 客户端关闭数据连接 */
	closesocket(data_sock);
	/* 客户端接收服务器的响应码和信息，正常为 ”226 Transfer complete.” */
	//read(control_sock, recData, read_len);
	ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s", recData);
	return 1;
}
int core::closeConnect() {
	/* 命令 ”QUIT\r\n” */
	sprintf_s(send_buf, "QUIT\r\n");
	/* 客户端将断开与服务器端的连接 */
	//write(control_sock, send_buf, strlen(send_buf));
	send(control_sock, send_buf, strlen(send_buf), 0);
	printLog("You send QUIT order to server:%s\n", send_buf);
	/* 客户端接收服务器的响应码，正常为 ”200 Closes connection.” */
	//read(control_sock, recData, read_len);
	int ret = recv(control_sock, recData, 255, 0);
	if (ret > 0) {
		recData[ret] = 0x00;
	}
	printLog("FTP:%s", recData);
	printLog("=============================================");
	/* 客户端关闭控制连接 */
	closesocket(control_sock);
	WSACleanup();
	return 1;
}