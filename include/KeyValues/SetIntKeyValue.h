#pragma once

#include "KeyValues/KeyValue.h"

class SetIntKeyValue : public KeyValue
{
private:
    int m_value;

protected:

public:
    SetIntKeyValue();
    SetIntKeyValue(const SetIntKeyValue& a_other);
    virtual ~SetIntKeyValue();

    SetIntKeyValue& operator =(const SetIntKeyValue& a_other);

    int GetBaseInt() const;
    int GetInt() const;

    void SetInt(int a_value);

    virtual void UpdateValue(double a_shift);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};