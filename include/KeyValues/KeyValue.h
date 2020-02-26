#pragma once

class KeyValue
{
private:
    KeyValue* m_prevKeyValue;
    KeyValue* m_nextKeyValue;
protected:

public:
    KeyValue();
    virtual ~KeyValue();

    void SetNextKeyValue(KeyValue* a_keyValue);
    void SetPrevKeyValue(KeyValue* a_keyValue);

    KeyValue* GetNextKeyValue() const;
    KeyValue* GetPrevKeyValue() const;

    virtual void UpdateValue(double a_shift) = 0;

    virtual char* ToString() const = 0;
    virtual void Parse(const char* a_string) = 0;
};