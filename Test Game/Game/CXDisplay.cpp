#include "CXDisplay.h"
#include "..\Graphics\CXGraphicsEngine.h"
#include "CXGame.h"

CXDisplay::CXDisplay(CXGame* game)
{
	m_game = game;
	m_swapChain = game->getGraphicsEngine()->createSwapChain({ m_hwnd, getClientSize() });
}

void CXDisplay::onSize(const  CXRect& size)
{
	m_swapChain->resize(size);
	m_game->onDisplaySize(size);
}

CXSwapChainPtr CXDisplay::getSwapChain()
{
	return m_swapChain;
}
