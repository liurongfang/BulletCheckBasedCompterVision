// bTstFaceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "atlimage.h"
#include "bTstFace.h"
#include "bTstFaceDlg.h"
#include "CvvImage.h"
//#include "cv.h"
//#include "highgui.h"
#include "easysize.h"
#include "afxwin.h"
#include "Dsicribe.h"
#include "Adjust.h"
#include "Setting.h"

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#define OBJECT_SUM (10);			//定义要检测的物体数目

/*#ifdef _DEBUG
#define new DEBUG_NEW
#endif*/

#include "memoryleak.h"
//程序控制变量
int   com = 1;						//默认com1口
CString botlv;

typedef struct colorRGB						//用来设置对话框的颜色
{
	int r;
	int g;
	int b;
}MyColor;
MyColor DlgColor, TextColor, StaticColor;	//分别为对话框颜色，文字颜色，静态控件背景色


//物体识别尺寸变量
int AreaMin = 20000;			//子弹面积阈值，大于AreaMin并小于AreaMax认为是子弹
int AreaMax = 60000;
int innerBoundary = 40;					//子弹选择区域宽度(内边界)
int outerBoundary = 80;				//计数选择区域的大小(外边界)
double dipScale = 143;			//像素比，像素/实际距离
double pricision = 20;			//匹配精度，即在尺寸匹配的时候允许的尺寸误差
double Offset = 9.0;

//int type_sum = 0;					//子弹的总数
//int type_count[10] = {0};					//最多检测十种物体，暂时A,B,C,D型子弹计数

//double object_height = 0;		//子弹实际的长和宽(像素单位)
//double object_width = 0;

//double heightSize[10] = { 363.6, 315.5, 285.8, 250.2};			//建立尺寸常量表
//double widthSize[10] = { 145.0, 117.9, 145.0, 122.5};
//int center_x = 0;
//int center_y = 0;

//openCV部分需要的变量
CvvImage m_CvvImg;
//CvCapture*	m_video = NULL;			//视频Capture
IplImage*	srcImg = NULL;				//存放视频帧，后续处理也在这个上面
//IplImage*	preBkImg = NULL;			//cvLoadImage("..\\preBk.jpg",1);			//背景图像
//IplImage*	contourImg = NULL;			//用来画最大轮廓的图像
//IplImage*	backupImg = NULL;			//用了保存仅进行过亮度处理的图片，以用于斑点检测
//IplImage*	willImg = NULL;			//得到的子弹待处理图片
//CvBox2D rect;						//用来存储“子弹”的BOX



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();

	CStatic m_thanks_text;
	CString m_thanks_value;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_thanks_value(_T(""))
{

}

BOOL CAboutDlg::OnInitDialog()
{
	//m_thanks_text.SetWindowText(L"指导老师：	王磊\n项目成员：刘荣芳(光信息1202)\n黄旭薇(光信息1202)\n胡志杰(电子信息1201)\n项目名称：	塞伯睿机器人技术（长沙）有限公司");
	m_thanks_value.Format(_T(" 项目名称：	塞伯睿机器人技术（长沙）有限公司 分拣机器人——机器视觉系统\n\n \
		非淡泊无以明志，非宁静无以致远！\n\n \
		"));
	UpdateData(FALSE);

	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_THANKS, m_thanks_text);
	DDX_Text(pDX, IDC_THANKS, m_thanks_value);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedOk()			//关于窗体的确定按钮
{
	this->ShowWindow(SW_HIDE);
	OnOK();

}

// CbTstFaceDlg 对话框

CbTstFaceDlg::CbTstFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CbTstFaceDlg::IDD, pParent)
	, m_light_value(_T("100"))
	, m_thres_value(_T("0"))
	, m_type_text(_T(""))
	, m_type_sum(_T("0"))
	, m_bullet_size(_T(""))
	, m_typeA_value(_T("0"))
	, m_typeB_value(_T("0"))
	, m_typeC_value(_T("0"))
	, m_typeD_value(_T("0"))
	, m_constrat_value(_T("20"))
	, m_sudu_value(_T("0"))
	, m_outText(_T(""))
{
	//dlg = new CAboutDlg;		//关于窗体
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// initialize
	statc = false;					//点击连接摄像头的开关变量
	flag = 0;						//判断图像是否更新
	canPlus = 1;
	enterTimercount = 0;			//进入定时器的次数
	m_video = NULL;			//视频Capture
	preBkImg = NULL;			//cvLoadImage("..\\preBk.jpg",1);			//背景图像
	contourImg = NULL;			//用来画最大轮廓的图像
	backupImg = NULL;			//用了保存仅进行过亮度处理的图片，以用于斑点检测
	willImg = NULL;			//得到的子弹待处理图片

	type_sum = 0;					//子弹的总数
	//memset(type_count, 0, sizeof(int)*10);					//最多检测十种物体，暂时A,B,C,D型子弹计数
	for (int i = 0; i < DETECT_TYPE_NUM; i++)
	{
		type_count[i] = 0;
	}
	send_data_flag = 0;
	send_data_finish = 0;

	//object_height = 0;		//子弹实际的长和宽(像素单位)
	//object_width = 0;

	heightSize[0] = 25;
	heightSize[1] = 25;
	heightSize[2] = 25;
	heightSize[3] = 25;
	heightSize[4] = 25;
	//heightSize[10] = { 363.6, 315.5, 285.8, 250.2};			//建立尺寸常量表
	widthSize[0] = 25;
	widthSize[1] = 25;
	widthSize[2] = 25;
	widthSize[3] = 25;
	widthSize[4] = 25;
	//widthSize[10] = { 145.0, 117.9, 145.0, 122.5};
	for (int i = 0; i < MAX_NUM; i++)
	{
		object_height[i] = 0;
		object_width[i] = 0;
		center_x[i] = 0;
		center_y[i] = 0;
	}

	for (int i = 0; i < SEND_DATA_LENGTH; i++)
	{
		senddata[i] = 0;
	}

}

CbTstFaceDlg::~CbTstFaceDlg()
{
	//	DeleteObject(DlgColorBRUSH);
	if (contourImg != NULL)
	{
		cvReleaseImage(&contourImg);
		contourImg = NULL;
	}
	if (backupImg != NULL)
	{
		cvReleaseImage(&backupImg);
		backupImg = NULL;
	}
	if (willImg != NULL)
	{
		cvReleaseImage(&willImg);
		willImg = NULL;
	}
	if (m_video != NULL)
	{
		cvReleaseCapture(&m_video);
		m_video = NULL;
	}
}

void CbTstFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIGHT_SLIDER, m_slider_light);
	DDX_Text(pDX, IDC_LIGHTVALUE, m_light_value);
	DDX_Text(pDX, IDC_THRESVALUE, m_thres_value);
	DDX_Control(pDX, IDC_THRE_SLIDER, m_slider_thres);
	DDX_Text(pDX, IDC_NOW_TYPE, m_type_text);
	DDX_Text(pDX, IDC_COUNT_SUM, m_type_sum);
	DDX_Text(pDX, IDC_BULLETSIZE, m_bullet_size);
	DDX_Control(pDX, IDC_BULLETSIZE, m_bulletSize_ctl);
	DDX_Control(pDX, IDC_NOW_TYPE, m_bulletType_ctl);
	DDX_Text(pDX, IDC_COUNT_A, m_typeA_value);
	DDX_Text(pDX, IDC_COUNT_B, m_typeB_value);
	DDX_Text(pDX, IDC_COUNT_C, m_typeC_value);
	DDX_Text(pDX, IDC_COUNT_D, m_typeD_value);
	DDX_Control(pDX, IDC_CONTRAST_SLIDER, m_contrast_ctl);
	DDX_Text(pDX, IDC_THRESVALUE2, m_constrat_value);
	DDX_Control(pDX, IDC_CHECK1, m_glabolThres_check);
	DDX_Control(pDX, IDC_CHECK2, m_lightAuto_ctl);
	DDX_Control(pDX, IDC_CHECK3, m_openMotor_chk);
	DDX_Control(pDX, IDC_MSCOMM1, m_scomm);
	DDX_Control(pDX, IDC_SUDU_SLIDER, m_sudu_slider);
	DDX_Text(pDX, IDC_SUDU_VALUE, m_sudu_value);
	DDX_Text(pDX, IDC_EDIT1, m_outText);
	DDX_Control(pDX, IDC_EDIT1, Edit_shuchu);
}

BEGIN_MESSAGE_MAP(CbTstFaceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_about, &CbTstFaceDlg::OnaboutClick)
	ON_COMMAND(ID_LINKCAM, &CbTstFaceDlg::OnLinkcam)
	ON_UPDATE_COMMAND_UI(ID_LINKCAM, &CbTstFaceDlg::OnUpdateLinkcam)
	ON_WM_INITMENUPOPUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EXIT, &CbTstFaceDlg::OnMenuExit)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_LIGHT_SLIDER, &CbTstFaceDlg::OnNMReleasedcaptureLightSlider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_THRE_SLIDER, &CbTstFaceDlg::OnNMReleasedcaptureThreSlider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CONTRAST_SLIDER, &CbTstFaceDlg::OnNMReleasedcaptureContrastSlider)
	ON_COMMAND(ID_HELP, &CbTstFaceDlg::OnHelp)
	ON_COMMAND(ID_ADJUST, &CbTstFaceDlg::OnAdjust)
	ON_COMMAND(ID_SETTING, &CbTstFaceDlg::OnSetting)
	ON_BN_CLICKED(IDC_CHECK3, &CbTstFaceDlg::OnBnOpenMotorChk)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SUDU_SLIDER, &CbTstFaceDlg::OnNMReleasedcaptureSuduSlider)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_COUNTCLS_BTN, &CbTstFaceDlg::OnBnClickedCountclsBtn)
	ON_STN_CLICKED(IDC_COUNT_A, &CbTstFaceDlg::OnStnClickedCountA)
	ON_STN_CLICKED(IDC_COUNT_B, &CbTstFaceDlg::OnStnClickedCountB)
	ON_STN_CLICKED(IDC_COUNT_C, &CbTstFaceDlg::OnStnClickedCountC)
	ON_STN_CLICKED(IDC_COUNT_D, &CbTstFaceDlg::OnStnClickedCountD)
END_MESSAGE_MAP()

