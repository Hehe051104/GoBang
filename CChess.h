#pragma once

// MFC GDI anviert Klasse
class CDC;

enum COLOR { NONE, BLACK, WHITE };			//棋子只有两种颜色
class CChess {
	int m_Num;				//序号
	int m_x;				//棋子位置x坐标
	int m_y;				//棋子位置y坐标
	COLOR m_Color; 			//棋子颜色
public:
	CChess();
	~CChess();
	void Set(int num, int x, int y, COLOR color);	//设置棋子属性
	COLOR GetColor() { return m_Color; }		//取得棋子颜色
	int GetX() { return m_x; }			//取得棋子逻辑坐标x
	int GetY() { return m_y; }			//取得棋子逻辑坐标y
	void Show(CDC* pDC);				//显示棋子
	static int m_dx;				//棋盘左上角屏幕坐标x
	static int m_dy;				//棋盘左上角屏幕坐标y
	static double m_d;				//棋子间距离
}; 