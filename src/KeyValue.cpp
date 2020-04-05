#include "KeyValues/KeyValue.h"

#include <string.h>

#include "imgui.h"

const char* KeyValue::ITEMS[] = { "Lerp", "Slerp", "Set" };

int KeyValue::ID = 0;

KeyValue::KeyValue()
{
    m_nextKeyValue = nullptr;
    m_prevKeyValue = nullptr;

    m_lerpMode = e_LerpMode::Lerp;
}
KeyValue::~KeyValue()
{
    
}

void KeyValue::SetNextKeyValue(KeyValue* a_keyValue)
{
    m_nextKeyValue = a_keyValue;
    if (a_keyValue != nullptr)
    {
        a_keyValue->m_prevKeyValue = this;
    }
}
void KeyValue::SetPrevKeyValue(KeyValue* a_keyValue)
{
    m_prevKeyValue = a_keyValue;
    if (a_keyValue != nullptr)
    {
        a_keyValue->m_nextKeyValue = this;
    }
}

void KeyValue::UpdateLerpGUI()
{
    m_selectedMode = ITEMS[(int)m_lerpMode];
    ImGui::PushID(ID++);
    if (ImGui::BeginCombo("Lerp Mode", m_selectedMode))
    {
        for (int i = 0; i < IM_ARRAYSIZE(ITEMS); ++i)
        {
            // Using bitshifting because it is some much easier for iterable values
            if ((m_showLerpMode & (0b1 << i)) != 0)
            {
                bool is_selected = (m_selectedMode == ITEMS[i]); 
                if (ImGui::Selectable(ITEMS[i], is_selected))
                {
                    m_selectedMode = ITEMS[i];
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }

        ImGui::EndCombo();
    }
    ImGui::PopID();
        
    if (strcmp(m_selectedMode, "Lerp") == 0)
    {
        m_lerpMode = e_LerpMode::Lerp;
    }
    else if (strcmp(m_selectedMode, "Slerp") == 0)
    {
        m_lerpMode = e_LerpMode::Slerp;
    }
    else
    {
        m_lerpMode = e_LerpMode::Set;
    }
}

bool KeyValue::GetDisplayLerp() const
{
    return (m_showLerpMode & (0b1 << 0)) != 0;
}
void KeyValue::SetDisplayLerp(bool a_value)
{
    m_showLerpMode |= (0b1 << 0);

    if (!a_value)
    {
        m_showLerpMode ^= (0b1 << 0);
    }    
}

bool KeyValue::GetDisplaySlerp() const
{
    return (m_showLerpMode & (0b1 << 1)) != 0;
}
void KeyValue::SetDisplaySlerp(bool a_value)
{
    m_showLerpMode |= (0b1 << 1);

    if (!a_value)
    {
        m_showLerpMode ^= (0b1 << 1);
    } 
}

bool KeyValue::GetDisplaySet() const
{
    return (m_showLerpMode & (0b1 << 2)) != 0;
}
void KeyValue::SetDisplaySet(bool a_value)
{
    m_showLerpMode |= (0b1 << 2);

    if (!a_value)
    {
        m_showLerpMode ^= (0b1 << 2);
    } 
}

void KeyValue::SetLerpMode(e_LerpMode a_lerpMode)
{
    m_lerpMode = a_lerpMode;
}
e_LerpMode KeyValue::GetLerpMode() const
{
    return m_lerpMode;
}

KeyValue* KeyValue::GetNextKeyValue() const
{
    return m_nextKeyValue;
}
KeyValue* KeyValue::GetPrevKeyValue() const
{
    return m_prevKeyValue;
}
void KeyValue::ResetGUI()
{
    ID = 0;
}