#pragma once

#include <istream>
#include <vector>

#include "DataStore.h"
#include "FileLoaders/ImageLoader.h"
#include "miniz.h"

class ModelPreview;
class PropertyFileProperty;
class SkeletonController;
class Texture;

struct ModelVertex;

struct LayerTexture
{
    ModelPreview* ModelData;
    unsigned char* Data;
    LayerMeta* Meta;
    ModelVertex* Vertices;
    unsigned int* Indices;
};

class TextureEditor
{
private:
    std::vector<LayerTexture>* m_layers;
    DataStore*                 m_dataStore;

    int                        m_selectedIndex;
    
    unsigned int               m_stepXY[2];   
    int                        m_vSize[2];

    Texture* GenerateTexture(LayerTexture& a_layerTexture) const;
    
    void GetImageData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);
    void GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    void SaveImageData(mz_zip_archive& a_archive) const;
    void SaveModelData(mz_zip_archive& a_archive) const;
protected:

public:
    TextureEditor(DataStore* a_dataStore);
    ~TextureEditor();

    void Update(double a_delta);

    void LoadTexture(const char* a_path);

    unsigned int GetLayerCount() const;

    LayerMeta GetLayerMeta(unsigned int a_index) const;

    static TextureEditor* Load(mz_zip_archive& a_archive, DataStore* a_dataStore);
    void Save(mz_zip_archive& a_archive) const;
};