#pragma once

#include <set>
#include "..\Math\CXRect.h"

class  CXWindow
{
public:
	CXWindow();
	~CXWindow();

	CXRect getClientSize();
	CXRect getScreenSize();

	//EVENT
	virtual void onCreate() {}
	virtual void onUpdate() {}
	virtual void onDestroy() {}
	virtual void onFocus() {}
	virtual void onKillFocus() {}
	virtual void onSize(const  CXRect& size) {}

	void* getHandle();
	void setTitle(const wchar_t* title);

protected:
	void* m_hwnd = nullptr;
};
