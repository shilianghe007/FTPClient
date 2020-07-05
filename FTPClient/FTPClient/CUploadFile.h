#pragma once
#include "core.h"


// CUploadFile 对话框

class CUploadFile : public CDialogEx
{
	DECLARE_DYNAMIC(CUploadFile)

public:
	CUploadFile(core* client, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUploadFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CEdit filepath;
	core* client;
public:
	afx_msg void OnBnClickedButton1();
};
