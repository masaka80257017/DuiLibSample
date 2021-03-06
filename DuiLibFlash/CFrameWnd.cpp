
#include "CFrameWnd.h"
#include "flash10a.tlh"

CFrameWnd::CFrameWnd()
{
	//
}

LPCTSTR CFrameWnd::GetWindowClassName() const
{
	return _T("UIFrame");
}

UINT CFrameWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_DIALOG;
}

void CFrameWnd::OnFinalMessage(HWND)
{
	delete this;
}

void CFrameWnd::Init()
{
	//
}

void CFrameWnd::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("closebtn")
			|| msg.pSender->GetName() == _T("closebtn2"))
		{
			PostQuitMessage(0);
			return;
		}
	}
	else if (msg.sType == _T("showactivex"))
	{
		if (msg.pSender->GetName() != _T("flash"))
		{
			return;
		}
		IShockwaveFlash* pFlash = NULL;
		DuiLib::CActiveXUI* pActiveX = static_cast<DuiLib::CActiveXUI*>(msg.pSender);
		pActiveX->GetControl(IID_IUnknown, (void**)& pFlash);
		if (pFlash != NULL)
		{
			pFlash->put_WMode(_bstr_t(_T("Transparent")));
			pFlash->put_Movie(_bstr_t(DuiLib::CPaintManagerUI::GetInstancePath() + _T("\\test.swf")));
			pFlash->DisableLocalSecurity();
			pFlash->put_AllowScriptAccess(_bstr_t(_T("always")));
			BSTR response;
			pFlash->CallFunction(_bstr_t(_T("<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>")), &response);
			pFlash->Release();
		}
	}
}

LRESULT CFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI* pRoot = builder.Create(_T("ui.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CFrameWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);

	bHandled = FALSE;
	return 0;
}

LRESULT CFrameWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::IsIconic(*this)) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CFrameWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFrameWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFrameWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if (pt.x >= rcClient.left + rcCaption.left
		&& pt.x < rcClient.right - rcCaption.right
		&& pt.y >= rcCaption.top
		&& pt.y < rcCaption.bottom)
	{
		DuiLib::CControlUI* pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(pt));
		if (pControl
			&& _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0)
		{
			return HTCAPTION;
		}
	}

	return HTCLIENT;
}

LRESULT CFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if (!::IsIconic(*this)
		&& (szRoundCorner.cx != 0
			|| szRoundCorner.cy != 0))
	{
		DuiLib::CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
	}
	if (bHandled) return lRes;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}