//这里就是最大化之后调节的尺寸
BEGIN_EASYSIZE_MAP(CbTstFaceDlg)

	//EASYSIZE(IDC_PIC0,IDC_PICCTRL_FRMWORK,ES_BORDER,IDC_PIC1,IDC_PIC2,0)		//图片框0
	//EASYSIZE(IDC_PIC1,IDC_PIC0,ES_BORDER,IDC_COUNT_STATIC,IDC_PIC3,0)		//图片框1
	//EASYSIZE(IDC_PIC2,IDC_PICCTRL_FRMWORK,IDC_PIC0,IDC_PIC3,ES_BORDER,0)		//图片框2
	//EASYSIZE(IDC_PIC3,IDC_PIC2,IDC_PIC1,IDC_COUNT_STATIC,ES_BORDER,0)		//图片框3

	//EASYSIZE(IDC_PICCTRL_FRMWORK,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)		//图片框0

END_EASYSIZE_MAP


// CbTstFaceDlg 消息处理程序

BOOL CbTstFaceDlg::OnInitDialog()
{
	//_CrtSetBreakAlloc(500);  // for debug;
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	INIT_EASYSIZE;			//easysize初始化

	//DlgColor.r = 153;		//初始化对话框颜色
	//DlgColor.g = 204;
	//DlgColor.b = 153;

	//TextColor.r = 255;		//初始化文字颜色
	//TextColor.g = 0;
	//TextColor.b = 0;

	//StaticColor.r = 137;
	//StaticColor.g = 190;
	//StaticColor.b = 178;

	//从ini文件中读取数据，初始化程序
	DlgColor.r = GetPrivateProfileInt("ColorSetting", "DlgColorR", 153, ".\\setting.ini");
	DlgColor.g = GetPrivateProfileInt("ColorSetting", "DlgColorG", 204, ".\\setting.ini");
	DlgColor.b = GetPrivateProfileInt("ColorSetting", "DlgColorB", 153, ".\\setting.ini");

	TextColor.r = GetPrivateProfileInt("ColorSetting", "TextColorR", 255, ".\\setting.ini");
	TextColor.g = GetPrivateProfileInt("ColorSetting", "TextColorG", 0, ".\\setting.ini");
	TextColor.b = GetPrivateProfileInt("ColorSetting", "TextColorB", 0, ".\\setting.ini");

	StaticColor.r = GetPrivateProfileInt("ColorSetting", "StaticColorR", 137, ".\\setting.ini");
	StaticColor.g = GetPrivateProfileInt("ColorSetting", "StaticColorG", 190, ".\\setting.ini");
	StaticColor.b = GetPrivateProfileInt("ColorSetting", "StaticColorB", 178, ".\\setting.ini");

	AreaMin = GetPrivateProfileInt("ProgramSetting", "AreaMin", 20000, ".\\setting.ini");
	AreaMax = GetPrivateProfileInt("ProgramSetting", "AreaMax", 60000, ".\\setting.ini");
	dipScale = GetPrivateProfileInt("ProgramSetting", "dipScale", 143, ".\\setting.ini");
	pricision = GetPrivateProfileInt("ProgramSetting", "pricision", 10, ".\\setting.ini");
	innerBoundary = GetPrivateProfileInt("ProgramSetting", "innerBoundary", 60, ".\\setting.ini");
	outerBoundary = GetPrivateProfileInt("ProgramSetting", "outerBoundary", 60, ".\\setting.ini");
	Offset = GetPrivateProfileInt("ProgramSetting", "Offset", 9, ".\\setting.ini");

	int i, nCount = 0;
	CString KeyName;
	nCount = GetPrivateProfileInt("DataSetting", "nCount", 4, ".\\setting.ini");
	for (i = 0; i < nCount; i++)
	{
		KeyName.Format(_T("bulletHeiget%d"), i);
		heightSize[i] = (double)GetPrivateProfileInt("DataSetting", KeyName, 25, ".\\setting.ini") / 10;
		KeyName.Format(_T("bulletWidth%d"), i);
		widthSize[i] = (double)GetPrivateProfileInt("DataSetting", KeyName, 25, ".\\setting.ini") / 10;
	}

	//创建三个背景画刷
	//HBRUSH DlgColorBRUSH = CreateSolidBrush(RGB(DlgColor.r,DlgColor.g,DlgColor.b));	 //创建画刷RGB(174,221,129)
	//HBRUSH StaticColorBRUSH = CreateSolidBrush(RGB(StaticColor.r,StaticColor.g,StaticColor.b));	 //创建画刷RGB(174,221,129)
	//HBRUSH TextColorBRUSH = CreateSolidBrush(RGB(TextColor.r,TextColor.g,TextColor.b));	 //创建画刷RGB(174,221,129)

	//获得绘图矩形大小
	CWnd* pWnd = GetDlgItem(IDC_PIC0);
	pWnd->GetClientRect(&disp_rect);

	//滑动条和相应复选框初始化
	m_glabolThres_check.SetCheck(TRUE);
	m_lightAuto_ctl.SetCheck(TRUE);
	m_slider_light.SetRange(0, 255);			//亮度滑动条初始化
	m_slider_light.SetPos(100);				//设置默认值100，即不对图像增强
	m_slider_thres.SetRange(0, 255);
	m_contrast_ctl.SetRange(0, 64);			//对比度滑动条初始化
	m_contrast_ctl.SetPos(20);				//刚好使alpha为1，即不对图像增强

	m_outText.Format(_T("测试消息和提示消息显示在这里\r\n"));

	hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU1));		//添加菜单项快捷键

	ShowWindow(SW_MAXIMIZE);				//使窗体默认最大化
	//m_CvvImg.Create(480,640,24);

	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbTstFaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbTstFaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//还是不要绘制欢迎界面的好
		//welImg = cvLoadImage("../welcome.jpg",1);
		//if (!welImg)
		//{
		//	//AfxMessageBox(_T("open failed"),MB_OK,0);
		//}
		//else
		//{
		//	//AfxMessageBox(_T("open successed"),MB_OK,0);
		//	DrawIplImgToDC(welImg,IDC_PIC0);
		//	DrawIplImgToDC(welImg,IDC_PIC1);
		//	DrawIplImgToDC(welImg,IDC_PIC2);
		//	DrawIplImgToDC(welImg,IDC_PIC3);
		//}
		//cvReleaseImage(&welImg);
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CbTstFaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//调节控件大小
void CbTstFaceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

//给选中的打钩
void CbTstFaceDlg::OnUpdateLinkcam(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(statc);
}

//菜单选中的实现代码
void CbTstFaceDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: 在此处添加消息处理程序代码
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}


//CREATE响应
int CbTstFaceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//LOGFONT lf;
	//CFont *pFont = GetFont();
	//pFont->GetLogFont(&lf);
	//lf.lfHeight = 10;			//控制字体高度
	//lf.lfFaceName = "宋体";	//控制字体类型，利用LOGFONT这个结构里设置字体
	//m_editFont.CreateFontIndirect(&lf);

	return 0;
}
//窗口销毁事件
void CbTstFaceDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(1);
	cvReleaseCapture(&m_video);
	if (contourImg != NULL)
	{
		cvReleaseImage(&contourImg);
		contourImg = NULL;
	}
	if (backupImg != NULL)
	{
		cvReleaseImage(&backupImg);
		backupImg = NULL;
	}
	if (willImg != NULL)
	{
		cvReleaseImage(&willImg);
		willImg = NULL;
	}

}

BOOL CbTstFaceDlg::DestroyWindow()
{
	KillTimer(1);
	//cvReleaseImage(&srcImg);			//不知怎么的，这里有问题
	if (contourImg != NULL)
	{
		cvReleaseImage(&contourImg);
		contourImg = NULL;
	}
	if (backupImg != NULL)
	{
		cvReleaseImage(&backupImg);
		backupImg = NULL;
	}
	if (willImg != NULL)
	{
		cvReleaseImage(&willImg);
		willImg = NULL;
	}
	if (m_video != NULL)
	{
		cvReleaseCapture(&m_video);
		m_video = NULL;
	}

	return CDialog::DestroyWindow();
}


//亮度滑动条的响应函数
void CbTstFaceDlg::OnNMReleasedcaptureLightSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_light_value.Format(_T("%d"), m_slider_light.GetPos());			//显示到滑动条旁边
	UpdateData(FALSE);
	*pResult = 0;
}


//阈值滑动条的响应函数
void CbTstFaceDlg::OnNMReleasedcaptureThreSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_thres_value.Format(_T("%d"), m_slider_thres.GetPos());			//显示到滑动条旁边
	UpdateData(FALSE);
	*pResult = 0;
}


//对比度滑动条的响应函数
void CbTstFaceDlg::OnNMReleasedcaptureContrastSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_constrat_value.Format(_T("%d"), m_contrast_ctl.GetPos());			//显示到滑动条旁边
	UpdateData(FALSE);
	*pResult = 0;
}

//速度调节滑动条的响应函数
void CbTstFaceDlg::OnNMReleasedcaptureSuduSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//WriteToSerial( 0x00, m_sudu_slider.GetPos()%256);
	m_sudu_value.Format(_T("%d"), m_sudu_slider.GetPos());
	UpdateData(FALSE);
	*pResult = 0;
}

//主窗体的关于选项
void CbTstFaceDlg::OnaboutClick()
{
	//this->ShowWindow(FALSE);
	//CAboutDlg dl;
	//dl.DoModal();
	//OnOK();
	//if (dlg->GetSafeHwnd() == 0)
	//{
	//	dlg->Create(IDD_ABOUTBOX);
	//}
	//dlg->ShowWindow(SW_SHOW);		//显示关于窗口
	CAboutDlg *dlg = NULL;
	dlg = new CAboutDlg;
	dlg->Create(IDD_ABOUTBOX, NULL);
	dlg->CenterWindow();
	dlg->ShowWindow(SW_SHOW);

}


//主窗体的设置选项的实现
void CbTstFaceDlg::OnSetting()
{
	UpdateData(TRUE);
	CSetting *dlg = NULL;
	dlg = new CSetting;
	dlg->Create(IDD_SETTING, NULL);
	dlg->m_dipScale = dipScale;
	dlg->m_pricision = pricision;
	dlg->m_areaMin = AreaMin;
	dlg->m_areaMax = AreaMax;
	dlg->UpdateData(FALSE);
	dlg->CenterWindow();
	dlg->ShowWindow(SW_SHOW);
}
//主窗体的标定选项
void CbTstFaceDlg::OnAdjust()
{
	if (srcImg)
	{
		CAdjust *dlg = NULL;
		dlg = new CAdjust;
		dlg->Create(IDD_CALIBRAT, NULL);
		dlg->CenterWindow();
		dlg->ShowWindow(SW_SHOW);
	}
	else
	{
		AfxMessageBox(_T("请先打开摄像头，放置好标定板。"), MB_OK, 0);
	}
}

