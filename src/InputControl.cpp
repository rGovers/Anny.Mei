#include "InputControl.h"

#include <limits>

#include "imgui.h"

InputControl::InputControl()
{
    m_startDragPos = glm::vec2(-std::numeric_limits<float>::infinity());

    m_clicked = false;

    m_pos = glm::vec2(0);
    m_prevPos = glm::vec2(0);
}
InputControl::~InputControl()
{

}

void InputControl::Update()
{
    const bool mouseDown = ImGui::IsMouseDown(0);

    if (mouseDown)
    {
        const ImVec2 tPos = ImGui::GetMousePos();
        const ImVec2 tWPos = ImGui::GetWindowPos();

        const glm::vec2 mousePos = { tPos.x, tPos.y };
        const glm::vec2 winPos = { tWPos.x + 10, tWPos.y + 40 };
        const glm::vec2 relPos = mousePos - winPos;

        m_clicked = m_prevState != mouseDown;

        if (m_startDragPos.x == -std::numeric_limits<float>::infinity() || m_startDragPos.y == -std::numeric_limits<float>::infinity())
        {
            m_startDragPos = relPos;
        }

        m_endDragPos = relPos;

        m_prevPos = m_pos;
        m_pos = relPos;
    }
    else if (!m_prevState)
    {
        m_startDragPos = glm::vec2(-std::numeric_limits<float>::infinity());
    }
    
    m_prevState = mouseDown;
}

bool InputControl::Clicked() const
{
    return m_clicked;
}
bool InputControl::Clicked(glm::vec2* a_pos) const
{
    *a_pos = m_startDragPos;

    return m_clicked;
}

glm::vec2 InputControl::DragMove() const
{
    return m_prevPos - m_pos;
}

bool InputControl::Dragging(glm::vec2* a_startPos, glm::vec2* a_endPos) const
{
    *a_startPos = m_startDragPos;
    *a_endPos = m_endDragPos;

    return m_prevState && m_startDragPos.x != -std::numeric_limits<float>::infinity() && m_startDragPos.y != -std::numeric_limits<float>::infinity();
}
bool InputControl::DragEnd(glm::vec2* a_startPos, glm::vec2* a_endPos) const
{
    *a_startPos = m_startDragPos;
    *a_endPos = m_endDragPos;

    return !m_prevState && m_startDragPos.x != -std::numeric_limits<float>::infinity() && m_startDragPos.y != -std::numeric_limits<float>::infinity();
}