// bTstFaceDlg.h : ͷ�ļ�
//

#pragma once
#include "opencv/cxcore.h"			//��Ϊ�õ���IplImage
#include "afxcmn.h"
#include "easysize.h"
#include "afxwin.h"
#include "mscomm1.h"

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;


//#include <opencv2/opencv.hpp>
//#include <opencv2/nonfree/nonfree.hpp>
//#include <opencv2/nonfree/features2d.hpp>

#define MAX_NUM 10 // ��֡ͼ�����ܼ�⵽�������������
#define DETECT_TYPE_NUM 10 // ��ֵΪ����������͵ĸ���
#define SEND_DATA_LENGTH 5 // ��Ч������Ϣ�ĳ��ȣ�1byte type 4byte height
#define COM_DATA_STRUCTURE_LENGTH 5 // ��ֵΪ��������ͷ������β֮��: ����������ʼλУ��λ����λ�ȵȣ�
//#define REJUST_ZERO_OFFSET 9 // ��ֵΪ����ͷ����ϵ��ʵ������ϵ��ƫ�ƣ�
#define ADD_DETECT_COLOR 0 // ���Ϊ0���������в�ʹ����ɫ��Ϣ����ͼ���ֵ�������Ϊ1����������ʹ����ɫ��Ϣ����ͼ���ֵ����
#define DETECT_BOTTLE 0  //  ���Ϊ1�����͸���Թܣ����Ϊ0�����Ӳ�һ��β�͸������

// CbTstFaceDlg �Ի���
class CbTstFaceDlg : public CDialog
{
	DECLARE_EASYSIZE
// ����
public:
	CbTstFaceDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CbTstFaceDlg();		//������������
	//CAboutDlg *dlg;

// �Ի�������
	enum { IDD = IDD_BTSTFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL DestroyWindow();		//��������֧��

private:
	CMenu m_menu;
	bool statc;					//�����������ͷ�Ŀ��ر���
    int flag;						//�ж�ͼ���Ƿ����
    int canPlus;
    int enterTimercount;			//���붨ʱ���Ĵ���
	CRect disp_rect;					//��ͼ�����С
	CvCapture*	m_video;			//��ƵCapture
    IplImage*	preBkImg;			//cvLoadImage("..\\preBk.jpg",1);			//����ͼ��
    IplImage*	contourImg;			//���������������ͼ��
    IplImage*	backupImg;			//���˱�������й����ȴ����ͼƬ�������ڰߵ���
    IplImage*	willImg;			//�õ����ӵ�������ͼƬ
    //CvBox2D rect;						//�����洢���ӵ�����BOX

	int type_sum;					//�ӵ�������
    int type_count[10];					//�����ʮ�����壬��ʱA,B,C,D���ӵ�����
	int send_data_flag;
	int send_data_finish;

    double object_height[MAX_NUM];		//�ӵ�ʵ�ʵĳ��Ϳ�(���ص�λ)
    double object_width[MAX_NUM];

    double heightSize[10];			//�����ߴ糣����
    double widthSize[10];
    int center_x[MAX_NUM];
    int center_y[MAX_NUM];
	int header[MAX_NUM];
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnaboutClick();
	afx_msg void OnLinkcam();
	afx_msg void OnUpdateLinkcam(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMenuExit();
	afx_msg void FindContourBoxAndWillImg(IplImage* srcIn, CvMemStorage* contour_mem, CvSeq** contour);
	afx_msg void FindContourBoxAndHeader(IplImage* srcIn, CvMemStorage* contour_mem, CvSeq** contour);
	afx_msg int CheckColor(IplImage* SrcIn, int cnt);
	afx_msg void DrawIplImgToDC(IplImage* img,int ID);
	afx_msg IplImage* DetchBkAndForground(IplImage* src, IplImage* preBkImg);
	afx_msg void OnNMReleasedcaptureLightSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureThreSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureContrastSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int getImageContour(IplImage* srcIn, CvMemStorage* mem, CvSeq** seq);
	afx_msg void CheckFault(IplImage* srcIn);
	afx_msg int MatchTypeAcorCon(CvSeq** contour);
	afx_msg int MatchTypeAcorSize(double pricision, double height, double width);
	afx_msg char* CString2char(CString &str);
	afx_msg int ImageAdjust(IplImage* src, IplImage* dst, double low, double high,double bottom, double top,double gamma );
	afx_msg int BasicGlobalThreshold(IplImage* gray);
	afx_msg int EqualizeHistColorImage(IplImage *pImage, IplImage* pEquaImage);
	afx_msg void WriteToSerial(char *VALUE, char control_code);
	afx_msg int ShowMat( cv::Mat img, int ID);
	CSliderCtrl m_slider_light;
	CString m_light_value;
	CString m_thres_value;
	CSliderCtrl m_slider_thres;
	CString m_type_text;
	CString m_type_sum;
	// �ӵ��ߴ�
	CString m_bullet_size;
	CStatic m_bulletSize_ctl;
	CStatic m_bulletType_ctl;
	CString m_typeA_value;
	CString m_typeB_value;
	CString m_typeC_value;
	CString m_typeD_value;
	CSliderCtrl m_contrast_ctl;
	CString m_constrat_value;

	char senddata[SEND_DATA_LENGTH];
	
	CButton m_glabolThres_check;
	CButton m_lightAuto_ctl;
	afx_msg void OnHelp();
	//afx_msg void OnNMReleasedcaptureErodeSlider(NMHDR *pNMHDR, LRESULT *pResult);
	//CSliderCtrl m_erode_ctl;
	//CString m_erode_value;
	afx_msg void OnAdjust();
	afx_msg void OnSetting();
	CButton m_openMotor_chk;
	afx_msg void OnBnOpenMotorChk();
	// �ؼ�����
	CMscomm1 m_scomm;
	CSliderCtrl m_sudu_slider;
	CString m_sudu_value;
	afx_msg void OnNMReleasedcaptureSuduSlider(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	CString m_outText;
	CEdit Edit_shuchu;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CFont m_editFont;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedCountclsBtn();
	afx_msg void OnStnClickedCountA();
	afx_msg void OnStnClickedCountB();
	afx_msg void OnStnClickedCountC();
	afx_msg void OnStnClickedCountD();
	HACCEL hAccel;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
