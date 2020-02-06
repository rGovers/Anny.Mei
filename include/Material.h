#pragma once

#include <map>

class ShaderProgram;
class Texture;

class Material
{
private:
    const ShaderProgram*                  m_shaderProgram;

    std::map<const char*, const Texture*> m_textures;
protected:

public:
    Material() = delete;
    Material(const ShaderProgram* a_shaderProgram);
    ~Material();

    const ShaderProgram* GetShaderProgram() const;

    void AddTexture(const char* a_textureName, const Texture* a_texture);
    void RemoveTexture(const char* a_textureName);

    void Bind() const;
};