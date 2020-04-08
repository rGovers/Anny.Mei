#include "AnimValue.h"

#include "AnimControl.h"

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

    m_display = false;

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
    double endTime = 0.0f;

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

            m_selectedAnimValue->UpdateValue(glm::min(shitedTime / relTime, 1.0));
        }
        else
        {
            m_selectedAnimValue->UpdateValue(0);
        }

        return;
    }

    m_selectedAnimValue = m_keyFrames.begin()->Value;

    m_selectedAnimValue->UpdateValue(0);
}

void AValue::SetDisplayState(bool a_value)
{
    m_display = a_value;
}
bool AValue::IsDisplayed() const
{
    return m_display;
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

void AValue::SaveValues(mz_zip_archive& a_archive) const
{
    PropertyFile* propertyFile = new PropertyFile();

    for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); ++iter)
    {
        PropertyFileProperty* prop = propertyFile->InsertProperty();

        prop->SetName("value");

        prop->EmplaceValue("time", std::to_string(iter->Time).c_str());

        const char* str = iter->Value->ToString();

        prop->EmplaceValue("value", str);

        prop->EmplaceValue("lerpmode", std::to_string((int)iter->Value->GetLerpMode()).c_str());

        delete[] str;
    }

    const char* data = propertyFile->ToString();

    mz_zip_writer_add_mem(&a_archive, (std::string("anim/") + m_name + ".prop").c_str(), data, strlen(data), MZ_DEFAULT_COMPRESSION);

    delete[] data;
}