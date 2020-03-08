#pragma once

#include "miniz.h"

class EditorController;
class ModelEditor;
struct ModelVertex;
class Object;
class SkeletonEditor;
class TextureEditor;

class Workspace
{
private:
    TextureEditor*    m_textureEditor;
    ModelEditor*      m_modelEditor;
    SkeletonEditor*   m_skeletonEditor;

    EditorController* m_selectedEditor;

protected:

public:
    Workspace();
    ~Workspace();

    void Init();

    void Open(mz_zip_archive& a_zip);
    void Save(mz_zip_archive& a_zip) const;

    void Update(double a_delta);

    bool IsEnabled() const;

    void SelectWorkspace(EditorController* a_editorController);

    Object* GetBaseObject() const;

    void LoadTexture(const char* a_filepath);

    void AddModel(const char* a_name, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount);
};