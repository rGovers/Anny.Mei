#pragma once

#include <list>
#include <string>
#include <string.h>

#include "FileUtils.h"
#include "KeyValues/KeyValue.h"
#include "miniz.h"
#include "PropertyFile.h"

class AnimControl;

class AValue
{
private:
    struct KeyFrame
    {
        double Time;
        KeyValue* Value;
    };

    AnimControl*  m_animControl;

protected:
    char*               m_name;

    bool                m_display;

    std::list<KeyFrame> m_keyFrames;

    KeyValue*           m_selectedValue;
    KeyValue*           m_selectedAnimValue;

public:
    AValue(const char* a_name, AnimControl* a_animControl);
    virtual ~AValue();

    void SelectKeyFrame(double a_time);
    void UpdateAnimValue(double a_time);

    void SetDisplayState(bool a_value);
    bool IsDisplayed() const;

    const char* GetName() const;

    virtual void AddKeyFrame(double a_time) { };
    virtual void RemoveKeyFrame(double a_time) { };

    std::list<double> GetKeyFrames() const;

    virtual void LoadValues(mz_zip_archive& a_archive) { };
    void SaveValues(mz_zip_archive& a_archive) const;
};

template<typename T>
class AnimValue : public AValue
{
private:

protected:

public:
    AnimValue(const char* a_name, AnimControl* a_animControl) :
        AValue(a_name, a_animControl)
    {
        KeyFrame frame;

        frame.Time = 0;
        frame.Value = new T();

        m_selectedAnimValue = nullptr;
        m_selectedValue = nullptr;

        m_keyFrames.emplace_back(frame);
    }

    virtual void AddKeyFrame(double a_time)
    {
        if (a_time == 0)
        {
            return;
        }

        KeyFrame frame;

        frame.Time = a_time;

        auto prevIter = m_keyFrames.begin();
        for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
        {
            if (iter->Time < a_time)
            {
                prevIter = iter;
            }
            // No need to add keyframe it already exists
            else if (iter->Time == a_time)
            {
                return;
            }
            else
            {
                frame.Value = new T(*(T*)prevIter->Value);
                
                frame.Value->SetNextKeyValue(iter->Value);
                frame.Value->SetPrevKeyValue(prevIter->Value);

                m_keyFrames.emplace(iter, frame);

                return;
            }
        }

        frame.Value = new T(*(T*)prevIter->Value);

        frame.Value->SetPrevKeyValue(prevIter->Value);
        frame.Value->SetNextKeyValue(nullptr);

        m_keyFrames.emplace_back(frame);
    }
    virtual void RemoveKeyFrame(double a_time)
    {
        if (a_time == 0)
        {
            return;
        }

        auto removeIter = m_keyFrames.end();
        // I want to ensure that there is 1 value and reserve the zero index so I skip the first
        for (auto iter = ++m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
        {
            if (iter->Time <= a_time)
            {
                removeIter = iter;
            }
        }

        if (removeIter != m_keyFrames.end())
        {
            auto nextIter = removeIter;
            ++nextIter;
            auto prevIter = removeIter;
            --prevIter;

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
                
            if (m_selectedAnimValue == removeIter->Value)
            {
                m_selectedAnimValue = nullptr;
            }
            if (m_selectedValue == removeIter->Value)
            {
                m_selectedValue = nullptr;
            }

            delete removeIter->Value;

            m_keyFrames.erase(removeIter);
        }
    }

    void Rename(const char* a_name)
    {
        if (m_name != nullptr)
        {
            delete[] m_name;
            m_name = nullptr;
        }

        if (a_name != nullptr)
        {
            const size_t len = strlen(a_name);
            m_name = new char[len + 1];
            strcpy(m_name, a_name);
        }
    }

    T* GetValue() const
    {
        return (T*)m_selectedValue;
    }
    T* GetAnimValue() const
    {
        return (T*)m_selectedAnimValue;
    }

    virtual void LoadValues(mz_zip_archive& a_archive)
    {
        char* data = ExtractFileFromArchive((std::string("anim/") + m_name + ".prop").c_str(), a_archive);

        if (data != nullptr)
        {
            PropertyFile* propertyFile = new PropertyFile(data);

            const std::list<PropertyFileProperty*> properties = propertyFile->GetBaseProperties();

            for (auto iter = properties.begin(); iter != properties.end(); ++iter)
            {
                KeyFrame frame;
                frame.Time = -1;
                frame.Value = nullptr;

                for (auto valIter = (*iter)->Values().begin(); valIter != (*iter)->Values().end(); ++valIter)
                {
                    IFSETTOATTVALD(valIter->Name, "time", frame.Time, valIter->Value)
                    else if (strcmp(valIter->Name, "value") == 0)
                    {
                        if (frame.Value == nullptr)
                        {
                            frame.Value = new T();
                            frame.Value->Parse(valIter->Value);
                        }
                    }
                }

                if (frame.Value != nullptr)
                {
                    if (frame.Time != -1)
                    {
                        bool found = false;

                        auto prevIter = m_keyFrames.begin();
                        for (auto iIter = m_keyFrames.begin(); iIter != m_keyFrames.end(); ++iIter)
                        {
                            if (iIter->Time < frame.Time)
                            {
                                prevIter = iIter;
                            }
                            else if (iIter->Time == frame.Time)
                            {
                                delete iIter->Value;
                                iIter->Value = frame.Value;

                                found = true;

                                break;
                            }
                            else
                            {
                                found = true;

                                frame.Value->SetNextKeyValue(iIter->Value);
                                frame.Value->SetPrevKeyValue(prevIter->Value);

                                m_keyFrames.emplace(iIter, frame);

                                break;
                            }
                        }

                        if (!found)
                        {
                            frame.Value->SetPrevKeyValue(prevIter->Value);
                            frame.Value->SetNextKeyValue(nullptr);

                            m_keyFrames.emplace_back(frame);
                        }
                    }
                    else
                    {
                        delete frame.Value;
                    }
                }
            }

            delete propertyFile;

            mz_free(data);
        }
    }
};