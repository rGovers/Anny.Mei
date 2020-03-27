#include "AnimControl.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "AnimValue.h"
#include "ColorTheme.h"
#include "FileLoaders/PropertyFile.h"
#include "imgui.h"
#include "IntermediateRenderer.h"
#include "Object.h"
#include "RenderTexture.h"
#include "WindowControls/AnimControlWindow.h"

AnimControl::AnimControl()
{
    m_window = new AnimControlWindow(this);

    m_imRenderer = new IntermediateRenderer();

    m_renderTexture = new RenderTexture(2048, 2048, GL_RGB);

    m_timer = 0;
}
AnimControl::~AnimControl()
{
    delete m_window;

    delete m_imRenderer;
}

void AnimControl::ResetTime()
{
    m_timer = 0.0;
}
void AnimControl::Update(double a_delta)
{
    const float selectedTime = m_window->GetSelectedTime();
    const float maxValue = m_window->GetMaxTimeValue();

    m_window->Update();
    
    m_timer += a_delta;
    if (m_timer >= maxValue)
    {
        m_timer -= maxValue;
    }

    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        (*iter)->SelectKeyFrame(selectedTime);
        (*iter)->UpdateAnimValue(m_timer);
    }
}

float AnimControl::GetMaxTime() const
{
    return m_window->GetMaxTimeValue();
}

const Texture* AnimControl::DrawTimeline(int& a_height)
{
    m_imRenderer->Reset();

    const float selectedTime = m_window->GetSelectedTime();
    const float maxValue = m_window->GetMaxTimeValue();

    unsigned int size = 0;

    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        if ((*iter)->IsDisplayed())
        {
            ++size;
        }
    }

    a_height = (int)size;

    unsigned int index = 0;

    const glm::mat4 orth = glm::ortho(0.0f, maxValue, 0.0f, (float)size, -1.0f, 1.0f);
    const glm::mat4 trans = glm::mat4(1);
    const glm::mat4 view = glm::inverse(trans);

    const glm::mat4 viewProj = view * orth;

    ImGui::BeginGroup();
    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        if ((*iter)->IsDisplayed())
        {
            const std::list<double> keyFrames = (*iter)->GetKeyFrames();

            bool removable = false;

            for (auto keyIter = keyFrames.begin(); keyIter != keyFrames.end(); ++keyIter)
            {
                const glm::vec4 pos = viewProj * glm::vec4(*keyIter, index + 0.5f, 0, 1);

                m_imRenderer->DrawSolidCircle(pos, 10, 0.5f, INACTIVE_COLOR, 1.0f / (maxValue * 25.0f), 1.0f / size);

                if (*keyIter <= selectedTime)
                {
                    removable = true;
                }
            }

            if (selectedTime != 0)
            {
                ImGui::PushID(index);

                if (ImGui::Button("+"))
                {
                    (*iter)->AddKeyFrame(selectedTime);
                }
                ImGui::SameLine();

                if (removable)
                {
                    if (ImGui::Button("-"))
                    {
                        (*iter)->RemoveKeyFrame(selectedTime);
                    }

                    ImGui::SameLine();
                }
                ImGui::PopID();
            }
            else
            {
                ImGui::AlignTextToFramePadding();
            }

            ImGui::SetCursorPosX(50);

            ImGui::Text((*iter)->GetName());

            ++index;
        }
    }
    ImGui::EndGroup();

    ImGui::SameLine();

    const glm::vec4 linePosA = viewProj * glm::vec4(selectedTime, size, 0, 1);
    const glm::vec4 linePosB = viewProj * glm::vec4(selectedTime, 0, 0, 1);

    m_imRenderer->DrawLine(linePosA, linePosB, 0.05f / maxValue, ACTIVE_COLOR);

    m_renderTexture->Bind();

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_imRenderer->Draw();

    m_renderTexture->Unbind();

    return m_renderTexture->GetTexture();
}

void AnimControl::AddValue(AValue* a_value)
{
    m_animatedObjects.emplace_back(a_value);
}
void AnimControl::RemoveValue(AValue* a_value)
{
    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        if (*iter == a_value)
        {
            m_animatedObjects.erase(iter);

            return;
        }
    }
}

void AnimControl::LoadValues(mz_zip_archive& a_archive)
{
    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        (*iter)->LoadValues(a_archive);
    }

    char* data = ExtractFileFromArchive("animation.prop", a_archive, true);

    if (data != nullptr)
    {
        PropertyFile* propertyFile = new PropertyFile(data);

        const std::list<PropertyFileProperty*> properties = propertyFile->GetBaseProperties();

        for (auto iter = properties.begin(); iter != properties.end(); ++iter)
        {
            if (strcmp((*iter)->GetName(), "maxtime") == 0)
            {
                const std::list<PropertyFileValue> values = (*iter)->Values();

                for (auto iIter = values.begin(); iIter != values.end(); ++iIter)
                {
                    if (strcmp(iIter->Name, "value") == 0)
                    {
                        m_window->SetMaxTimeValue(std::stof(iIter->Value));
                    }
                }
            }
        }

        mz_free(data);
    }
}
void AnimControl::SaveValues(mz_zip_archive& a_archive) const
{
    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        (*iter)->SaveValues(a_archive);
    }

    PropertyFile* propertyFile = new PropertyFile();
    PropertyFileProperty* property = propertyFile->InsertProperty();
    property->SetName("maxtime");
    property->EmplaceValue("value", std::to_string(m_window->GetMaxTimeValue()).c_str());

    const char* data = propertyFile->ToString();

    mz_zip_writer_add_mem(&a_archive, "animation.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);

    delete[] data;
}