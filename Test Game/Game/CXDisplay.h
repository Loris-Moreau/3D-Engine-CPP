#pragma once

#include <set>
#include "..\Math\CXRect.h"
#include "..\Window\CXWindow.h"

class  CXGame;
class  CXDisplay : public  CXWindow
{
public:
	CXDisplay(CXGame* game);
	//EVENTS
	void onSize(const  CXRect& size) override;

	CXSwapChainPtr getSwapChain();
	
private:
	CXGame* m_game = nullptr;
	CXSwapChainPtr m_swapChain;
};
