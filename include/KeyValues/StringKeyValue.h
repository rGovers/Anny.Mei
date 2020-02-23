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

    const char* GetString() const;

    void SetString(const char* a_value);

    virtual void UpdateValue(double a_shift);
};