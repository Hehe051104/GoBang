// GoBangView.cpp: CGoBangView 类的实现
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
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CGoBangView 构造/析构

CGoBangView::CGoBangView() noexcept
{
	// TODO: 在此处添加构造代码
	m_isGameOver = false;

}

CGoBangView::~CGoBangView()
{
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

	if (m_isGameOver) {
		CString str;
		str.Format(_T("%s win!"), m_chess.GetWinner() == BLACK ? _T("Black") : _T("White"));
		MessageBox(str);
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
		MessageBox(_T("Game over!"));
		return;
	}
	if (m_chess.Add(point.x, point.y) == 0) {
		m_pLast = point;
		Invalidate(false);
		if (m_chess.GameOver()) {
			m_isGameOver = true;
			Invalidate(false);
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CGoBangView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && !m_isGameOver) {
		m_chess.IncrementCurrentPlayerTime();
		// 只重绘计时器区域，避免闪烁
		CRect timerRect(20, 20, 200, 60);
		InvalidateRect(&timerRect, FALSE);
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
