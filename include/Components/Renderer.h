#pragma once

#include "Component.h"

class Renderer : public Component
{
private:

protected:

public:
    Renderer(Object* a_object);
    virtual ~Renderer();

    virtual void Update(double a_delta) = 0;
    virtual void UpdatePreview(double a_delta) = 0;
    virtual void UpdateGUI() = 0;

    virtual const char* ComponentName() const;
};