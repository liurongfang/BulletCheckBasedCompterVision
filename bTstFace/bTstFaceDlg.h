// bTstFaceDlg.h : 头文件
//

#pragma once
#include "opencv/cxcore.h"			//因为用到了IplImage
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

#define MAX_NUM 10 // 单帧图像内能检测到的物体的最大个数
#define DETECT_TYPE_NUM 10 // 此值为检测物体类型的个数
#define SEND_DATA_LENGTH 5 // 有效数据信息的长度；1byte type 4byte height
#define COM_DATA_STRUCTURE_LENGTH 5 // 此值为串口数据头和数据尾之和: 包括数据起始位校验位结束位等等；
//#define REJUST_ZERO_OFFSET 9 // 此值为摄像头坐标系到实际坐标系的偏移；
#define ADD_DETECT_COLOR 0 // 如果为0，检测过程中不使用颜色信息辅助图像二值化，如果为1，检测过程中使用颜色信息辅助图像二值化；
#define DETECT_BOTTLE 0  //  如果为1，检测透明试管；如果为0，检测硬币或方形不透明物体

// CbTstFaceDlg 对话框
class CbTstFaceDlg : public CDialog
{
	DECLARE_EASYSIZE
// 构造
public:
	CbTstFaceDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CbTstFaceDlg();		//声明析构函数
	//CAboutDlg *dlg;

// 对话框数据
	enum { IDD = IDD_BTSTFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL DestroyWindow();		//窗体销毁支持

private:
	CMenu m_menu;
	bool statc;					//点击连接摄像头的开关变量
    int flag;						//判断图像是否更新
    int canPlus;
    int enterTimercount;			//进入定时器的次数
	CRect disp_rect;					//绘图区域大小
	CvCapture*	m_video;			//视频Capture
    IplImage*	preBkImg;			//cvLoadImage("..\\preBk.jpg",1);			//背景图像
    IplImage*	contourImg;			//用来画最大轮廓的图像
    IplImage*	backupImg;			//用了保存仅进行过亮度处理的图片，以用于斑点检测
    IplImage*	willImg;			//得到的子弹待处理图片
    //CvBox2D rect;						//用来存储“子弹”的BOX

	int type_sum;					//子弹的总数
    int type_count[10];					//最多检测十种物体，暂时A,B,C,D型子弹计数
	int send_data_flag;
	int send_data_finish;

    double object_height[MAX_NUM];		//子弹实际的长和宽(像素单位)
    double object_width[MAX_NUM];

    double heightSize[10];			//建立尺寸常量表
    double widthSize[10];
    int center_x[MAX_NUM];
    int center_y[MAX_NUM];
	int header[MAX_NUM];
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	// 子弹尺寸
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
	// 控件变量
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
