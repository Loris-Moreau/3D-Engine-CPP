#pragma once

#include <set>
#include "..\Math\Rect.h"
#include "..\Window\Window.h"

class  Game;
class  Display : public  Window
{
public:
	Display(Game* game);
	//EVENTS
	void onSize(const  Rect& size) override;

	SwapChainPtr getSwapChain();
	
private:
	Game* m_game = nullptr;
	SwapChainPtr m_swapChain;
};
