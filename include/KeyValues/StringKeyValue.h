#pragma once

#include "KeyValue.h"

class StringKeyValue : public KeyValue
{
private:
    char* m_value;

protected:

public:
    StringKeyValue();
    StringKeyValue(const StringKeyValue& a_other);
    virtual ~StringKeyValue();

    StringKeyValue& operator =(const StringKeyValue& a_other);

    const char* GetBaseString() const;
    const char* GetString() const;

    void SetString(const char* a_value);

    virtual void UpdateValue(double a_shift);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};