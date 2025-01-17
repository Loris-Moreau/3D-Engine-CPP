#include <Windows.h>

#include "Descent_Game/DescentGame.h"

int main()
{
	try
	{  
		DescentGame game;
		game.run();
	}
	catch (const std::exception& e)
	{
		std::string s = "Error : ";
		s += e.what();
		MessageBoxA(nullptr, s.c_str(), "DirectX Game | Error", MB_ICONERROR | MB_OK);
	}
	
	return 0;
}
