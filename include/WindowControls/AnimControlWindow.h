#pragma once

class AnimControl;

class AnimControlWindow
{
private:
    AnimControl* m_animControl;

    float        m_maxValue;
    float        m_selectedTime;
protected:

public:
    AnimControlWindow(AnimControl* a_animControl);
    ~AnimControlWindow();

    void Update();

    float GetMaxTimeValue() const;
    void SetMaxTimeValue(float a_value);

    float GetSelectedTime() const;
};