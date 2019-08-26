#pragma once

#include <istream>
#include <vector>
#include <ZipLib/ZipArchive.h>

#include "FileLoaders/ImageLoader.h"

class ModelPreview;
class PropertyFileProperty;
class SkeletonController;
class Texture;

struct ModelVertex;

struct LayerTexture
{
    bool Update;
    Texture* TextureData;
    ModelPreview* ModelData;
    unsigned char* Data;
    LayerMeta* Meta;
    unsigned int VertexCount;
    ModelVertex* Vertices;
    unsigned int* Indices;
};

class TextureEditor
{
private:
    std::vector<LayerTexture>* m_layers;

    int                        m_selectedIndex;
    
    unsigned int               m_stepXY[2];   
    int                        m_vSize[2];

    void GenerateTexture(LayerTexture& a_layerTexture) const;
    
    void GetImageData(PropertyFileProperty& a_property, ZipArchive::Ptr& a_archive);
    void GetModelData(PropertyFileProperty& a_property, ZipArchive::Ptr& a_archive);
protected:

public:
    TextureEditor();
    ~TextureEditor();

    void Update(double a_delta);

    void LoadTexture(const char* a_path);

    unsigned int GetLayerCount() const;

    LayerMeta GetLayerMeta(unsigned int a_index) const;

    static TextureEditor* Load(ZipArchive::Ptr& a_archive);
    std::istream* SaveToStream() const;
    std::istream* SaveLayer(unsigned int a_index) const;

    void SyncModels(SkeletonController* a_skeletonController) const;
};