//主窗体的帮助选项
void CbTstFaceDlg::OnHelp()
{
	Dsicribe *dlg = NULL;
	dlg = new Dsicribe;
	dlg->Create(IDD_HELP, NULL);
	dlg->CenterWindow();
	dlg->ShowWindow(SW_SHOW);
}
//菜单的退出按钮实现
void CbTstFaceDlg::OnMenuExit()
{
	// TODO: 在此添加命令处理程序代码
	if (IDYES == AfxMessageBox(_T("确定退出程序吗？"), MB_YESNO, 0))
	{
		CDialog::OnCancel();			//关闭程序
	}
}


//连接摄像头菜单项
void CbTstFaceDlg::OnLinkcam()
{
	// TODO: 在此添加命令处理程序代码
	type_sum = 0;					//子弹的总数
	type_count[0] = 0;					//A,B,C,D型子弹计数
	type_count[1] = 0;
	type_count[2] = 0;
	type_count[3] = 0;
	type_count[4] = 0;
	/*CString str;
	str.Format(_T("com:%d\ninnerBoundary:%d\nconsultMin:%d\nconsultMax:%d\ndipScale:%0.1lf\nprision:%0.1lf\n"),com,innerBoundary,AreaMin,AreaMax,dipScale,pricision);
	str = str + botlv;*/
	//AfxMessageBox(str,MB_OK,0);				//测试用

	statc = !statc;			//开关取反
	if (statc)
	{

		m_video = cvCreateCameraCapture(0);			//从摄像头读取图像
		//m_video = cvCreateFileCapture("..\\money1.mp4");		//测试用
		if (!m_video)
		{
			statc = FALSE;
			AfxMessageBox(_T("无法打开摄像头"), MB_OK, 0);
			return;
		}

		srcImg = cvQueryFrame(m_video);          //先获取摄像头一帧图片数据

		if (srcImg == NULL) {
			statc = FALSE;
			AfxMessageBox(_T("获取视频帧失败！"), MB_OK, 0);
			return;
		}

		contourImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 1);			//用来画最大轮廓的图像
		backupImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 3);			//用了保存仅进行过亮度处理的图片，以用于斑点检测
		willImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 3);			//得到的子弹待处理图片

		SetTimer(1, 10, NULL);              //开启定时器，每隔100ms读取一次视频数据并处理图像


	}
	else
	{
		//关闭摄像头操作：清除capture,iplimage,dc等,关闭定时器
		KillTimer(1);
		//cvReleaseImage(&srcImg);					//企图释放变量导致错误
	}
}

//定时器内对图像进行处理，得到前景图片
void CbTstFaceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CvMemStorage* contour_mem = cvCreateMemStorage(0);			//用来存Seq*
	CvSeq* contour;				//用来存储找到的contour
	CString strTemp;

	srcImg = cvQueryFrame(m_video);

	// for test code
	if (srcImg == NULL)
	{
		if (m_video != NULL)
		{
			cvReleaseCapture(&m_video);
			m_video = NULL;
		}
		//m_video = cvCreateFileCapture("..\\wuziqi.mp4");		//测试用
		m_video = cvCreateCameraCapture(0);
		if (!m_video)
		{
			statc = FALSE;
			AfxMessageBox(_T("无法打开摄像头"), MB_OK, 0);
			return;
		}

		srcImg = cvQueryFrame(m_video);          //先获取摄像头一帧图片数据

	} //for test code end

	//DrawIplImgToDC(srcImg,IDC_PIC0);		//显示视频
	//原来想直方图均衡和对比度亮度调节只选择一个就好，实验发现亮度调节还是要有的

	cvConvertScale(srcImg, srcImg, 0.05*m_contrast_ctl.GetPos(), m_slider_light.GetPos() - 100);			//对比度和亮度增强
	DrawIplImgToDC(srcImg, IDC_PIC0);		//显示视频

	cvCopy(srcImg, backupImg);		//备份图片，check color;

	if (m_lightAuto_ctl.GetCheck())			//自动调节亮度按钮被选中
	{
		EqualizeHistColorImage(srcImg, srcImg);			//用直方图均衡化来增强图像
	}

	enterTimercount++;					//统计进入定时器次数变量自加
	if (!srcImg)				//测试用，测试视频完毕的处理，异常终结
	{
		AfxMessageBox(_T("退出程序！"), MB_OK, 0);
		exit(0);		//退出程序
	}

#if DETECT_BOTTLE
	FindContourBoxAndHeader(srcImg, contour_mem, &contour);
#else
	FindContourBoxAndWillImg(srcImg, contour_mem, &contour);		//求图像BOX和contours，并得到需要处理的图像
#endif
	//如果图像更新
	//CString str;
#if 1
	if (flag == 1)			//如果图像更新
	{
		//double width=0, height=0;
		int cnt = 0;
		while (cnt < MAX_NUM) {

			//width = object_width[cnt];
			//height = object_height[cnt];
			if ((object_width[cnt] == 0) || (object_height[cnt] == 0))
			{
				cnt++;
				continue;
			}

			int type = 0;
			type = MatchTypeAcorSize(pricision, object_height[cnt], object_width[cnt]);			//检测型号，返回整数,-1:出错,0:A型,1:B型，2:C型，3:D型,==>>MatchType()内部有错误<<==

			if (type == 1)
			{
				int type = CheckColor(backupImg, cnt);
			}

			if (type == -1)
			{
				//AfxMessageBox(_T("未知物体，无参考尺寸\n \n 请重试或检查尺寸数据是否出错"), MB_OK, 0);
				cnt++;
				continue;
			}

			else //if (type != -1)
			{
				strTemp.Format(_T("%02d型\n长%6.2lfpixel\t宽%6.2lfpixel\t长%6.2lfcm\t宽%6.2lfcm"), type, object_height[cnt], object_width[cnt], object_height[cnt] / dipScale, object_width[cnt] / dipScale);		//加入型号
				m_type_text.Format(_T("当前型号：") + strTemp);
				if (canPlus == 1)		//如果可以计数
				{

					if (type_sum < INT_MAX)		//防溢出处理
						type_sum++;				//LRF201601061910
					else
						type_sum = 0;
					if (type_count[type] < INT_MAX)
						type_count[type]++;		//LRF201601061910
					else
						type_count[type] = 0;			//计数累加器

					//send_data_flag = 1;
					//if( type < 5)			//只有前面5个才会向串口发送数据
					{
						if (send_data_flag)
						{
							// first send type;
							union FC
							{
								float fData;
								char cData[4];
							}tmp;
							tmp.fData = Offset - center_x[cnt] / dipScale; //0-center_x[cnt]/dipScale - Offset;
							tmp.fData *= 10.0;		///LRF 201601061138AM
#if DETECT_BOTTLE
							senddata[0] = header[cnt];
#else
							senddata[0] = (type + 1) % 256;
#endif
							senddata[1] = tmp.cData[0];
							senddata[2] = tmp.cData[1];
							senddata[3] = tmp.cData[2];
							senddata[4] = tmp.cData[3];
							/*tmp.fData = center_y[cnt]/dipScale;
							senddata[5] = tmp.cData[0];
							senddata[6] = tmp.cData[1];
							senddata[7] = tmp.cData[2];
							senddata[8] = tmp.cData[3];*/
							WriteToSerial(senddata, 0x01);		//发送数据到串口
							send_data_finish = 1;
#if 0
							switch (type)
							{
							case 0:m_typeA_value.Format(_T("%4d"), type_count[type]);
								break;
							case 1:m_typeB_value.Format(_T("%4d"), type_count[type]);
								break;
							case 2:m_typeC_value.Format(_T("%4d"), type_count[type]);
								break;
							case 3:m_typeD_value.Format(_T("%4d"), type_count[type]);
								break;
							case 4:m_typeD_value.Format(_T("%4d"), type_count[type]);
								break;
							}
#endif
						}
					}
					m_type_sum.Format(_T("%4d"), type_sum);
					canPlus = 0;		//计数一次，不能再计数

				}
			}
			cnt++;
		}
		if ((send_data_flag == 1) && (send_data_finish == 1))
		{
			send_data_flag = 0;
		}
		//switch (type)
		//{
		//case 0:m_type_text.Format(_T("当前型号:A型\n\n参考尺寸:\n长%6.2lfCM\t宽%6.2lfCM"),heightSize[type]/dipScale,widthSize[type]/dipScale);
		//	if (canPlus == 1)		//如果可以计数
		//	{
		//		type_sum++,type_count[type]++;
		//		WriteToSerial( ( type + 1 ) % 256 );
		//		canPlus =  0;
		//	}
		/*	m_type_sum.Format(_T("%4d"),type_sum);
			m_typeA_value.Format(_T("%4d"),type_count[type]);*/
			//	break;
			//case 1:m_type_text.Format(_T("当前型号:B型\n\n参考尺寸:\n长%6.2lfCM\t宽%6.2lfCM"),heightSize[type]/dipScale,widthSize[type]/dipScale);
			//	if (canPlus == 1)		//如果可以计数
			//	{
			//		type_sum++,type_count[type]++;
			//		WriteToSerial( ( type + 1 ) % 256 );
			//		canPlus =  0;
			//	}
			//	m_type_sum.Format(_T("%4d"),type_sum);
			//	m_typeB_value.Format(_T("%4d"),type_count[type]);
			//	break;
			//case 2:m_type_text.Format(_T("当前型号:C型\n\n参考尺寸:\n长%6.2lfCM\t宽%6.2lfCM"),heightSize[type]/dipScale,widthSize[type]/dipScale);
			//	if (canPlus == 1)		//如果可以计数
			//	{
			//		type_sum++,type_count[type]++;
			//		WriteToSerial( ( type + 1 ) % 256 );
			//		canPlus =  0;
			//	}
			//	m_type_sum.Format(_T("%4d"),type_sum);
			//	m_typeC_value.Format(_T("%4d"),type_count[type]);
			//	break;
			//case 3:m_type_text.Format(_T("当前型号:D型\n\n参考尺寸:\n长%6.2lfCM\t宽%6.2lfCM"),heightSize[type]/dipScale,widthSize[type]/dipScale);
			//	if (canPlus == 1)		//如果可以计数
			//	{
			//		type_sum++,type_count[type]++;
			//		WriteToSerial( ( type + 1 ) % 256 );
			//		canPlus =  0;
			//	}
			//	m_type_sum.Format(_T("%4d"),type_sum);
			//	m_typeD_value.Format(_T("%4d"),type_count[type]);
			//	break;
			//case 4:strTemp.Format(_T("当前型号:E型\n参考尺寸:\n长%6.2lfCM\t宽%6.2lfCM\n计数：%4d"),heightSize[type]/dipScale,widthSize[type]/dipScale,type_sum);
			//	m_outText += strTemp;
			//	if (canPlus == 1)		//如果可以计数
			//	{
			//		type_sum++,type_count[type]++;
			//		//WriteToSerial( ( type + 1 ) % 256 );
			//		canPlus =  0;
			//	}
			//	m_type_sum.Format(_T("\n%4d"),type_sum);
			//	//m_typeD_value.Format(_T("%4d"),type_count[4]);
			//	break;
			//case 5:strTemp.Format(_T("当前型号:F型\n参考尺寸:\n长%6.2lfCM\t宽%6.2lfCM\n计数：%4d"),heightSize[type]/dipScale,widthSize[type]/dipScale,type_sum);
			//	m_outText += strTemp;
			//	if (canPlus == 1)		//如果可以计数
			//	{
			//		type_sum++,type_count[type]++;
			//		//WriteToSerial( ( type + 1 ) % 256 );
			//		canPlus =  0;
			//	}
			//	m_type_sum.Format(_T("\n%4d"),type_sum);
			//	//m_typeD_value.Format(_T("%4d"),type_count[4]);
			//	break;
			//default:m_type_text.Format(_T("未知物体，无参考尺寸\n \n 请重试或检查尺寸数据是否出错"));
			//	break;
			//}

			//CheckFault(backupImg);		//检测缺陷并画出缺陷

		flag = 0;				//置图像状态默认没有被更新
	}
