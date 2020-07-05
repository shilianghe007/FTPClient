// CMainDlg.cpp: 实现文件
//

#include "pch.h"
#include "FTPClient.h"
#include "CMainDlg.h"
#include "afxdialogex.h"
#include "CUploadFile.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(core* client, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	this->client = client;
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, filelist);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序
int CMainDlg::updatelist() {
	//先清空
	filelist.DeleteAllItems();
	for (int i = 0; i < 5; i++) {
		filelist.DeleteColumn(i);
	}
		
	std::set<fileOrDir*> d, f;
	client->getList("./", d, f);

	CString titles[5] = { TEXT("名称"),TEXT("类型"),TEXT("权限"),TEXT("大小(字节)"),TEXT("创建日期") };
	for (int i = 0; i < 5; i++) {
		filelist.InsertColumn(i, titles[i], LVCFMT_LEFT, 100);
	}

	//添加表项(先文件夹后文件)
	set<fileOrDir*>::iterator iter = d.begin();
	int i = 0;
	while (iter != d.end())
	{
		int j = 0;
		filelist.InsertItem(i, (CString)(*iter)->name);

		filelist.SetItemText(i, ++j, TEXT("文件夹"));
		filelist.SetItemText(i, ++j, (CString)(*iter)->quanxian);
		filelist.SetItemText(i, ++j, (CString)(*iter)->size);
		filelist.SetItemText(i++, ++j, (CString)(*iter)->creatTime);
		++iter;
	}
	iter = f.begin();
	i = 0;
	while (iter != f.end())
	{
		int j = 0;
		filelist.InsertItem(i, (CString)(*iter)->name);

		filelist.SetItemText(i, ++j, TEXT("文件夹"));
		filelist.SetItemText(i, ++j, (CString)(*iter)->quanxian);
		filelist.SetItemText(i, ++j, (CString)(*iter)->size);
		filelist.SetItemText(i++, ++j, (CString)(*iter)->creatTime);
		++iter;
	}
	return 0;
}

BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 添加列表控件的显示
	filelist.SetExtendedStyle(filelist.GetExtendedStyle()|LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES);
	updatelist();
	// 


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMainDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ShowWindow(SW_HIDE);
	CUploadFile up(this->client);
	up.DoModal();
	this->ShowWindow(SW_SHOW);
}
