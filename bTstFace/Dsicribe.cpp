// Dsicribe.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "bTstFace.h"
#include "Dsicribe.h"


// Dsicribe �Ի���

IMPLEMENT_DYNAMIC(Dsicribe, CDialog)

Dsicribe::Dsicribe(CWnd* pParent /*=NULL*/)
	: CDialog(Dsicribe::IDD, pParent)
{

	EnableAutomation();

}

Dsicribe::~Dsicribe()
{
}

void Dsicribe::OnFinalRelease()
{
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease�����ཫ�Զ�
	// ɾ���ö����ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CDialog::OnFinalRelease();
}

void Dsicribe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Dsicribe, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Dsicribe, CDialog)
END_DISPATCH_MAP()

// ע��: ������� IID_IDsicribe ֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡��� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {670CC45A-5F07-4187-BA40-3BD4A0EE1E8C}
static const IID IID_IDsicribe =
{ 0x670CC45A, 0x5F07, 0x4187, { 0xBA, 0x40, 0x3B, 0xD4, 0xA0, 0xEE, 0x1E, 0x8C } };

BEGIN_INTERFACE_MAP(Dsicribe, CDialog)
	INTERFACE_PART(Dsicribe, IID_IDsicribe, Dispatch)
END_INTERFACE_MAP()


// Dsicribe ��Ϣ�������
BOOL Dsicribe::OnInitDialog()
{
	//((CDialog*)GetDlgItem(IDD_HELP))->SetWindowText(L"˵��");
	SetWindowText(_T("��Ŀ��..."));
	((CButton*)GetDlgItem(IDOK))->SetWindowText(_T("ȷ��"));
	((CStatic*)GetDlgItem(IDC_TIP))->SetWindowText(_T("���ڱ���Ŀ��˵��"));			//ȷ�е���˵����ֻ��һ��ʵ���˼򵥹��ܵĻ����Ӿ���Ŀ��

	CFile   file1(_T("detail.txt"),CFile::modeRead); 
	char   *pBuf; 
	//VARIANT pBuf;
	int   iLen=file1.GetLength(); 
	pBuf   =new   char[iLen+1]; 
	file1.Read(pBuf,iLen); 
	pBuf[iLen]=0; 
	file1.Close(); 
	//SetDlgItemText(IDC_DETAIL,pBuf);

	((CStatic*)GetDlgItem(IDC_DETAIL))->SetWindowText(pBuf);

	return TRUE;
}
HBRUSH Dsicribe::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG )			//�ı䱳��ɫ
	{
		//pDC->SetBkColor(RGB(255,0,0));
		HBRUSH B = CreateSolidBrush(RGB(153,204,153)); //������ˢRGB(174,221,129)
		return (HBRUSH) B;
	}

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);		//͸������
		HBRUSH B = CreateSolidBrush(RGB(153,204,153));	 //������ˢRGB(174,221,129)
		return (HBRUSH) B;
	}

	return hbr;
}
