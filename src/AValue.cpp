#include "AnimValue.h"

#include "AnimControl.h"
#include "KeyValues/KeyValue.h"

AValue::AValue(const char* a_name, AnimControl* a_animControl)
{
    m_name = nullptr;

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        m_name = new char[len + 1];

        strcpy(m_name, a_name);
    }

    m_animControl = a_animControl;

    m_selectedValue = nullptr;
    m_selectedAnimValue = nullptr;

    m_animControl->AddValue(this);
}
AValue::~AValue()
{
    for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        delete iter->Value;
    }

    m_animControl->RemoveValue(this);
}

void AValue::SelectKeyFrame(double a_time)
{
    for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        if (iter->Time <= a_time)
        {
            m_selectedValue = iter->Value;
        }
        else
        {
            break;
        }
    }
}
void AValue::UpdateAnimValue(double a_time)
{
    double endTime;
    auto prevIter = m_keyFrames.end();

    for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        if (iter->Time >= a_time)
        {
            m_selectedAnimValue = iter->Value;
            endTime = iter->Time;

            break;
        }
        else
        {
            prevIter = iter;
        }
    }

    if (m_selectedAnimValue != nullptr)
    {
        if (prevIter != m_keyFrames.end())
        {
            const double startTime = prevIter->Time;
            const double relTime = endTime - startTime;
            const double shitedTime = a_time - startTime;

            m_selectedAnimValue->UpdateValue(shitedTime / relTime);
        }
        else
        {
            m_selectedAnimValue->UpdateValue(0);
        }
    }
}

const char* AValue::GetName() const
{
    return m_name;
}

std::list<double> AValue::GetKeyFrames() const
{
    std::list<double> keyFrames;

    for (auto iter = ++m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        keyFrames.emplace_back(iter->Time);
    }

    return keyFrames;
}