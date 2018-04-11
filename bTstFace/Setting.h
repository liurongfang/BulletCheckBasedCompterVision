#pragma once
#include "afxwin.h"
#include "mscomm1.h"


// CSetting 对话框

class CSetting : public CDialog
{
	DECLARE_DYNAMIC(CSetting)

public:
	CSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetting();

	virtual void OnFinalRelease();

// 对话框数据
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();			//初始化对话框

	//DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	/*afx_msg void OnOK();*/
	double m_dipScale;
	// 匹配精度,double型
	double m_pricision;
	// 子弹面积最小值
	int m_areaMin;
	// 子弹面积最大值,int型
	int m_areaMax;
	// 选择COM口
	CComboBox m_comSelect;
	// 波特率选择
	CComboBox m_botSelect;
	int m_dpoffset;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedTstcom();
	//测试控件变量
	CMscomm1 m_scommtst;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedSavedataBtn();
	int m_cotoffset;
};
