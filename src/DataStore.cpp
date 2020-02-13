#include "DataStore.h"

#include <string.h>

#include "Models/Model.h"
#include "Texture.h"

DataStore* DataStore::Instance = nullptr;

DataStore::DataStore()  :
    m_textures(),
    m_models()
{
    if (Instance == nullptr)
    {
        Instance = this;
    }
}
DataStore::~DataStore()
{
    for (auto iter = m_models.begin(); iter != m_models.end(); ++iter)
    {
        delete iter->second;
    }

    if (Instance == this)
    {
        Instance = nullptr;
    }
}

DataStore* DataStore::GetInstance()
{
    return Instance;
}

void DataStore::AddModel(const char* a_name, Model* a_model)
{
    auto iter = m_models.find(a_name);

    ModelWrap* modelWrap;

    if (iter != m_models.end())
    {
        modelWrap = iter->second;
    }
    else
    {
        modelWrap = new ModelWrap();

        m_models.emplace(a_name, modelWrap);
    }
    
    switch (a_model->GetModelType())
    {
    case e_ModelType::Base:
    {
        modelWrap->Base = a_model;

        break;
    }
    case e_ModelType::MorphPlane:
    {
        modelWrap->MorphPlane = a_model;

        break;
    }
    }
}
Model* DataStore::GetModel(const char* a_name, e_ModelType a_modelType) const
{
    auto iter = m_models.find(a_name);

    if (iter != m_models.end())
    {
        switch (a_modelType)
        {
        case e_ModelType::Base:
        {
            return iter->second->Base;
        }
        case e_ModelType::MorphPlane:
        {
            return iter->second->MorphPlane;
        }
        }
    }

    return nullptr;
}
void DataStore::RemoveModelAll(const char* a_name)
{
    auto iter = m_models.find(a_name);

    if (iter != m_models.end())
    {
        delete iter->second;

        m_models.erase(iter);
    }
}
void DataStore::RemoteModel(const char* a_name, e_ModelType a_modelType)
{
    auto iter = m_models.find(a_name);

    if (iter != m_models.end())
    {
        switch (a_modelType)
        {
        case e_ModelType::Base:
        {
            iter->second->Base = nullptr;

            break;
        }
        case e_ModelType::MorphPlane:
        {
            iter->second->MorphPlane = nullptr;
        }
        }
    }

    if (iter->second->Base == nullptr && iter->second->MorphPlane == nullptr)
    {
        RemoveModelAll(a_name);
    }
}

void DataStore::SetModelTextureName(const char* a_modelName, const char* a_textureName)
{
    auto iter = m_models.find(a_modelName);

    if (iter != m_models.end())
    {
        if (a_textureName == nullptr)
        {
            if (iter->second->TextureName != nullptr)
            {
                delete[] iter->second->TextureName;
                iter->second->TextureName = nullptr;
            }
            
            return;
        }

        if (a_textureName != nullptr && iter->second->TextureName != nullptr)
        {
            if (strcmp(a_textureName, iter->second->TextureName) == 0)
            {
                return;
            }

            delete[] iter->second->TextureName;
        }

        const size_t len = strlen(a_textureName);

        iter->second->TextureName = new char[len + 1] { 0 };

        memcpy(iter->second->TextureName, a_textureName, len);        
    }
}
const char* DataStore::GetModelTextureName(const char* a_modelName) const
{
    auto iter = m_models.find(a_modelName);

    if (iter != m_models.end())
    {
        return iter->second->TextureName;
    }

    return nullptr;
}

void DataStore::AddTexture(const char* a_name, Texture* a_texture)
{
    auto iter = m_textures.find(a_name);

    if (iter != m_textures.end())
    {
        iter->second = a_texture;

        return;
    }

    m_textures.emplace(a_name, a_texture);
}
Texture* DataStore::GetTexture(const char* a_name) const
{
    auto iter = m_textures.find(a_name);
    
    if (iter != m_textures.end())
    {
        return iter->second;
    }

    return nullptr;
}
void DataStore::RemoveTexture(const char* a_name)
{
    auto iter = m_textures.find(a_name);

    if (iter != m_textures.end())
    {
        m_textures.erase(iter);
    }
}