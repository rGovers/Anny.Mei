#pragma once

#include <istream>
#include <list>
#include <vector>
#include <ZipLib/ZipArchive.h>

#include "FileLoaders/ImageLoader.h"

class PropertyFileProperty;
class Texture;

struct LayerTexture
{
    Texture* TextureData;
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

class TextureEditor
{
private:
    std::vector<LayerTexture>* m_layers;

    int                        m_selectedIndex;
    
    unsigned int               m_stepXY[2];   

    void GenerateTexture(LayerTexture& a_layerTexture) const;
    void GetImageData(PropertyFileProperty& a_property, ZipArchive::Ptr& a_archive);

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
    std::list<ModelFile> SaveLayer(unsigned int a_index) const;
};