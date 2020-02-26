#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include "KeyValue.h"

class QuatKeyValue : public KeyValue
{
private:
    glm::quat m_baseValue;
    glm::quat m_value;

protected:

public:
    QuatKeyValue();
    QuatKeyValue(const QuatKeyValue& a_other);

    QuatKeyValue& operator =(const QuatKeyValue& a_other);

    glm::quat GetBaseValue() const;
    glm::quat GetValue() const;

    void SetBaseValue(const glm::quat& a_value);

    virtual void UpdateValue(double a_shift);

    virtual char* ToString() const;
    virtual void Parse(const char* a_data);
};