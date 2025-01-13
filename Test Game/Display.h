#pragma once

#include <set>

#include "Rect.h"
#include "Window.h"

class  Game;

class  Display : public  Window
{
public:
    Display(Game* game);
    ~Display() = default;
    
    // Events
    void onSize(const  Rect& size) override;

    SwapChainPtr getSwapChain();
    
private:
    Game* m_game = nullptr;
    SwapChainPtr m_swapChain;
};
