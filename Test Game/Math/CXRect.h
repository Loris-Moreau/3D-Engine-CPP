#pragma once

#include "..\CXPrerequisites.h"

class  CXRect
{
public:
	CXRect() {}
	CXRect(i32 width, i32 height) : width(width), height(height) {}
	CXRect(i32 left, i32 top, i32 width, i32 height) :left(left), top(top), width(width), height(height) {}
	CXRect(const  CXRect& rect) :left(rect.left), top(rect.top), width(rect.width), height(rect.height) {}
	
	i32 width = 0, height = 0, left = 0, top = 0;
};
