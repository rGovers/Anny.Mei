#pragma once

#include "KeyValues/KeyValue.h"

class FloatKeyValue : public KeyValue
{
private:
    float m_baseValue;
    float m_value;
    
protected:

public:
    FloatKeyValue();
    FloatKeyValue(const FloatKeyValue& a_other);

    FloatKeyValue& operator =(const FloatKeyValue& a_other);

    float GetBaseValue() const;
    float GetValue() const;

    void SetBaseValue(float a_value);

    virtual void UpdateValue(double a_shift);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};