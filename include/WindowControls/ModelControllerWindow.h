#pragma once

#include <glm/glm.hpp>

class ModelController;

class ModelControllerWindow
{
private:
    ModelController* m_modelController;

    glm::vec3        m_backgroundColor;
protected:

public:
    ModelControllerWindow(ModelController* a_modelController);
    ~ModelControllerWindow();

    void Update();
    
    glm::vec3 GetBackgroundColor() const;
    void SetBackgroundColor(const glm::vec3& a_color);
};