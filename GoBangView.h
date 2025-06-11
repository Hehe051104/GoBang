// GoBangView.h: CGoBangView 类的接口
//

#pragma once
#include "CChessManager.h"

class CGoBangView : public CView
{
protected: // 仅从序列化创建
	CGoBangView() noexcept;
	DECLARE_DYNCREATE(CGoBangView)

// 特性
public:
	CGoBangDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CGoBangView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CChessManager m_chess;
	bool m_isGameOver;
	CPoint m_pLast;

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNewGame();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUndo();
};

#ifndef _DEBUG  // GoBangView.cpp 中的调试版本
inline CGoBangDoc* CGoBangView::GetDocument() const
   { return reinterpret_cast<CGoBangDoc*>(m_pDocument); }
#endif

