// Dsicribe.cpp : 实现文件
//

#include "stdafx.h"
#include "bTstFace.h"
#include "Dsicribe.h"


// Dsicribe 对话框

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
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

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

// 注意: 我们添加 IID_IDsicribe 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {670CC45A-5F07-4187-BA40-3BD4A0EE1E8C}
static const IID IID_IDsicribe =
{ 0x670CC45A, 0x5F07, 0x4187, { 0xBA, 0x40, 0x3B, 0xD4, 0xA0, 0xEE, 0x1E, 0x8C } };

BEGIN_INTERFACE_MAP(Dsicribe, CDialog)
	INTERFACE_PART(Dsicribe, IID_IDsicribe, Dispatch)
END_INTERFACE_MAP()


// Dsicribe 消息处理程序
BOOL Dsicribe::OnInitDialog()
{
	//((CDialog*)GetDlgItem(IDD_HELP))->SetWindowText(L"说明");
	SetWindowText(_T("项目书..."));
	((CButton*)GetDlgItem(IDOK))->SetWindowText(_T("确定"));
	((CStatic*)GetDlgItem(IDC_TIP))->SetWindowText(_T("关于本项目的说明"));			//确切的来说，这只是一个实现了简单功能的机器视觉项目。

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

	if (nCtlColor == CTLCOLOR_DLG )			//改变背景色
	{
		//pDC->SetBkColor(RGB(255,0,0));
		HBRUSH B = CreateSolidBrush(RGB(153,204,153)); //创建画刷RGB(174,221,129)
		return (HBRUSH) B;
	}

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);		//透明背景
		HBRUSH B = CreateSolidBrush(RGB(153,204,153));	 //创建画刷RGB(174,221,129)
		return (HBRUSH) B;
	}

	return hbr;
}
