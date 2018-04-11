#pragma once
#include "afxwin.h"
#include "mscomm1.h"


// CSetting �Ի���

class CSetting : public CDialog
{
	DECLARE_DYNAMIC(CSetting)

public:
	CSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetting();

	virtual void OnFinalRelease();

// �Ի�������
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();			//��ʼ���Ի���

	//DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	/*afx_msg void OnOK();*/
	double m_dipScale;
	// ƥ�侫��,double��
	double m_pricision;
	// �ӵ������Сֵ
	int m_areaMin;
	// �ӵ�������ֵ,int��
	int m_areaMax;
	// ѡ��COM��
	CComboBox m_comSelect;
	// ������ѡ��
	CComboBox m_botSelect;
	int m_dpoffset;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedTstcom();
	//���Կؼ�����
	CMscomm1 m_scommtst;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedSavedataBtn();
	int m_cotoffset;
};
