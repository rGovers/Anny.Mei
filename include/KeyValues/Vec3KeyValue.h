#pragma once

#include <glm/glm.hpp>

#include "KeyValue.h"

class Vec3KeyValue : public KeyValue
{
private:
    glm::vec3 m_baseValue;
    glm::vec3 m_value;

    glm::vec3 m_min;
    glm::vec3 m_max;

protected:

public:
    Vec3KeyValue();
    Vec3KeyValue(const Vec3KeyValue& a_other);

    Vec3KeyValue& operator =(const Vec3KeyValue& a_other);

    glm::vec3 GetBaseValue() const;
    glm::vec3 GetValue() const;

    void SetBaseValue(const glm::vec3& a_value);

    virtual void UpdateValue(double a_shift);

    void SetMinValue(const glm::vec3& a_value);
    glm::vec3 GetMinValue() const;

    void SetMaxValue(const glm::vec3& a_value);
    glm::vec3 GetMaxValue() const;

    virtual void UpdateGUI(const char* a_label, bool a_showLerpMode = true);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};