#pragma once

#include <glm/glm.hpp>

class ModelController;

class ModelControllerWindow
{
private:
    ModelController* m_modelController;

    glm::vec4        m_backgroundColor;
protected:

public:
    ModelControllerWindow(ModelController* a_modelController);
    ~ModelControllerWindow();

    void Update();
    
    glm::vec4 GetBackgroundColor() const;
    void SetBackgroundColor(const glm::vec4& a_color);
};