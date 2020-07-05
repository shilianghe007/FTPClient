// connectToServer.cpp: 实现文件
//

#include "pch.h"
#include "FTPClient.h"
#include "connectToServer.h"
#include "afxdialogex.h"


// connectToServer 对话框

IMPLEMENT_DYNAMIC(connectToServer, CDialogEx)

connectToServer::connectToServer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

connectToServer::~connectToServer()
{
}

void connectToServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, s_ip);
	DDX_Control(pDX, IDC_EDIT2, s_name);
	DDX_Control(pDX, IDC_EDIT3, s_ps);
}


BEGIN_MESSAGE_MAP(connectToServer, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &connectToServer::OnBnClickedButton1)
END_MESSAGE_MAP()


// connectToServer 消息处理程序


void connectToServer::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString name, ip, ps;
	s_name.GetWindowTextW(name);
	s_ip.GetWindowTextW(ip);
	s_ps.GetWindowTextW(ps);
	USES_CONVERSION;
	const char* pName = T2A(name);
	const char* pPs = T2A(ps);
	const char* pIp = T2A(ip);
	client = new core(pName, pPs, pIp, true);

	mdlg = new CMainDlg(client);
	this->ShowWindow(SW_HIDE);
	mdlg->DoModal();
	this->ShowWindow(SW_SHOW);
}


BOOL connectToServer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	s_ip.SetWindowTextW(TEXT("182.92.236.19"));
	s_name.SetWindowTextW(TEXT("cliff"));
	s_ps.SetWindowTextW(TEXT("160035"));
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void connectToServer::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}
