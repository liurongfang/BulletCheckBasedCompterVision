#pragma once
#include "opencv/cxcore.h"
#include "afxcmn.h"

// CAdjust 对话框

class CAdjust : public CDialog
{
	DECLARE_DYNAMIC(CAdjust)

public:
	CAdjust(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdjust();


	virtual void OnFinalRelease();

// 对话框数据
	enum { IDD = IDD_CALIBRAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();			//初始化函数

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg void OnBnClickedAdjustBtn();
	afx_msg void DrawIplImgToDC(IplImage* img,int ID);
	// 标定得到的缩放比
	double distance;
	CString m_showMsg;
	CString m_slect_value;
	CSliderCtrl m_select_slider;
	afx_msg void OnNMReleasedcaptureSelectSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
