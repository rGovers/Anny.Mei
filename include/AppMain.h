#pragma once

#include "Application.h"

class VirtualCamera;

class AppMain : public Application
{
private:
    VirtualCamera* m_virtualCamera;
    
protected:
    virtual void Update(double a_delta);

public:
    AppMain() = delete;
    AppMain(int a_width, int a_height);
    ~AppMain();
};