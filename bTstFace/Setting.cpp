// Setting.cpp : 实现文件
//

#include "stdafx.h"
#include "bTstFace.h"
#include "Setting.h"

typedef struct colorRGB						//用来设置对话框的颜色
{
	int r;
	int g;
	int b;
}MyColor;

extern int innerBoundary;			//选择区域宽度
extern int outerBoundary;			//外边界宽度
extern int AreaMin;		//面积最小值
extern int AreaMax;		//面积最大值
extern double dipScale;			//缩放比
extern double pricision;		//匹配精度
extern int com;
extern CString botlv;
extern MyColor DlgColor;
extern MyColor TextColor;
extern MyColor StaticColor;
// CSetting 对话框

IMPLEMENT_DYNAMIC(CSetting, CDialog)

CSetting::CSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CSetting::IDD, pParent)
	, m_dipScale(0)
	, m_pricision(0)
	, m_areaMin(0)
	, m_areaMax(0)
	, m_dpoffset(0)
	, m_cotoffset(0)
{

	EnableAutomation();

}

CSetting::~CSetting()
{
}

void CSetting::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CDialog::OnFinalRelease();
}

void CSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIPSCALE, m_dipScale);
	DDX_Text(pDX, IDC_EDIT_PRICISION, m_pricision);
	DDX_Text(pDX, IDC_EDIT_AREAMIN, m_areaMin);
	DDX_Text(pDX, IDC_EDIT_AREAMAX, m_areaMax);
	DDX_Text(pDX, IDC_EDIT_SELAREA, m_dpoffset);
	DDX_Control(pDX, IDC_COMSEL, m_comSelect);
	DDX_Control(pDX, IDC_BOTSEL, m_botSelect);
	DDX_Text(pDX, IDC_EDIT_COUNTAREA, m_cotoffset);
}


//BEGIN_MESSAGE_MAP(CSetting, CDialog)
//	ON_BN_CLICKED(IDOK, &CSetting::OnOk)
//END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSetting, CDialog)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_ISetting 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {326081F5-2BFD-42A8-820C-E76B7C9E7F05}
static const IID IID_ISetting =
{ 0x326081F5, 0x2BFD, 0x42A8, { 0x82, 0xC, 0xE7, 0x6B, 0x7C, 0x9E, 0x7F, 0x5 } };

BEGIN_INTERFACE_MAP(CSetting, CDialog)
	INTERFACE_PART(CSetting, IID_ISetting, Dispatch)
END_INTERFACE_MAP()


