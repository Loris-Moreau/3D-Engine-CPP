#include "Display.h"
#include "Game.h"
#include "GraphicsEngine.h"

Display::Display(Game* game)
{
    m_game = game;
    m_swapChain = game->getGraphicsEngine()->createSwapChain({ m_hwnd, getClientSize() });
}

void Display::onSize(const  Rect& size)
{
    m_swapChain->resize(size);
    m_game->onDisplaySize(size);
}

SwapChainPtr Display::getSwapChain()
{
    return m_swapChain;
}