#endif
	//m_bullettype_ctl.SetWindowText(str);
	cvReleaseMemStorage(&contour_mem);

	UpdateData(FALSE);			//将数据更新到文本框

	CDialog::OnTimer(nIDEvent);
}

//传入图像，传出轮廓，BOX，和可以用的图像
void CbTstFaceDlg::FindContourBoxAndWillImg(IplImage* srcIn, CvMemStorage* contour_mem, CvSeq** contour)
{

	int thres = 0;
	CvRect rect;
	CvPoint pt;
	int count = 0;
	int valid = 0;
	double minarea = 1500;
	int j = 0;
	for (int i = 0; i < MAX_NUM; i++)
	{
		center_x[i] = 0;
		center_y[i] = 0;
		object_height[i] = 0;		//得到一般意义上的长和宽(像素)
		object_width[i] = 0;
	}

	IplImage* dst = cvCreateImage(cvGetSize(srcIn), IPL_DEPTH_8U, 1);				//用来存放COPY的图像
	IplImage* test = cvCreateImage(cvGetSize(srcIn), IPL_DEPTH_8U, 1);				//用来存放COPY的图像
	IplImage* color = cvCreateImage(cvGetSize(srcIn), IPL_DEPTH_8U, 3);
	//cvCvtColor (srcIn,dst,CV_BGR2GRAY);
	cvZero(color);
	//cvCvtColor (srcIn, color, CV_BGR2HSV);
	//DrawIplImgToDC(color,IDC_PIC1);
	/*if (m_glabolThres_check.GetCheck())
	{
		thres = BasicGlobalThreshold(dst);			//使用全局阈值
		m_slider_thres.SetPos(thres);				//将阈值滑动条设置到全局阈值找到的值
		m_thres_value.Format(_T("%d"),m_slider_thres.GetPos());		//同时更新旁边的显示
		cvThreshold(dst,dst,thres,255,CV_THRESH_BINARY_INV);			//全局阈值法二值化图像
	}
	else*/
	{
#if ADD_DETECT_COLOR
		CvScalar s;
		for (int i = 0; i < srcIn->height; i++) {
			for (int j = 0; j < srcIn->width; j++) {
				s = cvGet2D(srcIn, i, j);
				double b = s.val[0];
				double g = s.val[1];
				double r = s.val[2];
				if (r >= 10)
				{
					cvSet2D(color, i, j, CV_RGB(255, 255, 255));
				}
				else {
					cvSet2D(color, i, j, CV_RGB(0, 0, 0));
				}
			}
		}
		DrawIplImgToDC(contourImg, IDC_PIC1);
		cvCvtColor(color, dst, CV_BGR2GRAY);
		cvThreshold(dst, dst, m_slider_thres.GetPos(), 255, CV_THRESH_BINARY_INV);
#else 
		cvCvtColor(srcIn, dst, CV_BGR2GRAY);
		cvThreshold(dst, dst, m_slider_thres.GetPos(), 255, CV_THRESH_BINARY);
#endif

	}



	cvSmooth(dst, dst, CV_GAUSSIAN, 3, 3);			//滤波处理
	cvDilate(dst, dst, 0, 2);			//膨胀2次
	cvErode(dst, dst, 0, 1);			//腐蚀1次
	DrawIplImgToDC(dst, IDC_PIC2);	//测试用，现在改为显示的一部分，因为斑点检测没有实现
#if 0
	cvZero(contourImg);
	*contour = cvHoughCircles(dst, contour_mem, CV_HOUGH_GRADIENT, 2, dst->height / 4, 200, 100);
	//************* 
	for (int i = 0; i < (*contour)->total; i++)
	{
		float* p = (float*)cvGetSeqElem(*contour, i);//霍夫圆变换
		pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
		cvCircle(contourImg, pt, cvRound(p[2]), CV_RGB(255, 255, 255)); //确定圆心 确定半径 画圆函数
		DrawIplImgToDC(contourImg, IDC_PIC3);
	}
	//DrawIplImgToDC(contourImg,IDC_PIC3);
#endif
#if 1
	//cvFindContours(dst, contour_mem, contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cvFindContours(dst, contour_mem, contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	cvZero(contourImg);
	cvZero(test);

	//there is something wrong with this function, need to check;
	//cvDrawContours(dst, *contour, CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), 0, 1, 8);   //绘制外部和内部的轮廓  

	for (CvSeq* c = *contour; c; c = c->h_next)
	{
		double tmparea = fabs(cvContourArea(c));
		if (tmparea < minarea)
		{
			cvSeqRemove(c, 0); // 删除面积小于设定值的轮廓
			continue;
		}
#if 1
		rect = cvBoundingRect(c, 0);
		int big = (rect.width > rect.height) ? rect.width : rect.height;
		int Small = (rect.width > rect.height) ? rect.height : rect.width;
		int diff = big - Small;
		//if (diff < 5) {
		cvRectangle(contourImg, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width - 1, rect.y + rect.height - 1), CV_RGB(255, 255, 255), 1, 8, 0);
		pt.x = (rect.x + rect.x + rect.width) / 2;
		pt.y = (rect.y + rect.y + rect.height) / 2;
		cvCircle(contourImg, pt, 2, CV_RGB(255, 255, 255), CV_FILLED);
		cvDrawContours(contourImg, c, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), -1, 1, 8);
		cvSeqRemove(c, 0); // 删除面积小于设定值的轮廓
	/*} else {
		pt.x = 0;
		pt.y = 0;
		if ((rect.width > 100)||(rect.height > 100))
		{
			cvDrawContours(test, c, CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), -1, 1, 8);
		}
		cvRectangle(contourImg, cvPoint(rect.x, rect.y), cvPoint(rect.x+rect.width-1, rect.y+rect.height-1),CV_RGB(255,255,255),1,8,0);
		pt.x = (rect.x + rect.x+rect.width)/2;
		pt.y = (rect.y + rect.y+rect.height)/2;
		cvCircle(contourImg, pt,  2, CV_RGB( 255, 255, 255), CV_FILLED );
		cvDrawContours(test, c, CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), -1, 1, 8);
	}*/
	//cvDrawContours(contourImg, c, CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), -1, 1, 8);   //绘制外部和内部的轮廓 , *contour, CV_RGB( 255, 255, 255), CV_RGB( 255, 255, 255), 0, 1, 8);   //绘制外部和内部的轮廓 
#else
		//声明变量
		CvPoint2D32f rect_p32f[4];						//存储4个顶点坐标值
		int npts = 4, tiaozheng = 0;									//顶点计数
		CvPoint rect_pts[4], *pt = rect_pts;

		//用白色描出“子弹”(最大轮廓)
		cvDrawContours(contourImg, c, cvScalarAll(255), cvScalarAll(255), -1, -1);

		//找出完整包含轮廓的最小矩形
		rect = cvMinAreaRect2(c);

		cvBoxPoints(rect, rect_p32f);			//找BOX的四个顶点,用cvPoint2D32f数组存起来
		for (int i = 0; i < 4; i++)
			rect_pts[i] = cvPointFrom32f(rect_p32f[i]);		//将cvPoint2D32f转为cvPoint型

		//用红色画出Box
		cvPolyLine(contourImg, &pt, &npts, 1, 1, CV_RGB(0, 255, 0), 1);

#endif
		//count++;
		//DrawIplImgToDC(contourImg,IDC_PIC3);
#endif
#if 0
		if ((pt.x != 0) && (pt.y <= (srcIn->height / 2 + innerBoundary) && pt.y >= (srcIn->height / 2 - innerBoundary)))
		{
			canPlus = 1;
			flag = 1;
			if (j < MAX_NUM) {
				center_x[j] = pt.x;
				center_y[j] = pt.y;
				object_height[j] = rect.height>rect.width ? rect.height : rect.width;		//得到一般意义上的长和宽(像素)
				object_width[j] = rect.height < rect.width ? rect.height : rect.width;
				j++;
			}
		}
		else {
			canPlus = 0;
			flag = 0;
		}
