#pragma once

#include "KeyValue.h"

class BoolKeyValue : public KeyValue
{
private:
    bool m_value;

protected:

public:
    BoolKeyValue();
    BoolKeyValue(const BoolKeyValue& a_other);
    virtual ~BoolKeyValue();

    BoolKeyValue& operator =(const BoolKeyValue& a_other);

    bool GetBaseBoolean() const;
    bool GetBoolean() const;

    void SetBoolean(bool a_value);

    virtual void UpdateValue(double a_shift);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};