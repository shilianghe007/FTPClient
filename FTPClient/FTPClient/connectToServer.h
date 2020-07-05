#pragma once
#include "core.h"
#include"CMainDlg.h"
#include <afxdisp.h>

// connectToServer 对话框

class connectToServer : public CDialogEx
{
	DECLARE_DYNAMIC(connectToServer)

public:
	connectToServer(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~connectToServer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CEdit s_ip;
	CEdit s_name;
	CEdit s_ps;
	core* client = nullptr;
	CMainDlg* mdlg;
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
