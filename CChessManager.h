#pragma once
#include "CChess.h"

#define MAX_ROWS   	15					//棋盘行数
#define MAX_COLS   	15					//棋盘列数
#define MAX_CHESS  	(MAX_ROWS * MAX_COLS)	//最多落子数
#define WIN_NUM		5

class CChessManager {
	CChess m_aChess[MAX_CHESS];	//保存落子信息对象数组
	int m_nChess;		//落子个数
	COLOR m_Color;			//当前将要落子的颜色
	int m_blackTime;		//黑方用时，单位：秒
	int m_whiteTime;		//白方用时，单位：秒
	bool CheckRows();				//检查行是否达到赢棋标准
	bool CheckCols();				//检查列是否达到赢棋标准
	bool CheckLSlash();				//检查左斜线 '\'方向是否达到赢棋标准
	bool CheckRSlash();				//检查右斜线 '/'方向是否达到赢棋标准

public:
	CChessManager();
	~CChessManager();
	void NewGame();
	bool Xy2Xy(int x0, int y0, int& x1, int& y1);
	int Add(int x, int y);
	void Show(CDC* pDC);
	bool GameOver();
	COLOR GetWinner();
	CChess* GetQz(int x, int y);	//取得指定逻辑坐标的棋子，无则返回空
	int GetBlackTime() { return m_blackTime; }	//获取黑方用时
	int GetWhiteTime() { return m_whiteTime; }	//获取白方用时
	void IncrementCurrentPlayerTime();			//增加当前走棋方用时
}; 