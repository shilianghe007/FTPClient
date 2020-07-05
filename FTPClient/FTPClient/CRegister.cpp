// CRegister.cpp: 实现文件
//

#include "pch.h"
#include "FTPClient.h"
#include "CRegister.h"
#include "afxdialogex.h"
#include "TxtFileTool.h"
#include "TxtFileTool.h"



// CRegister 对话框

IMPLEMENT_DYNAMIC(CRegister, CDialogEx)

CRegister::CRegister(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTER, pParent)
{

}

CRegister::~CRegister()
{
}

void CRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, r_name);
	DDX_Control(pDX, IDC_EDIT2, r_ps1);
	DDX_Control(pDX, IDC_EDIT3, r_ps2);
}


BEGIN_MESSAGE_MAP(CRegister, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CRegister::OnBnClickedButton2)
END_MESSAGE_MAP()


// CRegister 消息处理程序


void CRegister::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString name, ps1, ps2;
	r_name.GetWindowTextW(name);
	r_ps1.GetWindowTextW(ps1);
	r_ps2.GetWindowTextW(ps2);
	if (ps1 == ps2) {
		MessageBox(TEXT("ok"));
		//存储账号密码
		UsrInfo test;
		//声明标识
		USES_CONVERSION;
		//函数T2A和W2A均支持ATL和MFC中的字符
		char* pName = T2A(name);
		char* ps = T2A(ps1);
		strcpy_s(test.UsrName, pName);
		strcpy_s(test.Psword, ps);
		TxtFileTool tool;
		switch (tool.regist(&test)) {
		case 1:
			MessageBox(TEXT("成功注册！"));
			break;
		case 0:
			MessageBox(TEXT("该用户名已经被注册过！"));
			break;
		default:
			break;
		}
	}
	else {
		MessageBox(TEXT("你两次输入的密码不一致！"));
	}
}
