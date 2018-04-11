#pragma once


// Dsicribe 对话框

class Dsicribe : public CDialog
{
	DECLARE_DYNAMIC(Dsicribe)

public:
	Dsicribe(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Dsicribe();

	virtual void OnFinalRelease();

// 对话框数据
	enum { IDD = IDD_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
