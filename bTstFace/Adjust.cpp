// Adjust.cpp : 实现文件
//

#include "stdafx.h"
#include "bTstFace.h"
#include "Adjust.h"
#include "CvvImage.h"
#include "opencv/cv.h"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "math.h"
//using namespace cv;
//using namespace std;

extern IplImage* srcImg;			//从主窗体取得图片
extern double dipScale;				//主窗体的dipScale变量
IplImage* adjustImg = 0;
CvvImage m_cvvImg;
CRect rect;


// CAdjust 对话框

IMPLEMENT_DYNAMIC(CAdjust, CDialog)

CAdjust::CAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjust::IDD, pParent)
	,distance(0)
	, m_showMsg(_T(""))
	, m_slect_value(_T(""))
{

	EnableAutomation();

}

CAdjust::~CAdjust()
{
}

void CAdjust::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CDialog::OnFinalRelease();
}

void CAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, distance);
	DDX_Text(pDX, IDC_EDIT1, m_showMsg);
	DDX_Text(pDX, IDC_SELECT_VALUE, m_slect_value);
	DDX_Control(pDX, IDC_SELECT_SLIDER, m_select_slider);
}


BEGIN_MESSAGE_MAP(CAdjust, CDialog)
	ON_BN_CLICKED(IDC_ADJUST_BTN, &CAdjust::OnBnClickedAdjustBtn)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SELECT_SLIDER, &CAdjust::OnNMReleasedcaptureSelectSlider)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAdjust, CDialog)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IAdjust 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {412CA846-72ED-481C-A44D-7286FE741E6B}
static const IID IID_IAdjust =
{ 0x412CA846, 0x72ED, 0x481C, { 0xA4, 0x4D, 0x72, 0x86, 0xFE, 0x74, 0x1E, 0x6B } };

BEGIN_INTERFACE_MAP(CAdjust, CDialog)
	INTERFACE_PART(CAdjust, IID_IAdjust, Dispatch)
END_INTERFACE_MAP()


