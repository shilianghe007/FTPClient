#pragma once


// CRegister 对话框

class CRegister : public CDialogEx
{
	DECLARE_DYNAMIC(CRegister)

public:
	CRegister(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRegister();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CEdit r_name;
	CEdit r_ps1;
	CEdit r_ps2;
public:
	afx_msg void OnBnClickedButton2();
};
