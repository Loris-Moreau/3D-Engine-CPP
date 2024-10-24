#pragma once

#include <set>
#include "..\Math\Rect.h"

class  Window
{
public:
	Window();
	~Window();

	Rect getClientSize();
	Rect getScreenSize();

	//EVENT
	virtual void onCreate() {}
	virtual void onUpdate() {}
	virtual void onDestroy() {}
	virtual void onFocus() {}
	virtual void onKillFocus() {}
	virtual void onSize(const  Rect& size) {}

	void* getHandle();
	void setTitle(const wchar_t* title);

protected:
	void* m_hwnd = nullptr;
};
