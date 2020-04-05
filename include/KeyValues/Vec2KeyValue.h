#pragma once

#include <glm/glm.hpp>

#include "KeyValue.h"

class Vec2KeyValue : public KeyValue
{
private:
    glm::vec2 m_baseValue;
    glm::vec2 m_value;

    glm::vec2 m_min;
    glm::vec2 m_max;

protected:

public:
    Vec2KeyValue();
    Vec2KeyValue(const Vec2KeyValue& a_other);

    Vec2KeyValue& operator =(const Vec2KeyValue& a_other);

    glm::vec2 GetBaseValue() const;
    glm::vec2 GetValue() const;

    void SetBaseValue(const glm::vec2& a_value);

    virtual void UpdateValue(double a_shift);

    void SetMinValue(const glm::vec2& a_value);
    glm::vec2 GetMinValue() const;

    void SetMaxValue(const glm::vec2& a_value);
    glm::vec2 GetMaxValue() const;

    virtual void UpdateGUI(const char* a_label, bool a_showLerpMode = true);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};