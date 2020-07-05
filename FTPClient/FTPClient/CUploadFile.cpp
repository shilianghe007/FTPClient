// CUploadFile.cpp: 实现文件
//

#include "pch.h"
#include "FTPClient.h"
#include "CUploadFile.h"
#include "afxdialogex.h"


// CUploadFile 对话框

IMPLEMENT_DYNAMIC(CUploadFile, CDialogEx)

CUploadFile::CUploadFile(core* client, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	this->client = client;
}

CUploadFile::~CUploadFile()
{
}

void CUploadFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, filepath);
}


BEGIN_MESSAGE_MAP(CUploadFile, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CUploadFile::OnBnClickedButton1)
END_MESSAGE_MAP()


// CUploadFile 消息处理程序


void CUploadFile::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(TEXT("上传文件成功."));
	CString path;
	filepath.GetWindowTextW(path);
	//调用
	USES_CONVERSION;
	char* pPath = T2A(path);
	char pathname[] = "BIOS.pdf";
	client->downloadOrUploadFile(0,pathname);
}
