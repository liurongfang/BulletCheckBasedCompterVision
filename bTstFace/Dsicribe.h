#pragma once


// Dsicribe �Ի���

class Dsicribe : public CDialog
{
	DECLARE_DYNAMIC(Dsicribe)

public:
	Dsicribe(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Dsicribe();

	virtual void OnFinalRelease();

// �Ի�������
	enum { IDD = IDD_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