// CAdjust 消息处理程序
BOOL CAdjust::OnInitDialog()
{
	SetWindowText(_T("摄像头标定"));
	((CButton*)GetDlgItem(IDOK))->SetWindowText(_T("确定"));
	((CStatic*)GetDlgItem(IDCANCEL))->SetWindowText(_T("取消"));
	((CStatic*)GetDlgItem(IDC_ADJUST_BTN))->SetWindowText(_T("开始标定"));

	//adjustImg = GetDlgItem(IDD_BTSTFACE_DIALOG)->
	//cvCopy(srcImg, adjustImg);			//copy这局有问题

	CWnd* pWnd = GetDlgItem(IDC_PIC);			//这一块代码出现问题,原来ID写成IDC_PIC0了
	pWnd->GetClientRect(&rect);

	if (srcImg != NULL)
	{
		adjustImg = cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,3);
	}
	else
	{
		AfxMessageBox(_T("请先打开摄像头，放置好标定板。点击确定退出"),MB_OK,0);
		CDialog::OnCancel();		//关闭对话框
	}

	SetWindowText(_T("标定程序"));
	((CStatic*)GetDlgItem(IDC_TEX1))->SetWindowText(_T("比例尺(像素/实际距离CM,double型)："));
	((CStatic*)GetDlgItem(IDC_TEX2))->SetWindowText(_T("标定结果："));
	((CStatic*)GetDlgItem(IDC_ADJUST_BTN))->SetWindowText(_T("开始标定"));
	((CStatic*)GetDlgItem(IDOK))->SetWindowText(_T("确定"));
	((CStatic*)GetDlgItem(IDCANCEL))->SetWindowText(_T("取消"));
	
	//CDC *pDC = pWnd->GetDC();
	//HDC hDC= pDC->GetSafeHdc();

	//// 显示图片
	//m_cvvImg.CopyOf(srcImg,-1);			//默认通道
	//m_cvvImg.DrawToHDC(hDC,rect);

	//UpdateData(FALSE);

	return TRUE;
}
void CAdjust::OnBnClickedAdjustBtn()
{
	DrawIplImgToDC(srcImg,IDC_PIC);			//先将待标定的图片显示出来
	if (srcImg)
	{
		AfxMessageBox(_T("srcImg is not NULL!"),MB_OK,0);
					//测试用的

		double d[4] = {0};			//用来保存圆心之间的距离
		double x[4] = {0};
		double y[4] = {0};
		//IplImage* adjustImg = cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,3);

		cvCopy(srcImg,adjustImg);

		//在这里添加图像标定的处理

		IplImage* gray = cvCreateImage(cvGetSize(adjustImg),IPL_DEPTH_8U,1);
		cvCvtColor(adjustImg,gray,CV_BGR2GRAY);		//转换为灰度图
		cvSmooth(gray,gray,CV_GAUSSIAN,3,0,0,0);	//滤波

		////通过查找圆，来标定
		//CvMemStorage* circles = cvCreateMemStorage(0);
		//CvSeq* seq;

		//// 应用霍夫变换找圆
		//cvHoughCircles( gray, seq, CV_HOUGH_GRADIENT, 1, 1/8, 200, 100, 0, 0 );

		/*vector<Vec3f> circles;

		/// 应用霍夫变换找圆
		cvHoughCircles( gray, circles, CV_HOUGH_GRADIENT, 1, 40, 200, 100, 0, 0 );*/

		CvMemStorage* storage = cvCreateMemStorage (0);

		CvSeq* circles = cvHoughCircles (gray, storage, CV_HOUGH_GRADIENT, 2, gray->width /8, 300, 100, 0, 200);

		/////画圆
		//for( size_t i = 0; i < circles.size(); i++)
		//{
			//d[i] = sqrt(（circles[i][0] - circles[(i+1)%3][0])*（circles[i][0] - circles[(i+1)%3][0]) \
			//	+ (circles[i][1] - circles[(i+1)%3][1])*(circles[i][1] - circles[(i+1)%3][1]));		//计算两点间的距离 d = sqrt((x0-x1)^2 + (y0-y1)^2)
			//Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			//int radius = cvRound(circles[i][2]);
		//	//画出圆心
		//	circle( adjustImg, center, 2, Scalar(0,255,0), -1, 8, 0 );
		//	//画出圆周
		//	circle( adjustImg, center, radius, Scalar(0,0,255), 3, 8, 0 );
		//}

		for (int i = 0; i < circles->total; i++)
		{  
			float* p = (float*)cvGetSeqElem (circles, i);
			x[i] = p[0];
			y[i] = p[1];
			CvPoint pt = cvPoint (cvRound(p[0]), cvRound(p[1]));

			cvCircle (gray, pt, 2, CV_RGB(255, 0, 0), 3, 8, 0);
			cvCircle (gray, pt, cvRound(p[2]), CV_RGB(255, 0, 0), 3, 8, 0);
		}

		d[0] = sqrt( (x[0] - x[1]) * (x[0] - x[1]) + ( y[0] - y[1] )*( y[0] * y[1] ));
		d[1] = sqrt( (x[1] - x[2]) * (x[1] - x[2]) + ( y[1] - y[2] )*( y[1] - y[2] ));
		d[2] = sqrt( (x[2] - x[0]) * (x[2] - x[0]) + ( y[2] - y[0] )*( y[2] - y[0] ));

		d[3] = ( d[0] + d[1] + d[2])/3;			//像素距离

		UpdateData(TRUE);
		dipScale = d[3]/distance;			//计算得到的标定比，实际距离/像素距离，使用 像素距离/dipScale

		DrawIplImgToDC(gray,IDC_PIC);
		
		m_showMsg.Format(_T("标定完成！\n计算得到的标定比为:%0.1lf\n"),dipScale);
		UpdateData(FALSE);

	}
	else
	{
		AfxMessageBox(_T("srcImg is NULL!"),MB_OK,0);
		CDialog::OnOK();					//加这个按完之后就关闭了对话框
	}

	
}

void CAdjust::DrawIplImgToDC(IplImage* img,int ID)
{
	CWnd *pWnd = GetDlgItem(ID);
	CDC *pDC = pWnd->GetDC();
	HDC hDC= pDC->GetSafeHdc();

	// 显示图片
	m_cvvImg.CopyOf(img,-1);			//默认通道
	m_cvvImg.DrawToHDC(hDC,rect);

	ReleaseDC(pDC);
}
void CAdjust::OnNMReleasedcaptureSelectSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

HBRUSH CAdjust::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
