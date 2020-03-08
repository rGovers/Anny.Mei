#pragma once

#include <istream>
#include <list>

#include "EditorController.h"
#include "FileLoaders/ImageLoader.h"
#include "miniz.h"

class ModelEditor;
class ModelPreview;
class PropertyFileProperty;
class SkeletonController;
class Texture;
class TextureEditorWindow;
class Workspace;

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

class TextureEditor : public EditorController
{
private:
    Workspace*                        m_workspace;

    TextureEditorWindow*              m_window;
        
    std::list<LayerTexture>*          m_layers;
    std::list<LayerTexture>::iterator m_selectedIndex;

    Texture* GenerateTexture(LayerTexture& a_layerTexture) const;
    
    void GetImageData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    void SaveImageData(mz_zip_archive& a_archive) const;
protected:

public:
    TextureEditor() = delete;
    TextureEditor(Workspace* a_workspace);
    ~TextureEditor();

    bool LayerSelected() const;

    void TriangulateClicked();

    void Update(double a_delta);
    void DrawLayerGUI();

    void LoadTexture(const char* a_path);

    static TextureEditor* Load(mz_zip_archive& a_archive, Workspace* a_workspace);
    void Save(mz_zip_archive& a_archive) const;

    virtual void DrawPropertiesWindow();
};