#endif
#if 1
		if ((pt.x != 0) && (pt.y != 0))
		{
			//if ( pt.y >  srcIn->height - outerBoundary || pt.y <  outerBoundary || pt.x > srcIn->width - outerBoundary || pt.x < outerBoundary )		//如果视域内子弹碰到边线，就可以开始计数
			if (pt.y > srcIn->height / 2 - outerBoundary && pt.y < srcIn->height / 2 + outerBoundary)
			{
				canPlus = 1;
				//AfxMessageBox(_T("hello"),MB_OK,0);
			}
			if (pt.y <= (srcIn->height / 2 + innerBoundary) && pt.y >= (srcIn->height / 2 - innerBoundary) /*&& \
				pt.x <= (srcIn->width/2 +innerBoundary) && pt.x >= (srcIn->width/2 -innerBoundary)*/)
			{
				//返回更新信息
				flag = 1;				//图像被更新
				//valid = 1;
			}
			//if ((canPlus == 1) && (flag == 1))
			if (flag == 1)
			{
				//valid = 0;
				if (j < MAX_NUM) {
					center_x[j] = pt.x;
					center_y[j] = pt.y;
					object_height[j] = rect.height>rect.width ? rect.height : rect.width;		//得到一般意义上的长和宽(像素)
					object_width[j] = rect.height < rect.width ? rect.height : rect.width;
					j++;
				}
				//m_bullet_size.Format(_T("物体[%d]\t长:%0.2lfCM\t宽:%0.2lfCM\n中心点坐标(%2d , %2d)"), count, object_height[j]/dipScale,object_width[j]/dipScale, center_x[j], center_y[j]);
			//cvCopy(contourImg,willImg);
			}
		}
#endif
	}
#if 1
	//*contour=cvHoughCircles(contourImg,contour_mem,CV_HOUGH_GRADIENT,2,contourImg->height/4,200,100, 50, 80);
	cvSmooth(test, test, CV_GAUSSIAN, 3, 3);			//滤波处理
	*contour = cvHoughCircles(test, contour_mem, CV_HOUGH_GRADIENT, 2, 45, 150, 45, 40, 85);
	//************* 
	for (int i = 0; i < (*contour)->total; i++)
	{
		float* p = (float*)cvGetSeqElem(*contour, i);//霍夫圆变换
		pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
		cvCircle(contourImg, pt, cvRound(p[2]), CV_RGB(255, 255, 255)); //确定圆心 确定半径 画圆函数
		cvCircle(contourImg, pt, 2, CV_RGB(255, 255, 255), CV_FILLED);
#if 0
		if ((pt.x != 0) && (pt.y != 0))
		{
			canPlus = 1;
			flag = 1;
			if (j < MAX_NUM) {
				center_x[j] = cvRound(p[0]);
				center_y[j] = cvRound(p[1]);
				object_height[j] = cvRound(p[2]) * 2;		//得到一般意义上的长和宽(像素)
				object_width[j] = cvRound(p[2]) * 2;
				j++;
			}
		}
		else {
			canPlus = 0;
			flag = 0;
		}
#endif
		if ((pt.x != 0) && (pt.y != 0))
		{
			if (pt.y > srcIn->height - outerBoundary || pt.y <  outerBoundary || pt.x > srcIn->width - outerBoundary || pt.x < outerBoundary)		//如果视域内子弹碰到边线，就可以开始计数
			{
				canPlus = 1;
			}
			if (pt.y <= (srcIn->height / 2 + innerBoundary) && pt.y >= (srcIn->height / 2 - innerBoundary) && \
				pt.x <= (srcIn->width / 2 + innerBoundary) && pt.x >= (srcIn->width / 2 - innerBoundary))
			{
				//返回更新信息
				flag = 1;				//图像被更新
			}
			if (flag == 1)
			{
				if (j < MAX_NUM) {
					center_x[j] = cvRound(p[0]);
					center_y[j] = cvRound(p[1]);
					object_height[j] = cvRound(p[2]) * 2;		//得到一般意义上的长和宽(像素)
					object_width[j] = cvRound(p[2]) * 2;
					j++;
				}
			}
		}

	}

#endif
	//DrawIplImgToDC(dst,IDC_PIC3);
	DrawIplImgToDC(contourImg, IDC_PIC3);
	UpdateData(FALSE);
	cvReleaseImage(&dst);
	cvReleaseImage(&test);
	cvReleaseImage(&color);
}


//传入图像，传出轮廓，BOX，和可以用的图像
void CbTstFaceDlg::FindContourBoxAndHeader(IplImage* srcIn, CvMemStorage* contour_mem, CvSeq** contour)
{

	int thres = 0;
	CvRect rect;
	CvPoint pt;
	int count = 0;
	int valid = 0;
	double minarea = 1500;
	int j = 0;
	for (int i = 0; i < MAX_NUM; i++)
	{
		center_x[i] = 0;
		center_y[i] = 0;
		object_height[i] = 0;		//得到一般意义上的长和宽(像素)
		object_width[i] = 0;
	}

	IplImage* dst = cvCreateImage(cvGetSize(srcIn), IPL_DEPTH_8U, 1);				//用来存放COPY的图像
	IplImage* test = cvCreateImage(cvGetSize(srcIn), IPL_DEPTH_8U, 1);				//用来存放COPY的图像
	IplImage* color = cvCreateImage(cvGetSize(srcIn), IPL_DEPTH_8U, 3);
	//cvZero(color);
	//cvCvtColor (srcIn, color, CV_BGR2HSV);
	//DrawIplImgToDC(color,IDC_PIC1);
	/*if (m_glabolThres_check.GetCheck())
	{
		thres = BasicGlobalThreshold(dst);			//使用全局阈值
		m_slider_thres.SetPos(thres);				//将阈值滑动条设置到全局阈值找到的值
		m_thres_value.Format(_T("%d"),m_slider_thres.GetPos());		//同时更新旁边的显示
		cvThreshold(dst,dst,thres,255,CV_THRESH_BINARY_INV);			//全局阈值法二值化图像
	}
	else*/
	{
#if ADD_DETECT_COLOR
		CvScalar s;
		//cvCopy(srcIn,color);
		for (int i = 0; i < srcIn->height; i++) {
			for (int j = 0; j < srcIn->width; j++) {
				s = cvGet2D(srcIn, i, j);
				double b = s.val[0];
				double g = s.val[1];
				double r = s.val[2];

				if (g > 0)
				{
					s.val[1] = 0;
					g = 0;
					cvSet2D(color, i, j, s);
				}
				if ((r <= 10) || (b < 10))
				{
					s.val[0] = 0;
					s.val[1] = 0;
					s.val[2] = 0;
					cvSet2D(color, i, j, s);
				}
			}
		}
		DrawIplImgToDC(color, IDC_PIC1);
		cvCvtColor(color, dst, CV_BGR2GRAY);
		cvThreshold(dst, dst, m_slider_thres.GetPos(), 200, CV_THRESH_BINARY);
		/*thres = BasicGlobalThreshold(dst);			//使用全局阈值
		m_slider_thres.SetPos(thres);				//将阈值滑动条设置到全局阈值找到的值
		m_thres_value.Format(_T("%d"),m_slider_thres.GetPos());		//同时更新旁边的显示
		cvThreshold(dst,dst,thres,255,CV_THRESH_BINARY);			//全局阈值法二值化图像*/
#else 
		cvCvtColor(srcIn, dst, CV_BGR2GRAY);
		cvThreshold(dst, dst, m_slider_thres.GetPos(), 255, CV_THRESH_BINARY);
#endif

	}



	cvSmooth(dst, dst, CV_GAUSSIAN, 3, 3);			//滤波处理
	cvDilate(dst, dst, 0, 2);			//膨胀2次
	cvErode(dst, dst, 0, 1);			//腐蚀1次
	DrawIplImgToDC(dst, IDC_PIC2);	//测试用，现在改为显示的一部分，因为斑点检测没有实现

	cvFindContours(dst, contour_mem, contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	cvZero(contourImg);
	cvZero(test);

	for (CvSeq* c = *contour; c; c = c->h_next)
	{
		double tmparea = fabs(cvContourArea(c));
		if (tmparea < minarea)
		{
			cvSeqRemove(c, 0); // 删除面积小于设定值的轮廓
			continue;
		}

		rect = cvBoundingRect(c, 0);
		cvRectangle(contourImg, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width - 1, rect.y + rect.height - 1), CV_RGB(255, 255, 255), 1, 8, 0);
		if ((rect.height > 420) || (rect.height < 336))
		{
			cvSeqRemove(c, 0); // 删除面积小于设定值的轮廓
			continue;
		}
		pt.x = (rect.x + rect.x + rect.width) / 2;
		pt.y = (rect.y + rect.y + rect.height) / 2;
		cvCircle(contourImg, pt, 2, CV_RGB(255, 255, 255), CV_FILLED);
		cvDrawContours(contourImg, c, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, 1, 8);
		//cvSeqRemove(c, 0); // 删除面积小于设定值的轮廓
		cvDrawContours(test, c, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, 1, 8);

		if ((pt.x != 0) && (pt.y != 0))
		{
			//if ( pt.y >  srcIn->height - outerBoundary || pt.y <  outerBoundary || pt.x > srcIn->width - outerBoundary || pt.x < outerBoundary )		//如果视域内子弹碰到边线，就可以开始计数
			if (pt.y > srcIn->height / 2 - outerBoundary && pt.y < srcIn->height / 2 + outerBoundary)
			{
				canPlus = 1;
				//AfxMessageBox(_T("hello"),MB_OK,0);
			}
			if (pt.y <= (srcIn->height / 2 + innerBoundary) && pt.y >= (srcIn->height / 2 - innerBoundary) /*&& \
				pt.x <= (srcIn->width/2 +innerBoundary) && pt.x >= (srcIn->width/2 -innerBoundary)*/)
			{
				//返回更新信息
				flag = 1;				//图像被更新
				//valid = 1;
			}
			//if ((canPlus == 1) && (flag == 1))
			if (flag == 1)
			{
				//valid = 0;
				if (j < MAX_NUM) {
					center_x[j] = pt.x;
					center_y[j] = pt.y;
					object_height[j] = rect.height>rect.width ? rect.height : rect.width;		//得到一般意义上的长和宽(像素)
					object_width[j] = rect.height < rect.width ? rect.height : rect.width;
					j++;
				}
				//m_bullet_size.Format(_T("物体[%d]\t长:%0.2lfCM\t宽:%0.2lfCM\n中心点坐标(%2d , %2d)"), count, object_height[j]/dipScale,object_width[j]/dipScale, center_x[j], center_y[j]);
			//cvCopy(contourImg,willImg);
			}
		}
	}

	cvSmooth(test, test, CV_GAUSSIAN, 7, 7);			//滤波处理
	*contour = cvHoughCircles(test, contour_mem, CV_HOUGH_GRADIENT, 19, 150, 250, 200, 20, 40);
	//************* 
	for (int i = 0; i < (*contour)->total; i++)
	{
		float* p = (float*)cvGetSeqElem(*contour, i);//霍夫圆变换
		pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
		//cvCircle(contourImg,pt,cvRound(p[2]),CV_RGB( 255,255, 255 )); //确定圆心 确定半径 画圆函数
		//cvCircle(contourImg, pt,  2, CV_RGB( 255, 255, 255), CV_FILLED );

		if ((pt.x != 0) && (pt.y != 0))
		{
			int j = 0;
			while (center_x[j] != 0)
			{
				if ((pt.x > center_x[j] - object_width[j] / 2) && (pt.x < center_x[j] + object_width[j] / 2))
				{
					if (((pt.y - center_y[j]) < object_height[j] / 2) && ((pt.y - center_y[j]) > object_height[j] / 3))
					{
						header[j] = 1;
						cvCircle(contourImg, pt, cvRound(p[2]), CV_RGB(255, 255, 255)); //确定圆心 确定半径 画圆函数
						cvCircle(contourImg, pt, 2, CV_RGB(255, 255, 255), CV_FILLED);
					}
					else if (((pt.y - center_y[j]) < -object_height[j] / 3) && ((pt.y - center_y[j]) > -object_height[j] / 2))
					{
						header[j] = 2;
						cvCircle(contourImg, pt, cvRound(p[2]), CV_RGB(255, 255, 255)); //确定圆心 确定半径 画圆函数
						cvCircle(contourImg, pt, 2, CV_RGB(255, 255, 255), CV_FILLED);
					}
				}
				j++;
				if (j > MAX_NUM)
					break;
			}
		}
	}

	//DrawIplImgToDC(dst,IDC_PIC3);
	DrawIplImgToDC(contourImg, IDC_PIC3);
	UpdateData(FALSE);
	cvReleaseImage(&dst);
	cvReleaseImage(&test);
	cvReleaseImage(&color);

}

