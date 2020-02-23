#pragma once

#include <list>

class AValue;
class Object;

class AnimControl
{
private:
    std::list<AValue*> m_animatedObjects;

    double             m_selectedTime;
 
    double             m_timer;
    double             m_maxValue;
protected:

public:
    AnimControl();
    ~AnimControl();

    void Update(double a_delta);

    void AddValue(AValue* a_value);
    void RemoveValue(AValue* a_value);
};