#pragma once

#include <glm/glm.hpp>
#include <list>

#include "miniz.h"

class AnimControlWindow;
class AValue;
class IntermediateRenderer;
class Object;
class RenderTexture;
class Texture;

class AnimControl
{
private:
    AnimControlWindow*    m_window;

    IntermediateRenderer* m_imRenderer;

    std::list<AValue*>    m_animatedObjects;
 
    RenderTexture*        m_renderTexture;

    double                m_timer;
protected:

public:
    AnimControl();
    ~AnimControl();

    void Update(double a_delta);

    const Texture* DrawTimeline(int& a_height);

    void AddValue(AValue* a_value);
    void RemoveValue(AValue* a_value);

    void LoadValues(mz_zip_archive& a_archive);
    void SaveValues(mz_zip_archive& a_archive) const;
};