//匹配子弹型号的函数,-1读取文件出错，0,1,2,3分别为匹配结果
int CbTstFaceDlg::MatchTypeAcorCon(CvSeq** contour)
{
	CvMemStorage* storage = cvCreateMemStorage(0);			//创建一块默认大小的内存
	CvSeq* seq;// = cvCreateSeq(0, sizeof(CvSeq),sizeof(CvPoint),storage);			//存储模板的轮廓
	IplImage* modelImg = 0;
	char* filename = 0;
	double index[4] = { 0 }, p_min = 100;;			//用来保存匹配结果
	int a = 0, i = 0;
	CString str;

	//ifstream fin("..\\imagelist.txt");		//文件读取
	//const int LINE_LENGTH = 100;
	//char str[LINE_LENGTH];

	for (i = 0; i < 4; i++)			//依次读入模板比对
	{
		sprintf_s(filename, 10 * sizeof(char), "%s%d%s", "..\\", i, ".jpg");		//拼写文件名
		//CString str;
		//str.Format(_T("%s%d%s"),"..\\",i,".jpg");
		//AfxMessageBox(str,MB_OK,0);				//测试用

		//system("pause");			//测试用
		/*fin.getline(str,LINE_LENGTH);*/

		//str.Format(_T("%s%d%s"),_T("F:/RongfangLiu/Projects/bTstFace_20131206PM/"),i,_T(".jpg"));
		//AfxMessageBox(str,MB_OK,0);
		//filename = CString2char(str);

		modelImg = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);		//读模板图片,存为灰度图像，因为模板是二值化图像，这里不做二值化
		if (!modelImg)				//如果读取图片失败
		{
			return -1;
		}
		getImageContour(modelImg, storage, &seq);			//已经修改，使用opencv风格的接口，注意，getImageContour()里木有释放Memstorage,这里Seq应该还是使用那块内存，每次调用估计都会积累一块内存
		index[i] = cvMatchShapes(*contour, seq, 1);
	}

	p_min = index[0];			//假设第一个为最小值
	for (i = 1; i < 4; i++)			//找到最小值所在的位置
	{
		if (index[i] < p_min)
		{
			a = i;		//当前位置为最小
			p_min = index[i];		//将最小值修改为当前
		}
	}

	str.Format(_T("A:%0.1f\nB:%0.1f\nC:%0.1f\nD:%0.1f\n"), index[0], index[1], index[2], index[3]);
	//AfxMessageBox(str,MB_OK,0);			//测试用

	cvReleaseImage(&modelImg);
	cvReleaseMemStorage(&storage);


	//if (index[a] < 10000)		//其实不应该因为匹配结果不好就不返回，需要的是最接近的，然后再与标准的比较得到是否合格
	//	return a;			//如果匹配结果比较好,返回匹配结果
	//else
	//	return -1;			//匹配失败

	return a;

}


//基于尺寸的型号识别
int CbTstFaceDlg::MatchTypeAcorSize(double pricision, double height, double width)
{
	//pricision是精度值
	int i;
	for (i = 0; i < 10; i++)		//在prision这个精度下循环匹配尺寸表
	{
		if (height > heightSize[i] * dipScale - pricision && height < heightSize[i] * dipScale + pricision \
			&& width > widthSize[i] * dipScale - pricision && width < widthSize[i] * dipScale + pricision)			//进行匹配
		{
			return i;				//匹配成功，返回序号
		}
	}
	return -1;			//匹配失败，返回-1
}
//检测子弹缺陷的函数
void CbTstFaceDlg::CheckFault(IplImage* srcIn)			//没有实现的算法，因为光照和时间问题,20131219
{
	//201601231436PM注释整个函数
	//IplImage* src = cvCreateImage(cvGetSize(srcIn),IPL_DEPTH_8U,1);			//单通道8位图像

	////拷贝图像 
	//cvCopy(srcIn,src);
	//cv::Mat image(srcIn);		//从srcIn取得图像，// convert IplImage* -> Mat

	////cvConvert(src, image);

	////CvMat descriptors;
	//std::vector<cv::KeyPoint> keypoints;

	//cv::SimpleBlobDetector::Params params;

	//cv::SimpleBlobDetector blobDetector( params );
	//blobDetector.create("SimpleBlob");
	//blobDetector.detect( image, keypoints );

	//drawKeypoints(image, keypoints, image, cvScalar(255,0,0));

	//ShowMat(image,IDC_PIC3);

	//cvReleaseImage(&src);

}

//一个显示函数，传入IplImage*，和要画的空间ID，画图到相应控件
void CbTstFaceDlg::DrawIplImgToDC(IplImage* img, int ID)
{
	CWnd *pWnd = GetDlgItem(ID);
	CDC *pDC = pWnd->GetDC();
	HDC hDC = pDC->GetSafeHdc();

	// 显示图片

	m_CvvImg.CopyOf(img, -1);			//默认通道
	//m_CvvImg.m_img->imageData = img->imageData;

	m_CvvImg.DrawToHDC(hDC, disp_rect);
	m_CvvImg.Destroy();
	CPen TextPen(PS_SOLID, 2, RGB(255, 255, 255));
	pDC->SelectObject(&TextPen);					//选入画笔

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 0, 0));			//设置字体颜色

	//switch(ID)			//绘制标题
	//{
	//case IDC_PIC0:pDC->TextOut(10,10,_T("视频流"),6);break;		//
	//case IDC_PIC1:pDC->TextOut(10,10,_T("待检画面"),8);break;		//
	//case IDC_PIC2:pDC->TextOut(10,10,_T("尺寸检测"),8);break;
	//case IDC_PIC3:pDC->TextOut(10,10,_T("斑点检测"),8);break;
	//}

	switch (ID)			//绘制标题，20131219修改
	{
	case IDC_PIC0:pDC->TextOut(10, 10, _T("视频流"), 6); break;		//
	case IDC_PIC1:pDC->TextOut(10, 10, _T("均衡处理图"), 10); break;		//
	case IDC_PIC2:pDC->TextOut(10, 10, _T("滤波灰度图"), 10); break;
	case IDC_PIC3:pDC->TextOut(10, 10, _T("尺寸检测"), 8); break;
	}

	TextPen.DeleteObject();
	DeleteDC(hDC);
	ReleaseDC(pDC);
}

//显示Mat到控件上
int CbTstFaceDlg::ShowMat(cv::Mat img, int ID)
{
	CWnd *pWnd = GetDlgItem(ID);
	CDC *pDC = pWnd->GetDC();
	HWND hWndDisplay = pWnd->GetSafeHwnd(); //GetSafeHwnd(pWnd);
	if (img.channels() < 3)
	{
		return -1;
	}

	RECT rect;
	pWnd->GetClientRect(&rect);
	cv::Mat imgShow(abs(rect.top - rect.bottom), abs(rect.right - rect.left), CV_8UC3);
	resize(img, imgShow, imgShow.size());

	ATL::CImage CI;
	int w = imgShow.cols;//宽
	int h = imgShow.rows;//高
	int channels = imgShow.channels();//通道数

	CI.Create(w, h, 8 * channels);
	uchar *pS;
	uchar *pImg = (uchar *)CI.GetBits();//得到CImage数据区地址
	int step = CI.GetPitch();
	for (int i = 0; i < h; i++)
	{
		pS = imgShow.ptr<uchar>(i);
		for (int j = 0; j < w; j++)
		{
			for (int k = 0; k < 3; k++)
				*(pImg + i*step + j * 3 + k) = pS[j * 3 + k];
			//注意到这里的step不用乘以3
		}
	}

	HDC dc = ::GetDC(hWndDisplay);
	CI.Draw(dc, 0, 0);
	if (ID == IDC_PIC3)
	{
		CPen TextPen(PS_SOLID, 2, RGB(255, 255, 255));
		pDC->SelectObject(&TextPen);					//选入画笔

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 0, 0));			//设置字体颜色

		pDC->TextOut(10, 10, _T("斑点检测"), 4);		//输出提示信息
	}
	::ReleaseDC(hWndDisplay, dc);
	CI.Destroy();

	return 0;
}

