#include "pch.h"
#include "CChess.h"
#include "Resource.h"

int CChess::m_dx = 271;					//该坐标可根据背景图调整
int CChess::m_dy = 68;					//该坐标可根据背景图调整
double CChess::m_d = 36.5;					//该间距可根据背景图调整

CChess::CChess() {
}

CChess::~CChess() {
}

void CChess::Set(int num, int x, int y, COLOR color) {
	m_Num = num;							//设置落子序号
	m_x = x;							//设置落子逻辑坐标x
	m_y = y;							//设置落子逻辑坐标y
	m_Color = color;						//设置落子颜色
}

void CChess::Show(CDC* pDC) {
	CBrush brush;
	if (m_Color == WHITE) {
		brush.CreateSolidBrush(RGB(255, 255, 255));
	}
	else {
		brush.CreateSolidBrush(RGB(0, 0, 0));
	}
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	double radius = m_d / 2.0 * 0.9;
	int x = (int)(m_dx + m_x * m_d);
	int y = (int)(m_dy + m_y * m_d);
	pDC->Ellipse(
		(int)(x - radius),
		(int)(y - radius),
		(int)(x + radius),
		(int)(y + radius)
	);
	pDC->SelectObject(pOldBrush);
} 