﻿// GoBangView.cpp: CGoBangView 类的实现
//
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "GoBang.h"
#endif

#include "GoBangDoc.h"
#include "GoBangView.h"
#include "CChess.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGoBangView

IMPLEMENT_DYNCREATE(CGoBangView, CView)

BEGIN_MESSAGE_MAP(CGoBangView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGoBangView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_NEW_GAME, &CGoBangView::OnNewGame)
	ON_COMMAND(ID_NEW_GAME_AI, &CGoBangView::OnNewGameAI)
	ON_COMMAND(ID_EDIT_UNDO, &CGoBangView::OnUndo)
	ON_WM_TIMER()
	ON_COMMAND(ID_DIFFICULTY_EASY, &CGoBangView::OnDifficultyEasy)
	ON_COMMAND(ID_DIFFICULTY_MEDIUM, &CGoBangView::OnDifficultyMedium)
	ON_COMMAND(ID_DIFFICULTY_HARD, &CGoBangView::OnDifficultyHard)
	ON_UPDATE_COMMAND_UI(ID_DIFFICULTY_EASY, &CGoBangView::OnUpdateDifficultyEasy)
	ON_UPDATE_COMMAND_UI(ID_DIFFICULTY_MEDIUM, &CGoBangView::OnUpdateDifficultyMedium)
	ON_UPDATE_COMMAND_UI(ID_DIFFICULTY_HARD, &CGoBangView::OnUpdateDifficultyHard)
END_MESSAGE_MAP()

// CGoBangView 构造/析构

CGoBangView::CGoBangView() noexcept
{
	// TODO: 在此处添加构造代码
	m_isGameOver = false;

}

CGoBangView::~CGoBangView()
{
	mciSendString(_T("close bgm"), NULL, 0, NULL);
	KillTimer(1);
}

BOOL CGoBangView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

void CGoBangView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	SetTimer(1, 1000, NULL);
	mciSendString(_T("open res/background.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("play bgm repeat"), NULL, 0, NULL);
}

// CGoBangView 绘图

void CGoBangView::OnDraw(CDC* pDC)
{
	CGoBangDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	// 绘制棋盘
	CBrush brush(RGB(218, 165, 32)); // 使用GDI绘制棋盘背景色 (Goldenrod)
	CRect boardRect(
		(int)(CChess::m_dx - CChess::m_d / 2),
		(int)(CChess::m_dy - CChess::m_d / 2),
		(int)(CChess::m_dx + (MAX_COLS - 1) * CChess::m_d + CChess::m_d / 2),
		(int)(CChess::m_dy + (MAX_ROWS - 1) * CChess::m_d + CChess::m_d / 2)
	);
	pDC->FillRect(&boardRect, &brush);

	// 绘制网格线
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	for (int i = 0; i < MAX_ROWS; i++) {
		pDC->MoveTo((int)CChess::m_dx, (int)(CChess::m_dy + i * CChess::m_d));
		pDC->LineTo((int)(CChess::m_dx + (MAX_COLS - 1) * CChess::m_d), (int)(CChess::m_dy + i * CChess::m_d));
	}

	for (int j = 0; j < MAX_COLS; j++) {
		pDC->MoveTo((int)(CChess::m_dx + j * CChess::m_d), (int)CChess::m_dy);
		pDC->LineTo((int)(CChess::m_dx + j * CChess::m_d), (int)(CChess::m_dy + (MAX_ROWS - 1) * CChess::m_d));
	}

	pDC->SelectObject(pOldPen);

	// 绘制棋子
	m_chess.Show(pDC);

	// 绘制计时器
	CString blackTimeStr, whiteTimeStr;
	blackTimeStr.Format(_T("黑方用时: %d s"), m_chess.GetBlackTime());
	whiteTimeStr.Format(_T("白方用时: %d s"), m_chess.GetWhiteTime());
	
	pDC->TextOut(20, 20, blackTimeStr);
	pDC->TextOut(20, 40, whiteTimeStr);

	// 绘制局势评估条
	long blackScore, whiteScore;
	m_chess.GetBoardScore(blackScore, whiteScore);
	long totalScore = blackScore + whiteScore;
	if (totalScore > 0)
	{
		CRect scoreBarRect(20, 70, 220, 90);
		pDC->FillSolidRect(scoreBarRect, RGB(255, 255, 255)); // 背景
		
		int blackWidth = (int)(200.0 * blackScore / totalScore);
		pDC->FillSolidRect(CRect(20, 70, 20 + blackWidth, 90), RGB(0, 0, 0));
	}
}


// CGoBangView 打印


void CGoBangView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGoBangView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGoBangView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CGoBangView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CGoBangView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGoBangView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGoBangView 诊断

#ifdef _DEBUG
void CGoBangView::AssertValid() const
{
	CView::AssertValid();
}

void CGoBangView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGoBangDoc* CGoBangView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGoBangDoc)));
	return (CGoBangDoc*)m_pDocument;
}
#endif //_DEBUG


