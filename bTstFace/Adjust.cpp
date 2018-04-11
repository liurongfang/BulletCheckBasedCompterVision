// Adjust.cpp : ʵ���ļ�
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

extern IplImage* srcImg;			//��������ȡ��ͼƬ
extern double dipScale;				//�������dipScale����
IplImage* adjustImg = 0;
CvvImage m_cvvImg;
CRect rect;


// CAdjust �Ի���

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
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease�����ཫ�Զ�
	// ɾ���ö����ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

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

// ע��: ������� IID_IAdjust ֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡��� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {412CA846-72ED-481C-A44D-7286FE741E6B}
static const IID IID_IAdjust =
{ 0x412CA846, 0x72ED, 0x481C, { 0xA4, 0x4D, 0x72, 0x86, 0xFE, 0x74, 0x1E, 0x6B } };

BEGIN_INTERFACE_MAP(CAdjust, CDialog)
	INTERFACE_PART(CAdjust, IID_IAdjust, Dispatch)
END_INTERFACE_MAP()


// CAdjust ��Ϣ�������
BOOL CAdjust::OnInitDialog()
{
	SetWindowText(_T("����ͷ�궨"));
	((CButton*)GetDlgItem(IDOK))->SetWindowText(_T("ȷ��"));
	((CStatic*)GetDlgItem(IDCANCEL))->SetWindowText(_T("ȡ��"));
	((CStatic*)GetDlgItem(IDC_ADJUST_BTN))->SetWindowText(_T("��ʼ�궨"));

	//adjustImg = GetDlgItem(IDD_BTSTFACE_DIALOG)->
	//cvCopy(srcImg, adjustImg);			//copy���������

	CWnd* pWnd = GetDlgItem(IDC_PIC);			//��һ������������,ԭ��IDд��IDC_PIC0��
	pWnd->GetClientRect(&rect);

	if (srcImg != NULL)
	{
		adjustImg = cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,3);
	}
	else
	{
		AfxMessageBox(_T("���ȴ�����ͷ�����úñ궨�塣���ȷ���˳�"),MB_OK,0);
		CDialog::OnCancel();		//�رնԻ���
	}

	SetWindowText(_T("�궨����"));
	((CStatic*)GetDlgItem(IDC_TEX1))->SetWindowText(_T("������(����/ʵ�ʾ���CM,double��)��"));
	((CStatic*)GetDlgItem(IDC_TEX2))->SetWindowText(_T("�궨�����"));
	((CStatic*)GetDlgItem(IDC_ADJUST_BTN))->SetWindowText(_T("��ʼ�궨"));
	((CStatic*)GetDlgItem(IDOK))->SetWindowText(_T("ȷ��"));
	((CStatic*)GetDlgItem(IDCANCEL))->SetWindowText(_T("ȡ��"));
	
	//CDC *pDC = pWnd->GetDC();
	//HDC hDC= pDC->GetSafeHdc();

	//// ��ʾͼƬ
	//m_cvvImg.CopyOf(srcImg,-1);			//Ĭ��ͨ��
	//m_cvvImg.DrawToHDC(hDC,rect);

	//UpdateData(FALSE);

	return TRUE;
}
void CAdjust::OnBnClickedAdjustBtn()
{
	DrawIplImgToDC(srcImg,IDC_PIC);			//�Ƚ����궨��ͼƬ��ʾ����
	if (srcImg)
	{
		AfxMessageBox(_T("srcImg is not NULL!"),MB_OK,0);
					//�����õ�

		double d[4] = {0};			//��������Բ��֮��ľ���
		double x[4] = {0};
		double y[4] = {0};
		//IplImage* adjustImg = cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,3);

		cvCopy(srcImg,adjustImg);

		//���������ͼ��궨�Ĵ���

		IplImage* gray = cvCreateImage(cvGetSize(adjustImg),IPL_DEPTH_8U,1);
		cvCvtColor(adjustImg,gray,CV_BGR2GRAY);		//ת��Ϊ�Ҷ�ͼ
		cvSmooth(gray,gray,CV_GAUSSIAN,3,0,0,0);	//�˲�

		////ͨ������Բ�����궨
		//CvMemStorage* circles = cvCreateMemStorage(0);
		//CvSeq* seq;

		//// Ӧ�û���任��Բ
		//cvHoughCircles( gray, seq, CV_HOUGH_GRADIENT, 1, 1/8, 200, 100, 0, 0 );

		/*vector<Vec3f> circles;

		/// Ӧ�û���任��Բ
		cvHoughCircles( gray, circles, CV_HOUGH_GRADIENT, 1, 40, 200, 100, 0, 0 );*/

		CvMemStorage* storage = cvCreateMemStorage (0);

		CvSeq* circles = cvHoughCircles (gray, storage, CV_HOUGH_GRADIENT, 2, gray->width /8, 300, 100, 0, 200);

		/////��Բ
		//for( size_t i = 0; i < circles.size(); i++)
		//{
			//d[i] = sqrt(��circles[i][0] - circles[(i+1)%3][0])*��circles[i][0] - circles[(i+1)%3][0]) \
			//	+ (circles[i][1] - circles[(i+1)%3][1])*(circles[i][1] - circles[(i+1)%3][1]));		//���������ľ��� d = sqrt((x0-x1)^2 + (y0-y1)^2)
			//Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			//int radius = cvRound(circles[i][2]);
		//	//����Բ��
		//	circle( adjustImg, center, 2, Scalar(0,255,0), -1, 8, 0 );
		//	//����Բ��
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

		d[3] = ( d[0] + d[1] + d[2])/3;			//���ؾ���

		UpdateData(TRUE);
		dipScale = d[3]/distance;			//����õ��ı궨�ȣ�ʵ�ʾ���/���ؾ��룬ʹ�� ���ؾ���/dipScale

		DrawIplImgToDC(gray,IDC_PIC);
		
		m_showMsg.Format(_T("�궨��ɣ�\n����õ��ı궨��Ϊ:%0.1lf\n"),dipScale);
		UpdateData(FALSE);

	}
	else
	{
		AfxMessageBox(_T("srcImg is NULL!"),MB_OK,0);
		CDialog::OnOK();					//���������֮��͹ر��˶Ի���
	}

	
}

void CAdjust::DrawIplImgToDC(IplImage* img,int ID)
{
	CWnd *pWnd = GetDlgItem(ID);
	CDC *pDC = pWnd->GetDC();
	HDC hDC= pDC->GetSafeHdc();

	// ��ʾͼƬ
	m_cvvImg.CopyOf(img,-1);			//Ĭ��ͨ��
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
