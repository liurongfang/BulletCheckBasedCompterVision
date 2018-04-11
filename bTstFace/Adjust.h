#pragma once
#include "opencv/cxcore.h"
#include "afxcmn.h"

// CAdjust �Ի���

class CAdjust : public CDialog
{
	DECLARE_DYNAMIC(CAdjust)

public:
	CAdjust(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAdjust();


	virtual void OnFinalRelease();

// �Ի�������
	enum { IDD = IDD_CALIBRAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();			//��ʼ������

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg void OnBnClickedAdjustBtn();
	afx_msg void DrawIplImgToDC(IplImage* img,int ID);
	// �궨�õ������ű�
	double distance;
	CString m_showMsg;
	CString m_slect_value;
	CSliderCtrl m_select_slider;
	afx_msg void OnNMReleasedcaptureSelectSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
