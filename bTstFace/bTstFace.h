// bTstFace.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
//
//#ifndef _HAVECOLOR
//#define _HAVECOLOR
//typedef struct colorRGB						//用来设置对话框的颜色
//{
//	int r;
//	int g;
//	int b;
//}MyColor;
//MyColor DlgColor,TextColor,StaticColor;	//分别为对话框颜色，文字颜色，静态控件背景色
//#endif

// CbTstFaceApp:
// 有关此类的实现，请参阅 bTstFace.cpp
//

class CbTstFaceApp : public CWinApp
{
public:
	CbTstFaceApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CbTstFaceApp theApp;