//前景与背景分离,IplImage* pFrImg = src - preBkImg
IplImage* CbTstFaceDlg::DetchBkAndForground(IplImage* src, IplImage* preBkImg)
{
	//载入两张IplImage
	//转为Mat
	//用cvAbsDiff(src,pre,dst),得到前景

	IplImage* pFrame = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);			//待减去图片
	IplImage* pFrImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);			//保存前景图片
	IplImage* pBkImg = cvCreateImage(cvGetSize(preBkImg), IPL_DEPTH_8U, 3);			//cvLoadImage("..\\BkImg.jpg");		//减算子图片

	CvMat* pFrameMat = NULL;
	CvMat* pFrMat = NULL;
	CvMat* pBkMat = NULL;

	cvCopy(src, pFrame);				//拷贝图像
	cvCopy(preBkImg, pBkImg);

	cvCvtColor(pFrame, pFrame, CV_BGR2GRAY);		//转化成单通道图像再处理
	cvCvtColor(pBkImg, pBkImg, CV_BGR2GRAY);

	cvConvert(pFrame, pFrameMat);			//Mat与IplImage的互相转换,src,dst
	cvConvert(pBkImg, pBkMat);			//背景MAT

	cvAbsDiff(pFrameMat, pBkMat, pFrMat);  //当前帧跟背景图相减

	cvThreshold(pFrMat, pFrImg, 60, 255.0, CV_THRESH_BINARY);		//二值化前景图  

	cvRunningAvg(pFrame, preBkImg, 0.003, 0);			//更新背景

	cvConvert(pBkMat, pBkImg);			//将背景转化为图像格式，用以显示
	//cvConvert(pFrMat, preBkImg);			//将前景转化为图像格式，直接修改到头变量

	cvReleaseImage(&pFrame);
	//cvReleaseImage(&pFrImg);			//要返回图像，应该不能释放
	cvReleaseImage(&pBkImg);
	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);

	return pFrImg;
}

//函数功能:获取输入图像 的轮廓
int CbTstFaceDlg::getImageContour(IplImage* srcIn, CvMemStorage* mem, CvSeq** seq)
{
	IplImage* src = cvCreateImage(cvGetSize(srcIn), 8, 1);

	//拷贝图像 
	cvCopy(srcIn, src);

	//创建空间->>改为从外部获取空间，防止内存泄露
	//CvMemStorage* mem = cvCreateMemStorage(0);
	//CvSeq* seq;

	if (!mem)
	{
		printf("mem is null,IN FUNC:getImageContour()");
	}
	cvSmooth(src, src, CV_GAUSSIAN, 3, 0, 0, 0);

	//二值化图像
	cvThreshold(src, src, 100, 255, 0);

	//计算图像轮廓
	cvFindContours(src, mem, seq, sizeof(CvContour), CV_RETR_CCOMP);

	//释放图像空间
	cvReleaseImage(&src);

	//返回轮廓信息
	return 0;

}


//基本全局阈值,输入:灰度图像,输出:阈值
int  CbTstFaceDlg::BasicGlobalThreshold(IplImage* gray)
{
	int  pg[256], i, t = 0, t1, t2, k1, k2;
	double u = 0, u1, u2;

	if (gray->nChannels != 1)
	{
		return 40;
	}

	for (i = 0; i < 256; i++) pg[i] = 0;
	for (i = 0; i < gray->imageSize; i++)      //  直方图统计
		pg[(BYTE)gray->imageData[i]]++;

	//thre=BasicGlobalThreshold(pg,0,256);    //  确定阈值

	for (i = 0; i < 256; i++)
	{
		t += pg[i];		//灰度值累加
		u += i*pg[i];		//灰度值*i累加
	}

	k2 = (int)(u / t);                          //  计算此范围灰度的平均值

	do {
		k1 = k2;
		t1 = 0;     u1 = 0;
		for (i = 0; i <= k1; i++) {             //  计算低灰度组的累加和
			t1 += pg[i];
			u1 += i*pg[i];
		}
		t2 = t - t1;
		u2 = u - u1;
		if (t1) u1 = u1 / t1;                     //  计算低灰度组的平均值
		else u1 = 0;
		if (t2) u2 = u2 / t2;                     //  计算高灰度组的平均值
		else u2 = 0;
		k2 = (int)((u1 + u2) / 2);                 //  得到新的阈值估计值
	} while (k1 != k2);                           //  数据未稳定，继续

	return k1;
}






//彩色图像直方图均衡化
int CbTstFaceDlg::EqualizeHistColorImage(IplImage *pImage, IplImage* dst)
{
	IplImage *pEquaImage = cvCreateImage(cvGetSize(pImage), pImage->depth, 3);

	// 原图像分成各通道后再均衡化,最后合并即彩色图像的直方图均衡化
	const int MAX_CHANNEL = 4;
	IplImage *pImageChannel[MAX_CHANNEL] = { NULL };

	int i;
	for (i = 0; i < pImage->nChannels; i++)
		pImageChannel[i] = cvCreateImage(cvGetSize(pImage), pImage->depth, 1);

	cvSplit(pImage, pImageChannel[0], pImageChannel[1], pImageChannel[2], pImageChannel[3]);  //将多通道图像分为几个单通道图像

	for (i = 0; i < pImage->nChannels; i++)
		cvEqualizeHist(pImageChannel[i], pImageChannel[i]);

	cvMerge(pImageChannel[0], pImageChannel[1], pImageChannel[2], pImageChannel[3], pEquaImage);	//将几个单通道的图像合并为一个多通道的图像

	for (i = 0; i < pImage->nChannels; i++)
		cvReleaseImage(&pImageChannel[i]);

	cvCopy(pEquaImage, dst);

	cvReleaseImage(&pEquaImage);

	return 1;
}


//打开串口复选框的实现
void CbTstFaceDlg::OnBnOpenMotorChk()
{
#if 1
	CString str, lowstr, tips;
	tips.Format(_T("串口号:# COM%d \n"), com);		//要显示的提示信息
	lowstr.Format(_T(",N,8,1"));		//偶校验位，8位数据，1个停止位
	str = botlv + lowstr;		//拼成字符串"9600,n,8,1"
	//AfxMessageBox(str,MB_OK,0);		//测试用
	// TODO: 在此添加控件通知处理程序代码
	if (m_openMotor_chk.GetCheck())		//如果打开串口木有被选中
	{
		if (m_scomm.get_PortOpen())
		{
			m_scomm.put_PortOpen(FALSE);		//如果串口打开，关闭串口
		}

		m_scomm.put__CommPort(com);			//设置串口号
		m_scomm.put_PortOpen(TRUE);			//打开串口

		if (m_scomm.get_PortOpen())			//如果串口打开了，配置串口
		{
			AfxMessageBox(_T("打开串口成功\n") + tips, MB_OK, 0);
			m_scomm.put_Settings(str);

			m_scomm.put_InputMode(1);			//1：表示以二进制方式检取数据
			m_scomm.put_RThreshold(2);			//参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
			m_scomm.put_InputLen(0);			//设置当前接收区数据长度为0
			m_scomm.get_Input();			//先预读缓冲区以清除残留数据
		}
	}
	else			//如果打开串口被选中，关闭串口
	{
		if (m_scomm.get_PortOpen())
		{
			m_scomm.put_PortOpen(FALSE);		//如果串口打开，关闭串口
			if (!m_scomm.get_PortOpen())
			{
				AfxMessageBox(_T("串口已关闭\n") + tips, MB_OK, 0);
			}
		}
	}
#endif
}



//用来写转盘位置0,1,2,3到串口，函数内部实现了写速度值到串口
void CbTstFaceDlg::WriteToSerial(char* VALUE, char control_code)
{
	CByteArray arraySend;
	unsigned int size = 0;
	int i = 0;
	size = SEND_DATA_LENGTH + COM_DATA_STRUCTURE_LENGTH;
	//UINT8 h = m_sudu_slider.GetPos();
	//h = h & 0x00ff;
	//l = l & 0x00ff;
	arraySend.RemoveAll();
	arraySend.SetSize(size);
	//arraySend.SetSize(SEND_DATA_LENGTH);
	//arraySend.SetAt(0,0xff);		//增加了一个校验位，数据形式:0xff,0x00(速度 or 0x01 转盘),0xXX数值,速度是0-100，转盘是0-3
	//arraySend.SetAt(1,MODE);
	arraySend.SetAt(i++, 0xFA);        // 帧起始符
	arraySend.SetAt(i++, control_code);        // 控制码
	arraySend.SetAt(i++, SEND_DATA_LENGTH);        // 数据长度
	//for (; i < SEND_DATA_LENGTH + 3; i++)
	for (; i < SEND_DATA_LENGTH + 3; i++)
		arraySend.SetAt(i, VALUE[i - 3]);

	arraySend.SetAt(i++, 0xFB);        // 校验码
	arraySend.SetAt(i, 0x99);        // 结束符

	if (m_scomm.get_PortOpen())		//如果串口打开，就发送数据
	{
		m_scomm.put_Output(COleVariant(arraySend));
	}
}BEGIN_EVENTSINK_MAP(CbTstFaceDlg, CDialog)
ON_EVENT(CbTstFaceDlg, IDC_MSCOMM1, 1, CbTstFaceDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()
//接收串口数据，显示到提示信息窗口
void CbTstFaceDlg::OnCommMscomm1()
{
	// TODO: 在此处添加消息处理程序代码
	//VARIANT variant_inp;
	COleVariant myVar;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[32]; //设置BYTE数组 An 8-bit integerthat is not signed.
	CString strtemp;
	if (m_scomm.get_CommEvent() == 2) //事件值为2表示接收缓冲区内有字符
	{             ////////以下你可以根据自己的通信协议加入处理代码
		//AfxMessageBox(_T("接受到数据了"),MB_OK,0);
		//variant_inp=m_scomm.get_Input(); //读缓冲区
		myVar.Attach(m_scomm.get_Input());
		safearray_inp = myVar;//variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
		len = safearray_inp.GetOneDimSize(); //得到有效数据长度
		if (len > 32)
		{
			AfxMessageBox(_T("串口有效数据长度应小于32！"), MB_OK, 0);
		}
		else
		{
			for (k = 0; k < len; k++)
				safearray_inp.GetElement(&k, rxdata + k);//转换为BYTE型数组

			//接收到下位机处理完成的标志
			if ((len == 4) && (rxdata[0] == 0xfa) && (rxdata[1] == 0xf0))
			{
				//WriteToSerial(senddata, 0x02);		//注释这句，LRF201501071859PM

				if (send_data_flag == 0) {
					send_data_flag = 1;
					send_data_finish = 0;
				}
			}
			for (k = 0; k < len; k++) //将数组转换为Cstring型变量
			{
				BYTE bt = *(char*)(rxdata + k); //字符型
				strtemp.Format(_T("%02X "), bt);		//转换为16进制，将字符送入临时变量strtemp存放，每条命令是两个字节
				if (m_outText.GetLength() < 512)	//LRF201601061925PM
					m_outText += strtemp; //加入接收编辑框对应字符串
				else
					m_outText = _T("");		//清除内容
			}
		}
	}
	//else if (m_scomm.get_CommEvent() == 8002)		//无效的端口号
	//{
	//	AfxMessageBox(_T("无效的端口号"),MB_OK,0);
	//}
	UpdateData(FALSE); //更新编辑框内容
	Edit_shuchu.SetSel(-1);			//使光标定位到最末行，实现自动换行的效果
}

//设置主界面的背景颜色
HBRUSH CbTstFaceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	////CFont * cfont=new CFont;
	////cfont->CreateFont( 10,10,0,0,10,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"宋体");

	////pWnd->SetFont(cfont);
	//pDC->SetTextColor(RGB(TextColor.r,TextColor.g,TextColor.b))	;    //改变所有文本颜色RGB(248,147,29)#FECD97,这里有问题

	//if (nCtlColor == CTLCOLOR_DLG )			//改变背景色
	//{
	//	//pDC->SetBkColor(RGB(255,0,0));
	//	HBRUSH B = CreateSolidBrush(RGB(DlgColor.r,DlgColor.g,DlgColor.b)); //创建画刷RGB(174,221,129),20131221修改
	//	return (HBRUSH) B;
	//}

	//if (nCtlColor == CTLCOLOR_STATIC && (pWnd->GetDlgCtrlID() != IDC_COUNT_SUM \
	//	&& pWnd->GetDlgCtrlID() != IDC_COUNT_A \
	//	&& pWnd->GetDlgCtrlID() != IDC_COUNT_B \
	//	&& pWnd->GetDlgCtrlID() != IDC_COUNT_C \
	//	&& pWnd->GetDlgCtrlID() != IDC_COUNT_D))
	//{
	//	pDC->SetBkMode(TRANSPARENT);		//透明背景，这里有问题
	//	HBRUSH B = CreateSolidBrush(RGB(DlgColor.r,DlgColor.g,DlgColor.b)); //创建画刷RGB(174,221,129),20131221修改
	//	return (HBRUSH) B;
	//}

	//if (nCtlColor == CTLCOLOR_STATIC && (pWnd->GetDlgCtrlID() == IDC_COUNT_SUM \
	//	|| pWnd->GetDlgCtrlID() == IDC_COUNT_A \
	//	|| pWnd->GetDlgCtrlID() == IDC_COUNT_B \
	//	|| pWnd->GetDlgCtrlID() == IDC_COUNT_C \
	//	|| pWnd->GetDlgCtrlID() == IDC_COUNT_D))
	//{
	//	pDC->SetBkMode(TRANSPARENT);		//OPAQUE
	//	HBRUSH B = CreateSolidBrush(RGB(StaticColor.r,StaticColor.g,StaticColor.b)); //创建画刷,20131221修改
	//	return (HBRUSH) B; 
	//}

	////if (nCtlColor == CTLCOLOR_BTN)		//改变按钮颜色
	////{
	////	//pDC->SetBkMode(TRANSPARENT);		//透明背景
	////	HBRUSH B = CreateSolidBrush(RGB(0,0,255)); //创建画刷
	////	return (HBRUSH) B;
	////}

	return hbr;
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG)			//改变背景色
	{
		//pDC->SetBkColor(RGB(255,0,0));
		HBRUSH B = CreateSolidBrush(RGB(153, 204, 153)); //创建画刷RGB(174,221,129)
		return (HBRUSH)B;
	}

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);		//透明背景
		HBRUSH B = CreateSolidBrush(RGB(153, 204, 153));	 //创建画刷RGB(174,221,129)
		return (HBRUSH)B;
	}

	return hbr;
}

