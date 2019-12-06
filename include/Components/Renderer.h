#pragma once

#include "Component.h"

class Renderer : public Component
{
private:

protected:

public:
    Renderer(Object* a_object);
    virtual ~Renderer();

    virtual void Update();
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;
};