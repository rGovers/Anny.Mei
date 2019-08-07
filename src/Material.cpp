#include "Material.h"

#include <glad/glad.h>

#include "ShaderProgram.h"
#include "Texture.h"

Material::Material(const ShaderProgram* a_shaderProgram) :
    m_shaderProgram(a_shaderProgram)
{

}
Material::~Material()
{

}

void Material::AddTexture(const char* a_textureName, const Texture* a_texture)
{
    m_textures.emplace(a_textureName, a_texture);
}
void Material::RemoveTexture(const char* a_textureName)
{
    m_textures.erase(a_textureName);
}

void Material::Bind() const
{
    const unsigned int shaderProgram = m_shaderProgram->GetHandle();

    glUseProgram(shaderProgram);

    int index = 0;

    for (auto iter = m_textures.begin(); iter != m_textures.end(); ++iter, ++index)
    {
        const int location = glGetUniformLocation(shaderProgram, iter->first);
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, iter->second->GetHandle());
        glUniform1i(location, index);
    }
}