#include "KeyValues/KeyValue.h"

KeyValue::KeyValue()
{
    m_nextKeyValue = nullptr;
    m_prevKeyValue = nullptr;
}
KeyValue::~KeyValue()
{
    
}

void KeyValue::SetNextKeyValue(KeyValue* a_keyValue)
{
    m_nextKeyValue = a_keyValue;
    m_nextKeyValue->m_prevKeyValue = this;
}
void KeyValue::SetPrevKeyValue(KeyValue* a_keyValue)
{
    m_prevKeyValue = a_keyValue;
    m_prevKeyValue->m_nextKeyValue = this;
}

KeyValue* KeyValue::GetNextKeyValue() const
{
    return m_nextKeyValue;
}
KeyValue* KeyValue::GetPrevKeyValue() const
{
    return m_prevKeyValue;
}