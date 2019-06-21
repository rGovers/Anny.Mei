#pragma once

#include <istream>
#include <list>
#include <vector>
#include <ZipLib/ZipArchive.h>

#include "FileLoaders/ImageLoader.h"

class PropertyFileProperty;

struct LayerTexture
{
    unsigned int Handle;
    unsigned char* Data;
    LayerMeta* Meta;
};

enum class e_ModelType : unsigned int
{
    Image
};

struct ModelFile
{
    e_ModelType ModelType;
    std::istream* Stream;
};

class ModelEditor
{
private:
    std::vector<LayerTexture>* m_layers;

    int                        m_selectedIndex;
    
    void GenerateTexture(LayerTexture& a_layerTexture) const;
    void GetImageData(PropertyFileProperty& a_property, ZipArchive::Ptr& a_archive);

protected:

public:
    ModelEditor();
    ~ModelEditor();

    void Update(double a_delta);

    void LoadTexture(const char* a_path);

    unsigned int GetLayerCount() const;

    LayerMeta GetLayerMeta(unsigned int a_index) const;

    static ModelEditor* Load(ZipArchive::Ptr& a_archive);
    std::istream* SaveToStream() const;
    std::list<ModelFile> SaveLayer(unsigned int a_index) const;
};