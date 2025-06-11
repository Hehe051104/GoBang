#include "pch.h"
#include "CChessManager.h"
#include <cmath>

CChessManager::CChessManager() {
	NewGame();
}
void CChessManager::NewGame()
{
	m_nChess = 0;
	m_Color = BLACK;
	m_blackTime = 0;
	m_whiteTime = 0;
}
CChessManager::~CChessManager() {
}
bool CChessManager::Xy2Xy(int x0, int y0, int& x1, int& y1) {
	int x, y;						//棋盘上每个交叉点位置的坐标
	for (int i = 0; i < 15; i++)					//遍历棋盘每一列
		for (int j = 0; j < 15; j++) {				//遍历棋盘每一行
			x = (int)(CChess::m_dx + i * CChess::m_d); //交叉点x坐标
			y = (int)(CChess::m_dy + j * CChess::m_d); //交叉点y坐标
			if (sqrt((double)(x - x0) * (x - x0) + (y - y0) * (y - y0)) < 15) {	//（x,y）到交叉点距离小于15
				x1 = i;
				y1 = j;			//设置逻辑坐标
				return true;
			}
		}
	return false;							//没有找到（x,y）临近的交叉点
}
int CChessManager::Add(int x, int y) { //落子，成功返回0，失败返回非0值
	int x1, y1;
	if (!Xy2Xy(x, y, x1, y1))
		return 1;			//物理坐标（x,y）不在棋盘交叉点上，返回1
	for (int i = 0; i < m_nChess; i++)	//遍历落子数组，根据逻辑坐标判断是否已有落子
		if (x1 == m_aChess[i].GetX() && y1 == m_aChess[i].GetY())
			return 2;			//已有落子，返回2
	m_aChess[m_nChess].Set(m_nChess, x1, y1, m_Color);
	m_nChess++;					//落子数量加1		
	m_Color = (m_Color == WHITE ? BLACK : WHITE);	//设置下次落子的颜色
	return 0;					//落子成功返回0
}
void CChessManager::IncrementCurrentPlayerTime()
{
	if (m_Color == BLACK) {
		m_blackTime++;
	}
	else {
		m_whiteTime++;
	}
}
void CChessManager::Show(CDC* pDC) {	//显示所有落子
	for (int i = 0; i < m_nChess; i++)
		m_aChess[i].Show(pDC);
}
bool CChessManager::GameOver() {		//判断游戏是否结束
	if (CheckRows())			//判断棋盘所有行
		return true;
	if (CheckCols())			//判断棋盘所有列
		return true;
	if (CheckLSlash())			//判断棋盘所有"\"方向左斜线
		return true;
	if (CheckRSlash())			//判断棋盘所有"/"方向右斜线
		return true;
	return false;
}
COLOR CChessManager::GetWinner()
{
	return m_nChess > 0 ? m_aChess[m_nChess - 1].GetColor() : BLACK;
}
CChess* CChessManager::GetQz(int x, int y) {	//根据逻辑坐标取得棋子，失败返回空
	if (x < 0 || x >= MAX_COLS || y < 0 || y >= MAX_ROWS) {
		return nullptr;
	}
	for (int i = 0; i < m_nChess; i++)
		if (m_aChess[i].GetX() == x && m_aChess[i].GetY() == y)
			return &m_aChess[i];
	return nullptr;
}
bool CChessManager::CheckRows() {
	CChess* pQz;							//取得棋子
	COLOR color;							//上一个棋子颜色
	int iCount;							//已连续同色棋子个数
	for (int i = 0; i < MAX_ROWS; i++)		//扫描所有行
	{
		iCount = 0; 						//已连续同色棋子初始数量为0
		for (int j = 0; j < MAX_COLS; j++)
			if (pQz = GetQz(j, i)) {			//取得下一个棋子
				if (iCount == 0) {			//如果是第一个棋子
					color = pQz->GetColor();	//保存颜色
					iCount++;			//记录已连续数量
				}
				else if (color == pQz->GetColor()) {
					iCount++;			//已连续数量加1
					if (iCount == WIN_NUM)			//已达到获胜数
						return true;			//游戏结束
				}
				else {						//新颜色棋子
					color = pQz->GetColor();		//保存颜色
					iCount = 1;				//记录数量
				}
			}
			else							//该行、列无棋子
				iCount = 0;					//连续棋子数量置0
	}
	return false;								//未达获胜数
}
bool CChessManager::CheckCols() {					//按列方向检查
	CChess* pQz;								//取得棋子
	COLOR color;								//上一个棋子颜色
	int iCount;
	for (int i = 0; i < MAX_COLS; i++) {				//扫描所有列
		iCount = 0;						//已连续同色棋子初始数量为0
		for (int j = 0; j < MAX_ROWS; j++)			//扫描所有行
			if (pQz = GetQz(i, j)) {				//取得下一棋子
				if (iCount == 0) {				//如果是第一个棋子
					color = pQz->GetColor();
					iCount++;				//记录已连续数量
				}
				else if (color == pQz->GetColor()) {	//是同色连续棋子
					iCount++;			//已连续数量加1
					if (iCount == WIN_NUM)			//已达到获胜数量
						return true;			//游戏结束
				}
				else {						//新颜色棋子
					color = pQz->GetColor();		//保存颜色
					iCount = 1;				//记录数量
				}
			}
			else							//该行、列无棋子
				iCount = 0;
	}
	return false;
}
bool CChessManager::CheckLSlash() {		//检查左斜线 '\'方向
	CChess* pQz;
	COLOR color;					//上一个棋子颜色
	int iCount;					//已连续同色棋子个数
	for (int i = -14; i < MAX_COLS; i++) {		//棋盘左侧虚拟14列，扫描所有列
		iCount = 0;				//已连续同色棋子初始数量为0
		for (int j = 0; j < MAX_ROWS; j++)	//扫描所有行
			if (pQz = GetQz(i + j, j)) {	//取得下一棋子
				if (iCount == 0) {			//如果是第一个棋子
					color = pQz->GetColor();	//保存颜色
					iCount++;			//记录已连续数量
				}
				else if (color == pQz->GetColor()) {	//是同色连续棋子
					iCount++;		//已连续数量加1
					if (iCount == WIN_NUM)		//已达到获胜数量
						return true;		//游戏结束
				}
				else {					//新颜色棋子
					color = pQz->GetColor();	//保存颜色
					iCount = 1;
				}
			}
			else						//该行、列无棋子
				iCount = 0;				//连续数量置0
	}
	return false;
}
bool CChessManager::CheckRSlash() {
	CChess* pQz;
	COLOR color;							//上一个棋子颜色
	int iCount;							//已连续同色棋子个数
	for (int i = 0; i < MAX_COLS + 14; i++) {		//棋盘右侧虚拟14列，扫描所有列
		iCount = 0;						//已连续同色棋子初始数量为0
		for (int j = 0; j < MAX_ROWS; j++)			//扫描所有行
			if (pQz = GetQz(i - j, j)) {
				if (iCount == 0) {			//如果是第一个棋子
					color = pQz->GetColor();	//保存颜色
					iCount++;			//记录已连续数量
				}
				else if (color == pQz->GetColor()) {	//是同色连续棋子
					iCount++;		//已连续数量加1
					if (iCount == WIN_NUM)		//已达到获胜数量
						return true;
				}
				else {					//新颜色棋子
					color = pQz->GetColor();	//保存颜色
					iCount = 1;			//记录数量
				}
			}
			else						//该行列、无棋子
				iCount = 0;				//连续数量置0
	}
	return false;
} 