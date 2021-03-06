
#include "UIBase.h"

#include <windowsx.h>
#include <tchar.h>
#include "UIManager.h"

namespace DuiLib {

	//////////////////////////////////////////////////////////////////////////
	///

	CWindowWnd::CWindowWnd() : m_hWnd(NULL), m_OldWndProc(::DefWindowProc), m_bSubclassed(false)
	{
	}

	HWND CWindowWnd::GetHWND() const
	{
		return m_hWnd;
	}

	UINT CWindowWnd::GetClassStyle() const
	{
		return 0;
	}

	LPCTSTR CWindowWnd::GetSuperClassName() const
	{
		return NULL;//已经是顶级的抽象基类，不可被实例化，没有ClassName
	}

	CWindowWnd::operator HWND() const
	{
		return m_hWnd;
	}

	HWND CWindowWnd::CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName, DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/)
	{
		return Create(hwndParent, pstrWindowName, dwStyle, dwExStyle, 0, 0, 0, 0, NULL);
	}

	HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
	{
		return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
	}

	HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
	{
		if (GetSuperClassName() != NULL && !RegisterSuperclass()) return NULL;
		if (GetSuperClassName() == NULL && !RegisterWindowClass()) return NULL;
		m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, CPaintManagerUI::GetInstance(), this);//this 为 CWindowWnd *
		//ASSERT(m_hWnd != NULL);
		return m_hWnd;
	}

	HWND CWindowWnd::Subclass(HWND hWnd)
	{
		//ASSERT(::IsWindow(hWnd));
		//ASSERT(m_hWnd == NULL);
		m_OldWndProc = SubclassWindow(hWnd, __WndProc);
		if (m_OldWndProc == NULL) return NULL;
		m_bSubclassed = true;
		m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
		return m_hWnd;
	}

	void CWindowWnd::Unsubclass()
	{
		//ASSERT(::IsWindow(m_hWnd));
		if (!::IsWindow(m_hWnd)) return;
		if (!m_bSubclassed) return;
		SubclassWindow(m_hWnd, m_OldWndProc);
		m_OldWndProc = ::DefWindowProc;
		m_bSubclassed = false;
	}

	void CWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
	{
		//ASSERT(::IsWindow(m_hWnd));
		if (!::IsWindow(m_hWnd)) return;
		::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
	}

	UINT CWindowWnd::ShowModal()
	{
		//ASSERT(::IsWindow(m_hWnd));
		UINT nRet = 0;
		HWND hWndParent = GetWindowOwner(m_hWnd);
		::ShowWindow(m_hWnd, SW_SHOWNORMAL);
		::EnableWindow(hWndParent, FALSE);
		MSG msg = { 0 };
		while (::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0)) {
			if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd) {
				nRet = msg.wParam;
				::EnableWindow(hWndParent, TRUE);
				::SetFocus(hWndParent);
			}
			if (!CPaintManagerUI::TranslateMessage(&msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT) break;
		}
		::EnableWindow(hWndParent, TRUE);
		::SetFocus(hWndParent);
		if (msg.message == WM_QUIT) ::PostQuitMessage(msg.wParam);
		return nRet;
	}

	void CWindowWnd::Close(UINT nRet)
	{
		//ASSERT(::IsWindow(m_hWnd));
		if (!::IsWindow(m_hWnd)) return;
		PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
	}

	void CWindowWnd::CenterWindow()
	{
		//ASSERT(::IsWindow(m_hWnd));
		//ASSERT((GetWindowStyle(m_hWnd) & WS_CHILD) == 0);
		RECT rcDlg = { 0 };
		::GetWindowRect(m_hWnd, &rcDlg);
		RECT rcArea = { 0 };
		RECT rcCenter = { 0 };
		HWND hWnd = *this;
		HWND hWndParent = ::GetParent(m_hWnd);
		HWND hWndCenter = ::GetWindowOwner(m_hWnd);
		if (hWndCenter != NULL)
			hWnd = hWndCenter;

		// 处理多显示器模式下屏幕居中
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rcArea = oMonitor.rcWork;

		if (hWndCenter == NULL)
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if (xLeft < rcArea.left) xLeft = rcArea.left;
		else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
		if (yTop < rcArea.top) yTop = rcArea.top;
		else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void CWindowWnd::SetIcon(UINT nRes)
	{
		HICON hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		//ASSERT(hIcon);
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
		hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		//ASSERT(hIcon);
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
	}

	bool CWindowWnd::RegisterWindowClass()
	{
		WNDCLASS wc = { 0 };
		wc.style = GetClassStyle();//描述窗体风格（class style），比如：CS_HREDRAW | CS_VREDRAW
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = NULL;
		wc.lpfnWndProc = CWindowWnd::__WndProc;
		wc.hInstance = CPaintManagerUI::GetInstance();
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = GetWindowClassName();
		ATOM ret = ::RegisterClass(&wc);
		//ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	bool CWindowWnd::RegisterSuperclass()
	{
		// Get the class information from an existing
		// window so we can subclass it later on...
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		if (!::GetClassInfoEx(NULL, GetSuperClassName(), &wc)) {
			if (!::GetClassInfoEx(CPaintManagerUI::GetInstance(), GetSuperClassName(), &wc)) {
				//ASSERT(!"Unable to locate window class");
				return NULL;
			}
		}
		m_OldWndProc = wc.lpfnWndProc;
		wc.lpfnWndProc = CWindowWnd::__ControlProc;
		wc.hInstance = CPaintManagerUI::GetInstance();
		wc.lpszClassName = GetWindowClassName();
		ATOM ret = ::RegisterClassEx(&wc);
		//ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CWindowWnd* pThis = NULL;
		if (uMsg == WM_NCCREATE) {//窗体被创建时，比WM_CREATE更先到达的一个消息，消息的wParam没有用，lParam为一个CREATESTRUCT结构，包含被创建窗体的信息
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);//lpCreateParams，该参数为调用CreateWindowEx时的参数lParam。当前类中，调用时，lParam的值被设置为this!!!需要注意的是，this代表的指针地址既是子类对象的起始地址，也是父类对象的起始地址
			pThis->m_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));//保存this指针，下次使用
		}
		else {
			pThis = reinterpret_cast<CWindowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));//获取到this指针
			if (uMsg == WM_NCDESTROY && pThis != NULL) {//WM_NCDESTROY，窗体的非客户端区域将被销毁的消息。WM_NCDESTROY的含义是 Windows Message NonClient Destroy
				LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
				::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);//参数0L是什么意思？
				if (pThis->m_bSubclassed) pThis->Unsubclass();
				pThis->m_hWnd = NULL;
				pThis->OnFinalMessage(hWnd);
				return lRes;
			}
		}

		if (pThis != NULL) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else {
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	LRESULT CALLBACK CWindowWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CWindowWnd* pThis = NULL;
		if (uMsg == WM_NCCREATE) {
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
			::SetProp(hWnd, _T("WndX"), (HANDLE)pThis);
			pThis->m_hWnd = hWnd;
		}
		else {
			pThis = reinterpret_cast<CWindowWnd*>(::GetProp(hWnd, _T("WndX")));
			if (uMsg == WM_NCDESTROY && pThis != NULL) {
				LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
				if (pThis->m_bSubclassed) pThis->Unsubclass();
				::SetProp(hWnd, _T("WndX"), NULL);
				pThis->m_hWnd = NULL;
				pThis->OnFinalMessage(hWnd);
				return lRes;
			}
		}
		if (pThis != NULL) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else {
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
	{
		//ASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
	}

	LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
	{
		//ASSERT(::IsWindow(m_hWnd));
		return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
	}

	void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
	{
		//ASSERT(::IsWindow(m_hWnd));
		RECT rc = { 0 };
		if (!::GetClientRect(m_hWnd, &rc)) return;
		if (cx != -1) rc.right = cx;
		if (cy != -1) rc.bottom = cy;
		if (!::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd))) return;
		::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);//将消息传送给指定的窗口过程函数，因为同一类窗体共用窗口过程函数，因此，需要传递窗体句柄，以指示消息适用于哪个窗体。
	}

	void CWindowWnd::OnFinalMessage(HWND /*hWnd*/)
	{
	}

} // namespace DuiLib
