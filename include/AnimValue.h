#pragma once

#include <list>

#include "KeyValues/KeyValue.h"

class AnimControl;

class AValue
{
private:
    struct KeyFrame
    {
        double Time;
        KeyValue* Value;
    };

    AnimControl*        m_animControl;

protected:
    std::list<KeyFrame> m_keyFrames;

    KeyValue*           m_selectedValue;
    KeyValue*           m_selectedAnimValue;

public:
    AValue(AnimControl* a_animControl);
    virtual ~AValue();

    void SelectKeyFrame(double a_time);
    void UpdateAnimValue(double a_time);
};

template<typename T>
class AnimValue : public AValue
{
private:

protected:

public:
    AnimValue(AnimControl* a_animControl) :
        AValue(a_animControl)
    {
        KeyFrame frame;

        frame.Time = 0;
        frame.Value = new T();

        m_selectedAnimValue = frame.Value;
        m_selectedValue = frame.Value;

        m_keyFrames.emplace_back(frame);
    }

    void AddKeyFrame(double a_time)
    {
        KeyFrame frame;

        frame.Time = a_time;

        auto prevIter = m_keyFrames.begin();
        for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
        {
            if (iter->Time < a_time)
            {
                prevIter = iter;
            }
            else if (iter->Time == a_time)
            {
                return;
            }
            else
            {
                frame.Value = new T(*prevIter->Value);
                
                frame.Value->SetNextKeyValue(iter->Value);
                frame.Value->SetPrevKeyValue(prevIter->Value);

                m_keyFrames.emplace(iter, frame);

                return;
            }
        }

        frame.Value = new T(*prevIter->Value);

        frame.Value->SetPrevKeyValue(prevIter->Value);
        frame.Value->SetNextKeyValue(nullptr);

        m_keyFrames.emplace_back(frame);
    }
    void RemoveKeyFrame(double a_time)
    {
        if (a_time == 0)
        {
            return;
        }

        auto prevIter = m_keyFrames.end();
        for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
        {
            if (iter->Time == a_time)
            {
                auto nextIter = iter;
                ++nextIter;

                if (nextIter != m_keyFrames.end())
                {
                    if (prevIter != m_keyFrames.end())
                    {
                        nextIter->Value->SetPrevKeyValue(prevIter->Value);
                    }
                    else
                    {
                        nextIter->Value->SetPrevKeyValue(nullptr);
                    }
                }
                else if (prevIter != m_keyFrames.end())
                {
                    prevIter->Value->SetNextKeyValue(nullptr);
                }
                
                if (m_selectedAnimValue == iter->Value)
                {
                    m_selectedAnimValue = nullptr;
                }
                if (m_selectedValue == iter->Value)
                {
                    m_selectedValue = nullptr;
                }

                delete iter->Value;

                m_keyFrames.erase(iter);

                return;
            }

            prevIter = iter;
        }
    }

    std::list<double> GetKeyFrames() const
    {
        std::list<double> keyFrames;

        for (auto iter = ++m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
        {
            keyFrames.emplace_back(iter->Time);
        }

        return keyFrames;
    }

    T* GetValue() const
    {
        return (T*)m_selectedValue;
    }
    T* GetAnimValue() const
    {
        return (T*)m_selectedAnimValue;
    }
};