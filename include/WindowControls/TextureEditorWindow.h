#pragma once

#include <glm/glm.hpp>

class TextureEditor;

enum class e_TriangulationMode;

class TextureEditorWindow
{
private:
    const static char* ITEMS[];

    TextureEditor*      m_textureEditor;

    const char*         m_selectedMode;
    e_TriangulationMode m_triangulationMode;

    glm::ivec2          m_vSize;
    glm::ivec2          m_texStep;

    glm::ivec2          m_stepQuad;

    float               m_channelDiff;
    float               m_alphaThreshold;
protected:

public:
    TextureEditorWindow(TextureEditor* a_textureEditor);
    ~TextureEditorWindow();

    void Update();
    void UpdatePropertiesWindow();

    e_TriangulationMode GetTriangulationMode() const;

    glm::ivec2 GetVoronoiSize() const;
    glm::ivec2 GetTextureStep() const;

    float GetChannelDifference() const;

    float GetAlphaThreshold() const;

    glm::ivec2 GetQuadStep() const;
};