#pragma once

#include <list>
#include <map>
#include <string>

class DepthRenderTexture;
class Model;
class MorphPlane;
class Texture;

enum class e_ModelType;

class DataStore
{
private:
    struct ModelWrap
    {
        Model* Base;
        Model* MorphPlane;
        Model* MorphTarget;

        char* TextureName;
    };

    static DataStore* Instance;

    std::map<std::string, Texture*>                       m_textures;
    std::map<std::string, ModelWrap*>                     m_models;
    std::map<std::string, MorphPlane*>                    m_morphPlanes;
    std::map<std::string, std::list<DepthRenderTexture*>> m_masks;
    std::map<std::string, std::list<DepthRenderTexture*>> m_previewMasks;
protected:

public:
    DataStore();
    ~DataStore();

    static DataStore* GetInstance();

    void AddModel(const char* a_name, Model* a_model);
    Model* GetModel(const char* a_name, e_ModelType a_modelType) const;
    void RemoveModelAll(const char* a_name);
    void RemoveModel(const char* a_name, e_ModelType a_modelType);

    void SetModelTextureName(const char* a_modelName, const char* a_textureName);
    const char* GetModelTextureName(const char* a_modelName) const;

    void AddMorphPlane(const char* a_name, MorphPlane* a_morphPlane);
    MorphPlane* GetMorphPlane(const char* a_name) const;
    void RemoveMorphPlane(const char* a_name);

    void AddTexture(const char* a_name, Texture* a_texture);
    Texture* GetTexture(const char* a_name) const;
    void RemoveTexture(const char* a_name);

    void AddMask(const char* a_name, DepthRenderTexture* a_texture);
    void AddPreviewMask(const char* a_name, DepthRenderTexture* a_texture);

    DepthRenderTexture* GetMask(const char* a_name) const;
    DepthRenderTexture* GetPreviewMask(const char* a_name) const;

    void RemoveMask(const char* a_name, DepthRenderTexture* a_texture);
    void RemovePreviewMask(const char* a_name, DepthRenderTexture* a_texture);
};