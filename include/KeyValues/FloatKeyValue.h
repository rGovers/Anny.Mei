#pragma once

#include "KeyValues/KeyValue.h"

class FloatKeyValue : public KeyValue
{
private:
    float m_baseValue;
    float m_value;

    float m_min;
    float m_max;
    
protected:

public:
    FloatKeyValue();
    FloatKeyValue(const FloatKeyValue& a_other);

    FloatKeyValue& operator =(const FloatKeyValue& a_other);

    float GetBaseValue() const;
    float GetValue() const;

    void SetBaseValue(float a_value);

    virtual void UpdateValue(double a_shift);

    void SetMinLimit(float a_value);
    float GetMinLimit() const;

    void SetMaxLimit(float a_value);
    float GetMaxLimit() const;

    virtual void DisplayGUI(const char* a_label, bool a_showLerpMode = true);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};