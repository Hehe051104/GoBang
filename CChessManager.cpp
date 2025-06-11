#include "pch.h"
#include "CChessManager.h"
#include <cmath>
#include <vector>
#include <algorithm>

struct Move
{
	int x, y;
	long score;
};

bool compareMoves(const Move& a, const Move& b)
{
	return a.score > b.score;
}

CChessManager::CChessManager() {
	NewGame();
}
void CChessManager::NewGame(GameMode mode, AIDifficulty difficulty)
{
	m_nChess = 0;
	m_Color = BLACK;
	m_blackTime = 0;
	m_whiteTime = 0;
	m_gameMode = mode;
	m_aiDifficulty = difficulty;
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

void CChessManager::AIMove()
{
	if (m_gameMode == PVE)
	{
		std::vector<Move> moves;

		for (int y = 0; y < MAX_ROWS; y++)
		{
			for (int x = 0; x < MAX_COLS; x++)
			{
				if (GetQz(x, y) == nullptr) // 只在空位评估
				{
					long aiScore = GetScore(x, y, m_Color);
					long playerScore = GetScore(x, y, (m_Color == WHITE ? BLACK : WHITE));
					moves.push_back({ x, y, aiScore + playerScore });
				}
			}
		}

		std::sort(moves.begin(), moves.end(), compareMoves);

		int bestX = -1, bestY = -1;

		if (!moves.empty())
		{
			if (m_aiDifficulty == EASY)
			{
				int range = (std::min)((int)moves.size(), 5);
				int choice = rand() % range;
				bestX = moves[choice].x;
				bestY = moves[choice].y;
			}
			else // MEDIUM and HARD
			{
				bestX = moves[0].x;
				bestY = moves[0].y;
			}
		}


		if (bestX != -1 && bestY != -1)
		{
			// AI落子
			m_aChess[m_nChess].Set(m_nChess, bestX, bestY, m_Color);
			m_nChess++;
			m_Color = (m_Color == WHITE ? BLACK : WHITE);
		}
	}
}

void CChessManager::Undo()
{
	if (m_nChess > 0) {
		m_nChess--;
		m_Color = (m_Color == WHITE ? BLACK : WHITE);
	}
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

long CChessManager::GetScore(int x, int y, COLOR color)
{
	long totalScore = 0;
	int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} }; // 水平, 垂直, 右斜, 左斜

	for (int i = 0; i < 4; i++)
	{
		int count = 1;    // 连续棋子数
		int openEnds = 0; // 开放端数

		// 正方向
		for (int j = 1; j < WIN_NUM; j++)
		{
			CChess* qz = GetQz(x + j * directions[i][0], y + j * directions[i][1]);
			if (qz && qz->GetColor() == color)
				count++;
			else
			{
				if (qz == nullptr) openEnds++;
				break;
			}
		}

		// 反方向
		for (int j = 1; j < WIN_NUM; j++)
		{
			CChess* qz = GetQz(x - j * directions[i][0], y - j * directions[i][1]);
			if (qz && qz->GetColor() == color)
				count++;
			else
			{
				if (qz == nullptr) openEnds++;
				break;
			}
		}

		if (count >= WIN_NUM) totalScore += 100000; // 连五
		else if (count == 4 && openEnds == 2) totalScore += 10000; // 活四
		else if (count == 4 && openEnds == 1) totalScore += 1000;  // 死四
		else if (count == 3 && openEnds == 2) totalScore += 1000;  // 活三
		else if (count == 3 && openEnds == 1) totalScore += 100;   // 死三
		else if (count == 2 && openEnds == 2) totalScore += 100;   // 活二
		else if (count == 2 && openEnds == 1) totalScore += 10;    // 死二
		else if (count == 1 && openEnds == 2) totalScore += 10;    // 活一
	}
	return totalScore;
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