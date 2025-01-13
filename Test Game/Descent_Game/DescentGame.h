#pragma once

#include "../All.h"

class DescentGame : public Game
{
public:
    DescentGame() = default;
    ~DescentGame() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;
    void onQuit() override;
    
private:
    bool m_input = true;
};
