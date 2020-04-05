#pragma once

#include "KeyValues/KeyValue.h"

class IntKeyValue : public KeyValue
{
private:
    int m_baseValue;
    int m_value;

    int m_min;
    int m_max;

protected:

public:
    IntKeyValue();
    IntKeyValue(const IntKeyValue& a_other);
    virtual ~IntKeyValue();

    IntKeyValue& operator =(const IntKeyValue& a_other);

    int GetBaseValue() const;
    int GetValue() const;

    void SetBaseValue(int a_value);

    virtual void UpdateValue(double a_shift);

    void SetMinLimit(int a_value);
    int GetMinLimit() const;

    void SetMaxLimit(int a_value);
    int GetMaxLimit() const;

    virtual void UpdateGUI(const char* a_label, bool a_showLerpMode = true);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};