//清零计数
void CbTstFaceDlg::OnBnClickedCountclsBtn()
{
	type_sum = 0;
	type_count[0] = 0;
	type_count[1] = 0;
	type_count[2] = 0;
	type_count[3] = 0;
	m_type_sum.Format(_T("%4d"), type_sum);
	m_typeA_value.Format(_T("%4d"), type_count[0]);
	m_typeB_value.Format(_T("%4d"), type_count[1]);
	m_typeC_value.Format(_T("%4d"), type_count[2]);
	m_typeD_value.Format(_T("%4d"), type_count[3]);
	UpdateData(FALSE);
}


//保存尺寸数据的实现
void CbTstFaceDlg::OnStnClickedCountA()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTempBH, strTempBW;		//以前的尺寸
	CString strTempAH, strTempAW;		//现在的尺寸
	strTempAH.Format(_T("%0.1lf"), object_height[0]);
	strTempAW.Format(_T("%0.1lf"), object_width[0]);
	strTempBH.Format(_T("%0.1lf"), heightSize[0]);
	strTempBW.Format(_T("%0.1lf"), widthSize[0]);
	if (IDYES == AfxMessageBox(_T("要用当前尺寸") + strTempAH + '-' + strTempAW + _T("代替之前尺寸") + strTempBH + '-' + strTempBW + _T("到队列 0 吗？"), MB_YESNO, 0))
	{
		CString KeyValue;
		BOOL writeOk = FALSE;

		heightSize[0] = object_height[0];			//建立尺寸常量表
		widthSize[0] = object_width[0];
		type_count[0]++;

		KeyValue.Format(_T("%d"), (int)(10 * heightSize[0]));
		writeOk = ::WritePrivateProfileString("DataSetting", "bulletHeiget0", KeyValue, ".\\setting.ini");		//写入面积最小值
		KeyValue.Format(_T("%d"), (int)(10 * widthSize[0]));
		writeOk = writeOk && ::WritePrivateProfileString("DataSetting", "bulletWidth0", KeyValue, ".\\setting.ini");		//写入面积最小值
		if (writeOk)
			AfxMessageBox(_T("类型 0 尺寸已经保存"), MB_OK, 0);
	}
}

void CbTstFaceDlg::OnStnClickedCountB()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTempBH, strTempBW;		//以前的尺寸
	CString strTempAH, strTempAW;		//现在的尺寸
	strTempAH.Format(_T("%0.1lf"), object_height[0]);
	strTempAW.Format(_T("%0.1lf"), object_width[0]);
	strTempBH.Format(_T("%0.1lf"), heightSize[1]);
	strTempBW.Format(_T("%0.1lf"), widthSize[1]);
	if (IDYES == AfxMessageBox(_T("要用当前尺寸") + strTempAH + '-' + strTempAW + _T("代替之前尺寸") + strTempBH + '-' + strTempBW + _T("到队列 1 吗？"), MB_YESNO, 0))
	{
		CString KeyValue;
		BOOL writeOk = FALSE;

		heightSize[1] = object_height[0];			//建立尺寸常量表
		widthSize[1] = object_width[0];
		type_count[1]++;

		KeyValue.Format(_T("%d"), (int)(10 * heightSize[1]));
		writeOk = ::WritePrivateProfileString("DataSetting", "bulletHeiget1", KeyValue, ".\\setting.ini");		//写入面积最小值
		KeyValue.Format(_T("%d"), (int)(10 * widthSize[1]));
		writeOk = writeOk && ::WritePrivateProfileString("DataSetting", "bulletWidth1", KeyValue, ".\\setting.ini");		//写入面积最小值
		if (writeOk)
			AfxMessageBox(_T("类型 1 尺寸已经保存"), MB_OK, 0);
	}
}

void CbTstFaceDlg::OnStnClickedCountC()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTempBH, strTempBW;		//以前的尺寸
	CString strTempAH, strTempAW;		//现在的尺寸
	strTempAH.Format(_T("%0.1lf"), object_height[0]);
	strTempAW.Format(_T("%0.1lf"), object_width[0]);
	strTempBH.Format(_T("%0.1lf"), heightSize[2]);
	strTempBW.Format(_T("%0.1lf"), widthSize[2]);
	if (IDYES == AfxMessageBox(_T("要用当前尺寸") + strTempAH + '-' + strTempAW + _T("代替之前尺寸") + strTempBH + '-' + strTempBW + _T("到队列 2 吗？"), MB_YESNO, 0))
	{
		CString KeyValue;
		BOOL writeOk = FALSE;

		heightSize[2] = object_height[0];			//建立尺寸常量表
		widthSize[2] = object_width[0];
		type_count[2]++;

		KeyValue.Format(_T("%d"), (int)(10 * heightSize[2]));
		writeOk = ::WritePrivateProfileString("DataSetting", "bulletHeiget2", KeyValue, ".\\setting.ini");		//写入面积最小值
		KeyValue.Format(_T("%d"), (int)(10 * widthSize[2]));
		writeOk = writeOk && ::WritePrivateProfileString("DataSetting", "bulletWidth2", KeyValue, ".\\setting.ini");		//写入面积最小值
		if (writeOk)
			AfxMessageBox(_T("类型 2 尺寸已经保存"), MB_OK, 0);
	}
}

void CbTstFaceDlg::OnStnClickedCountD()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strTempBH, strTempBW;		//以前的尺寸
	CString strTempAH, strTempAW;		//现在的尺寸
	strTempAH.Format(_T("%0.1lf"), object_height[0]);
	strTempAW.Format(_T("%0.1lf"), object_width[0]);
	strTempBH.Format(_T("%0.1lf"), heightSize[3]);
	strTempBW.Format(_T("%0.1lf"), widthSize[3]);
	if (IDYES == AfxMessageBox(_T("要用当前尺寸") + strTempAH + '-' + strTempAW + _T("代替之前尺寸") + strTempBH + '-' + strTempBW + _T("到队列 3 吗？"), MB_YESNO, 0))
	{
		CString KeyValue;
		BOOL writeOk = FALSE;

		heightSize[3] = object_height[0];			//建立尺寸常量表
		widthSize[3] = object_width[0];
		type_count[3]++;

		KeyValue.Format(_T("%d"), (int)(10 * heightSize[3]));
		writeOk = ::WritePrivateProfileString("DataSetting", "bulletHeiget3", KeyValue, ".\\setting.ini");		//写入面积最小值
		KeyValue.Format(_T("%d"), (int)(10 * widthSize[3]));
		writeOk = writeOk && ::WritePrivateProfileString("DataSetting", "bulletWidth3", KeyValue, ".\\setting.ini");		//写入面积最小值
		if (writeOk)
			AfxMessageBox(_T("类型 3 尺寸已经保存"), MB_OK, 0);
	}
}


//菜单消息响应的函数
BOOL CbTstFaceDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::TranslateAccelerator(GetSafeHwnd(), hAccel, pMsg))
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

int CbTstFaceDlg::CheckColor(IplImage* srcIn, int cnt)
{
	int type = 1;
	CvScalar s;
	int r = 10;
	s = cvGet2D(srcIn, center_y[cnt] + r, center_x[cnt] + r);
	double x = s.val[0];
	double y = s.val[1];
	double z = s.val[2];
	return type;
}