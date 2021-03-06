#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

	class UILIB_API CControlUI
	{
	public:
		CControlUI();
		virtual ~CControlUI();

	public:
		virtual CDuiString GetName() const;
		virtual void SetName(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual UINT GetControlFlags() const;

		virtual bool Activate();
		virtual CPaintManagerUI* GetManager() const;
		virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
		virtual CControlUI* GetParent() const;

		// 文本相关
		virtual CDuiString GetText() const;
		virtual void SetText(LPCTSTR pstrText);

		// 图形相关
		DWORD GetBkColor() const;
		void SetBkColor(DWORD dwBackColor);
		DWORD GetBkColor2() const;
		void SetBkColor2(DWORD dwBackColor);
		DWORD GetBkColor3() const;
		void SetBkColor3(DWORD dwBackColor);
		LPCTSTR GetBkImage();
		void SetBkImage(LPCTSTR pStrImage);
		DWORD GetFocusBorderColor() const;
		void SetFocusBorderColor(DWORD dwBorderColor);
		bool IsColorHSL() const;
		void SetColorHSL(bool bColorHSL);
		SIZE GetBorderRound() const;
		void SetBorderRound(SIZE cxyRound);
		bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

		//边框相关
		int GetBorderSize() const;
		void SetBorderSize(int nSize);
		DWORD GetBorderColor() const;            //获取边界颜色
		void SetBorderColor(DWORD dwBorderColor);

		void SetBorderSize(RECT rc);
		int GetLeftBorderSize() const;           //获取边界尺寸的左侧长度
		void SetLeftBorderSize(int nSize);		 //设置边界尺寸的左侧长度，完成后重绘窗体
		int GetTopBorderSize() const;            //获取边界尺寸的上侧长度
		void SetTopBorderSize(int nSize); 		 //设置边界尺寸的上侧长度，完成后重绘窗体
		int GetRightBorderSize() const;			 //获取边界尺寸的右侧长度
		void SetRightBorderSize(int nSize); 	 //设置边界尺寸的右侧长度，完成后重绘窗体
		int GetBottomBorderSize() const;		 //获取边界尺寸的下侧长度
		void SetBottomBorderSize(int nSize);	 //设置边界尺寸的下侧长度，完成后重绘窗体
		int GetBorderStyle() const;
		void SetBorderStyle(int nStyle);

		// 位置相关
		virtual const RECT& GetPos() const;
		virtual void SetPos(RECT rc);
		virtual int GetWidth() const;
		virtual int GetHeight() const;
		virtual int GetX() const;
		virtual int GetY() const;
		virtual RECT GetPadding() const;
		virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
		virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
		virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		virtual void SetFixedWidth(int cx);      // 预设的参考值
		virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
		virtual void SetFixedHeight(int cy);     // 预设的参考值
		virtual int GetMinWidth() const;
		virtual void SetMinWidth(int cx);
		virtual int GetMaxWidth() const;
		virtual void SetMaxWidth(int cx);
		virtual int GetMinHeight() const;
		virtual void SetMinHeight(int cy);
		virtual int GetMaxHeight() const;
		virtual void SetMaxHeight(int cy);
		virtual void SetRelativePos(SIZE szMove, SIZE szZoom);
		virtual void SetRelativeParentSize(SIZE sz);
		virtual TRelativePosUI GetRelativePos() const;
		virtual bool IsRelativePos() const;

		// 鼠标提示
		virtual CDuiString GetToolTip() const;
		virtual void SetToolTip(LPCTSTR pstrText);
		virtual void SetToolTipWidth(int nWidth);
		virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

		// 快捷键
		virtual TCHAR GetShortcut() const;
		virtual void SetShortcut(TCHAR ch);

		// 菜单
		virtual bool IsContextMenuUsed() const;
		virtual void SetContextMenuUsed(bool bMenuUsed);

		// 用户属性
		virtual const CDuiString& GetUserData(); // 辅助函数，供用户使用
		virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
		virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
		virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用

		// 一些重要的属性
		virtual bool IsVisible() const;
		virtual void SetVisible(bool bVisible = true);
		virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
		virtual bool IsEnabled() const;
		virtual void SetEnabled(bool bEnable = true);
		virtual bool IsMouseEnabled() const;
		virtual void SetMouseEnabled(bool bEnable = true);
		virtual bool IsKeyboardEnabled() const;
		virtual void SetKeyboardEnabled(bool bEnable = true);
		virtual bool IsFocused() const;
		virtual void SetFocus();
		virtual bool IsFloat() const;
		virtual void SetFloat(bool bFloat = true);

		virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

		void Invalidate();
		bool IsUpdateNeeded() const;
		void NeedUpdate();
		void NeedParentUpdate();
		DWORD GetAdjustColor(DWORD dwColor);

		virtual void Init();  //预定义的初始化流程：先执行默认初始化，然后执行初始化委托，
		virtual void DoInit();//默认初始化

		virtual void Event(TEventUI& event);
		virtual void DoEvent(TEventUI& event);

		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		CControlUI* ApplyAttributeList(LPCTSTR pstrList);

		virtual SIZE EstimateSize(SIZE szAvailable);

		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void PaintBkColor(HDC hDC);
		virtual void PaintBkImage(HDC hDC);
		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintText(HDC hDC);
		virtual void PaintBorder(HDC hDC);

		virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

		//虚拟窗口参数
		void SetVirtualWnd(LPCTSTR pstrValue);
		CDuiString GetVirtualWnd() const;

	public:
		CEventSource OnInit;   //控件的初始化事件
		CEventSource OnDestroy;
		CEventSource OnSize;
		CEventSource OnEvent;
		CEventSource OnNotify;

	protected:
		CPaintManagerUI* m_pManager;
		CControlUI* m_pParent;
		CDuiString m_sVirtualWnd;
		CDuiString m_sName;
		bool m_bUpdateNeeded;
		bool m_bMenuUsed;
		RECT m_rcItem;
		RECT m_rcPadding;
		SIZE m_cXY;
		SIZE m_cxyFixed;
		SIZE m_cxyMin;
		SIZE m_cxyMax;
		bool m_bVisible;
		bool m_bInternVisible;
		bool m_bEnabled;
		bool m_bMouseEnabled;
		bool m_bKeyboardEnabled;
		bool m_bFocused;
		bool m_bFloat;
		bool m_bSetPos; // 防止SetPos循环调用
		TRelativePosUI m_tRelativePos;

		CDuiString m_sText;		   //***文本，具体看子类怎么用
		CDuiString m_sToolTip;	   //提示文本
		TCHAR m_chShortcut;        //
		CDuiString m_sUserData;	   //用户数据，具体看怎么用，这个变量子类没有使用，当前类中，就设置和读取了一下
		UINT_PTR m_pTag;		   //依赖于当前控件的一个指针类型的变量，应该类似于C#中的控件的Tag属性

		DWORD m_dwBackColor;	   //颜色混合
		DWORD m_dwBackColor2;	   //颜色混合
		DWORD m_dwBackColor3;	   //颜色混合
		CDuiString m_sBkImage;
		CDuiString m_sForeImage;
		DWORD m_dwBorderColor;     //边界颜色，无符号长整形形式的颜色值
		DWORD m_dwFocusBorderColor;//聚焦时,窗体的边界颜色，颜色为无符号长整形形式
		bool m_bColorHSL;
		int m_nBorderSize;
		int m_nBorderStyle;
		int m_nTooltipWidth;
		SIZE m_cxyBorderRound;
		RECT m_rcPaint;			   //绘制区域吗？需要仔细查看
		RECT m_rcBorderSize;       //窗体边界尺寸
	};

} // namespace DuiLib

#endif // __UICONTROL_H__
