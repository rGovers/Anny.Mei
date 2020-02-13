#pragma once

#include <map>
#include <string>

enum class e_ModelType;
class Model;
class Texture;

class DataStore
{
private:
    struct ModelWrap
    {
        Model* Base;
        Model* MorphPlane;
        char* TextureName;
    };

    static DataStore* Instance;

    std::map<std::string, Texture*>   m_textures;
    std::map<std::string, ModelWrap*> m_models;

protected:

public:
    DataStore();
    ~DataStore();

    static DataStore* GetInstance();

    void AddModel(const char* a_name, Model* a_model);
    Model* GetModel(const char* a_name, e_ModelType a_modelType) const;
    void RemoveModelAll(const char* a_name);
    void RemoteModel(const char* a_name, e_ModelType a_modelType);

    void SetModelTextureName(const char* a_modelName, const char* a_textureName);
    const char* GetModelTextureName(const char* a_modelName) const;

    void AddTexture(const char* a_name, Texture* a_texture);
    Texture* GetTexture(const char* a_name) const;
    void RemoveTexture(const char* a_name);
};