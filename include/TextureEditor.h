#pragma once

#include <istream>
#include <vector>

#include "DataStore.h"
#include "FileLoaders/ImageLoader.h"
#include "miniz.h"

class ModelEditor;
class ModelPreview;
class PropertyFileProperty;
class SkeletonController;
class Texture;

struct ModelVertex;

struct LayerTexture
{
    unsigned char* Data;
    LayerMeta* Meta;
};



class TextureEditor
{
private:
    enum class e_TriangulationMode
    {
        Alpha,
        Quad,
        Outline
    };

    const static char* ITEMS[];

    std::vector<LayerTexture>* m_layers;

    e_TriangulationMode        m_triangulationMode;

    int                        m_selectedIndex;
    
    float                      m_alphaThreshold;
    int                        m_vSize[2];
    unsigned int               m_texStep[2];

    const char*                m_selectedMode;

    unsigned int               m_stepXY[2];   

    float                      m_channelDiff;

    Texture* GenerateTexture(LayerTexture& a_layerTexture) const;
    
    void GetImageData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    void SaveImageData(mz_zip_archive& a_archive) const;
protected:

public:
    TextureEditor();
    ~TextureEditor();

    void Update(double a_delta, ModelEditor* a_modelEditor);

    void LoadTexture(const char* a_path);

    unsigned int GetLayerCount() const;

    LayerMeta GetLayerMeta(unsigned int a_index) const;

    static TextureEditor* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;
};