// CSetting 消息处理程序
BOOL CSetting::OnInitDialog()
{
	SetWindowText(_T("设置基本参数"));
	((CStatic*)GetDlgItem(IDC_TEXT1))->SetWindowText(_T("比例尺(像素/实际距离CM,double型)："));
	((CStatic*)GetDlgItem(IDC_TEXT2))->SetWindowText(_T("子弹匹配精度(像素值,double型)："));
	((CStatic*)GetDlgItem(IDC_TEXT3))->SetWindowText(_T("子弹面积范围(像素值,doubel型)："));
	((CStatic*)GetDlgItem(IDC_TEXT4))->SetWindowText(_T("<   X   <"));
	((CStatic*)GetDlgItem(IDC_TEXT7))->SetWindowText(_T("子弹选择区域宽度(内边界)："));
	((CStatic*)GetDlgItem(IDC_TEXT8))->SetWindowText(_T("计数限制区域宽度(外边界)："));
	((CStatic*)GetDlgItem(IDC_TEXT5))->SetWindowText(_T("COM口："));
	((CStatic*)GetDlgItem(IDC_TEXT6))->SetWindowText(_T("波特率："));
	((CButton*)GetDlgItem(IDC_TSTCOM))->SetWindowText(_T("测试串口"));
	((CButton*)GetDlgItem(IDOK))->SetWindowText(_T("确定"));
	((CButton*)GetDlgItem(IDCANCEL))->SetWindowText(_T("取消"));

	//m_comSelect.InsertString(0,_T("COM1"));		//注意，L之后不加括号
	//m_comSelect.InsertString(1,_T("COM2"));
	//m_comSelect.InsertString(2,_T("COM3"));
	//m_comSelect.InsertString(3,_T("COM4"));
	//m_comSelect.InsertString(4,_T("COM5"));		//问题，为什么这段不能用？
	//m_comSelect.SetCurSel(0);

	//m_botSelect.InsertString(0,_T("1200"));
	//m_botSelect.InsertString(1,_T("4800"));
	//m_botSelect.InsertString(2,_T("9600"));
	//m_botSelect.InsertString(3,_T("14400"));
	//m_botSelect.InsertString(4,_T("19200"));
	//m_botSelect.SetCurSel(2);

	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(0,_T("COM1"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(1,_T("COM2"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(2,_T("COM3"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(3,_T("COM4"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(4,_T("COM5"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(5,_T("COM6"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(6,_T("COM7"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(7,_T("COM8"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(8,_T("COM9"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(9,_T("COM10"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(10,_T("COM11"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(11,_T("COM12"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(12,_T("COM13"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(13,_T("COM14"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(14,_T("COM15"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(15,_T("COM16"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(16,_T("COM17"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(17,_T("COM18"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(18,_T("COM19"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->InsertString(19,_T("COM20"));
	((CComboBox*)GetDlgItem(IDC_COMSEL))->SetCurSel(com-1);

	((CComboBox*)GetDlgItem(IDC_BOTSEL))->InsertString(0,_T("1200"));
	((CComboBox*)GetDlgItem(IDC_BOTSEL))->InsertString(1,_T("4800"));
	((CComboBox*)GetDlgItem(IDC_BOTSEL))->InsertString(2,_T("9600"));
	((CComboBox*)GetDlgItem(IDC_BOTSEL))->InsertString(3,_T("14400"));
	((CComboBox*)GetDlgItem(IDC_BOTSEL))->InsertString(4,_T("19200"));
	((CComboBox*)GetDlgItem(IDC_BOTSEL))->SetCurSel(2);
	

	//数值更新部分
	m_areaMin = AreaMin;
	m_areaMax = AreaMax;
	m_dpoffset = innerBoundary;
	m_dipScale = dipScale;
	m_pricision = pricision;
	m_cotoffset = outerBoundary;

	UpdateData(FALSE);

	return TRUE;
}




//void CSetting::OnOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	UpdateData(TRUE);
//	AfxMessageBox(_T("ACB"),MB_OK,0);
//	OnOK();
//}
BEGIN_MESSAGE_MAP(CSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_TSTCOM, &CSetting::OnBnClickedTstcom)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SAVEDATA_BTN, &CSetting::OnBnClickedSavedataBtn)
END_MESSAGE_MAP()

//点击确定按钮
void CSetting::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);			//从控件获取变量
	//AfxMessageBox(_T("ACB"),MB_OK,0);
	AreaMin = m_areaMin;
	AreaMax = m_areaMax;
	innerBoundary = m_dpoffset;
	dipScale = m_dipScale;
	pricision = m_pricision;
	//com = ((CComboBox*)GetDlgItem(IDC_COMSEL)).GetCurSel();
	com = m_comSelect.GetCurSel() + 1;
	GetDlgItem(IDC_BOTSEL)->GetWindowText(botlv);

	OnOK();
}

//测试串口按钮
void CSetting::OnBnClickedTstcom()
{
	// TODO:在此添加控件通知处理程序代码
	//m_scommtst.put__CommPort(com);
	//if (m_scommtst.get_PortOpen())		//如果串口被占用，提示
	//{
	//	AfxMessageBox(_T("当前串口被占用"),MB_OK,0);
	//}
	//else		//串口正常
	//{
	//	m_scommtst.put_PortOpen(TRUE);		//打开串口
	//	if (m_scommtst.get_PortOpen())
	//	{
	//		AfxMessageBox(_T("当前串口可以使用"),MB_OK,0);
	//		m_scommtst.put_PortOpen(FALSE);
	//	}
	//}

}

HBRUSH CSetting::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG )			//改变背景色
	{
		//pDC->SetBkColor(RGB(255,0,0));
		HBRUSH B = CreateSolidBrush(RGB(DlgColor.r,DlgColor.g,DlgColor.b)); //创建画刷RGB(174,221,129)
		return (HBRUSH) B;
	}

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);		//透明背景
		HBRUSH B = CreateSolidBrush(RGB(DlgColor.r,DlgColor.g,DlgColor.b));	 //创建画刷RGB(174,221,129)
		return (HBRUSH) B;
	}

	return hbr;
}

void CSetting::OnBnClickedSavedataBtn()
{
	CString KeyValue;
	BOOL writeOk = FALSE;
	UpdateData(TRUE);
	AreaMin = m_areaMin;
	AreaMax = m_areaMax;
	innerBoundary = m_dpoffset;
	outerBoundary = m_cotoffset;
	dipScale = m_dipScale;
	pricision = m_pricision;
	KeyValue.Format(_T("%d"),AreaMin);
	writeOk = ::WritePrivateProfileString("ProgramSetting","AreaMin",KeyValue,".\\setting.ini");		//写入面积最小值
	KeyValue.Format(_T("%d"),AreaMax);
	writeOk = writeOk && ::WritePrivateProfileString("ProgramSetting","AreaMax",KeyValue,".\\setting.ini");		//写入面积最大值
	KeyValue.Format(_T("%d"),(int)dipScale);
	writeOk = writeOk &&::WritePrivateProfileString("ProgramSetting","dipScale",KeyValue,".\\setting.ini");		//写入标定比
	KeyValue.Format(_T("%d"),(int)pricision);
	writeOk = writeOk &&::WritePrivateProfileString("ProgramSetting","pricision",KeyValue,".\\setting.ini");	//写入匹配精度
	KeyValue.Format(_T("%d"),innerBoundary);
	//AfxMessageBox(_T("dpOffset")+KeyValue,MB_OK,0);
	writeOk = writeOk &&::WritePrivateProfileString("ProgramSetting","innerBoundary",KeyValue,".\\setting.ini");		//写入允许偏移(内边界)
	KeyValue.Format(_T("%d"),outerBoundary);
	writeOk = writeOk &&::WritePrivateProfileString("ProgramSetting","outerBoundary",KeyValue,".\\setting.ini");	//写入匹配精度(外边界)
	if (writeOk)
	{
		AfxMessageBox(_T("除模板尺寸与RGB值外的全部数据写入完毕！"),MB_OK,0);
	}
	else
	{
		if(IDRETRY == AfxMessageBox(_T("请重试！"),MB_RETRYCANCEL,0))
			OnBnClickedSavedataBtn();
	}
	CSetting::OnBnClickedOk();
}
