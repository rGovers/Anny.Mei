#pragma once

class DataStore;
class Material;
class Model;
class RenderTexture;
class ShaderProgram;
class Texture;

class ModelPreview
{
private:
    DataStore*     m_dataStore;

    RenderTexture* m_renderTexture;
    
    const char*    m_textureName;
    const char*    m_modelName;

    Material*      m_material;

    bool           m_solid;
    bool           m_wireframe;

    static unsigned int SHADER_PROGRAM_REF;
    static ShaderProgram* STANDARD_SHADER_PROGRAM;
    static ShaderProgram* WIREFRAME_SHADER_PROGRAM;
protected:

public:
    ModelPreview() = delete;
    ModelPreview(const char* a_textureName, const char* a_modelName, DataStore* a_dataStore);
    ~ModelPreview();

    RenderTexture* GetRenderTexture() const;

    void Update();
    void Render() const; 
};