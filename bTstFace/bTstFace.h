// bTstFace.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
//
//#ifndef _HAVECOLOR
//#define _HAVECOLOR
//typedef struct colorRGB						//�������öԻ������ɫ
//{
//	int r;
//	int g;
//	int b;
//}MyColor;
//MyColor DlgColor,TextColor,StaticColor;	//�ֱ�Ϊ�Ի�����ɫ��������ɫ����̬�ؼ�����ɫ
//#endif

// CbTstFaceApp:
// �йش����ʵ�֣������ bTstFace.cpp
//

class CbTstFaceApp : public CWinApp
{
public:
	CbTstFaceApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CbTstFaceApp theApp;