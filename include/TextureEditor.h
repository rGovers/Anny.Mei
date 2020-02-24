#pragma once

#include <istream>
#include <list>

#include "DataStore.h"
#include "FileLoaders/ImageLoader.h"
#include "miniz.h"

class ModelEditor;
class ModelPreview;
class PropertyFileProperty;
class SkeletonController;
class Texture;
class TextureEditorWindow;

struct ModelVertex;

struct LayerTexture
{
    unsigned char* Data;
    LayerMeta* Meta;
};

enum class e_TriangulationMode
{
    Alpha,
    Quad,
    Outline
};

class TextureEditor
{
private:
    TextureEditorWindow*              m_window;
        
    std::list<LayerTexture>*          m_layers;
    std::list<LayerTexture>::iterator m_selectedIndex;

    ModelEditor*                      m_modelEditor;

    Texture* GenerateTexture(LayerTexture& a_layerTexture) const;
    
    void GetImageData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    void SaveImageData(mz_zip_archive& a_archive) const;
protected:

public:
    TextureEditor();
    ~TextureEditor();

    bool LayerSelected() const;

    void TriangulateClicked();

    void Update(double a_delta, ModelEditor* a_modelEditor);
    void DrawLayerGUI();

    void LoadTexture(const char* a_path);

    static TextureEditor* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;
};