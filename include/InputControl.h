#pragma once

#include <glm/glm.hpp>

class InputControl
{
private:
    glm::vec2 m_startDragPos;
    glm::vec2 m_endDragPos;

    glm::vec2 m_pos;
    glm::vec2 m_prevPos;

    bool      m_clicked;
    bool      m_prevState;

protected:

public:
    InputControl();
    ~InputControl();

    void Update();
    
    bool Clicked() const;
    bool Clicked(glm::vec2* a_pos) const;

    glm::vec2 DragMove() const;
    bool Dragging(glm::vec2* a_startPos, glm::vec2* a_endPos) const;
    bool DragEnd(glm::vec2* a_startPos, glm::vec2* a_endPos) const;
};