// CGoBangView 消息处理程序

void CGoBangView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_isGameOver) {
		HandleGameOver();
		return;
	}
	if (m_chess.Add(point.x, point.y) == 0) {
		MessageBeep(MB_OK);
		m_pLast = point;
		Invalidate(false);
		if (m_chess.GameOver()) {
			m_isGameOver = true;
			Invalidate(false); // 绘制最后一个棋子
			HandleGameOver();
			return; // 游戏结束，AI不再走棋
		}

		if (m_chess.GetGameMode() == PVE) {
			m_chess.AIMove();
			Invalidate(false);
			if (m_chess.GameOver()) {
				m_isGameOver = true;
				Invalidate(false); // 绘制最后一个棋子
				HandleGameOver();
			}
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CGoBangView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && !m_isGameOver) {
		m_chess.IncrementCurrentPlayerTime();
		// 只重绘计时器和评分条区域
		CRect updateRect(20, 20, 220, 100);
		InvalidateRect(&updateRect, FALSE);
	}
	CView::OnTimer(nIDEvent);
}

void CGoBangView::OnNewGame()
{
	// TODO: 在此添加命令处理程序代码
	m_chess.NewGame();
	m_isGameOver = false;
	Invalidate(false);
}

void CGoBangView::OnNewGameAI()
{
	m_chess.NewGame(PVE, m_chess.GetDifficulty());
	m_isGameOver = false;
	Invalidate(false);
}

void CGoBangView::OnUndo()
{
	m_chess.Undo();
	if (m_chess.GetGameMode() == PVE) {
		m_chess.Undo(); // 在PVE模式下，悔棋需要撤销两步（玩家和AI）
	}
	Invalidate(false);
}

void CGoBangView::OnDifficultyEasy()
{
	m_chess.SetDifficulty(EASY);
}

void CGoBangView::OnDifficultyMedium()
{
	m_chess.SetDifficulty(MEDIUM);
}

void CGoBangView::OnDifficultyHard()
{
	m_chess.SetDifficulty(HARD);
}

void CGoBangView::OnUpdateDifficultyEasy(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_chess.GetDifficulty() == EASY);
}

void CGoBangView::OnUpdateDifficultyMedium(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_chess.GetDifficulty() == MEDIUM);
}

void CGoBangView::OnUpdateDifficultyHard(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_chess.GetDifficulty() == HARD);
}

void CGoBangView::HandleGameOver()
{
	long blackScore, whiteScore;
	m_chess.GetBoardScore(blackScore, whiteScore);

	CString str;
	CString winnerStr = _T("Unknown");
	if (m_chess.GetWinner() != NONE)
	{
		winnerStr.Format(_T("%s win!"), m_chess.GetWinner() == BLACK ? _T("Black") : _T("White"));
	}
	
	str.Format(_T("%s\n\nFinal Score:\nBlack: %ld\nWhite: %ld\n\nDo you want to play again?"),
		winnerStr,
		blackScore, whiteScore);

	if (MessageBox(str, _T("Game Over"), MB_YESNO) == IDYES)
	{
		if (m_chess.GetGameMode() == PVE)
		{
			OnNewGameAI();
		}
		else
		{
			OnNewGame();
		}
	}
}
