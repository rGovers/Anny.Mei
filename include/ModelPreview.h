#pragma once

class Material;
class Model;
class RenderTexture;
class ShaderProgram;
class Texture;

class ModelPreview
{
private:
    RenderTexture* m_renderTexture;
    Model*         m_model;
    Material*      m_material;

    bool m_solid;
    bool m_wireframe;

    static unsigned int SHADER_PROGRAM_REF;
    static ShaderProgram* STANDARD_SHADER_PROGRAM;
    static ShaderProgram* WIREFRAME_SHADER_PROGRAM;
protected:

public:
    ModelPreview() = delete;
    ModelPreview(Texture* a_texture, Model* a_model);
    ~ModelPreview();

    RenderTexture* GetRenderTexture() const;

    Model* GetModel() const;

    void Update();
    void Render() const; 
};