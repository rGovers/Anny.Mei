#include "AnimValue.h"

#include "AnimControl.h"
#include "KeyValues/KeyValue.h"

AValue::AValue(AnimControl* a_animControl)
{
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
    auto prevIter = m_keyFrames.begin();
    for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        if (iter->Time >= a_time)
        {
            m_selectedValue = prevIter->Value;
        }
        else
        {
            prevIter = iter;
        }
    }
}
void AValue::UpdateAnimValue(double a_time)
{
    auto prevIter = m_keyFrames.begin();
    for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        if (iter->Time >= a_time)
        {
            m_selectedAnimValue = prevIter->Value;

            if (iter != prevIter)
            {
                const double startTime = prevIter->Time;
                const double relTime = iter->Time - startTime;
                const double shitedTime = a_time - startTime;

                m_selectedAnimValue->UpdateValue(shitedTime / relTime);

                return;
            }
            else
            {
                m_selectedAnimValue->UpdateValue(0);
            }
        }
        else
        {
            prevIter = iter;
        }
    }

    m_selectedAnimValue = prevIter->Value;

    m_selectedAnimValue->UpdateValue(0);
}