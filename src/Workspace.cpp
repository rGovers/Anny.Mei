#include "Workspace.h"

#include "imgui.h"
#include "ModelEditor.h"
#include "SkeletonEditor.h"
#include "TextureEditor.h"

Workspace::Workspace()
{
    m_textureEditor = nullptr;
    m_modelEditor = nullptr;
    m_skeletonEditor = nullptr;

    m_selectedEditor = nullptr;
}
Workspace::~Workspace()
{
    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
    }
    if (m_textureEditor != nullptr)
    {
        delete m_textureEditor;
    }
    if (m_skeletonEditor != nullptr)
    {
        delete m_skeletonEditor;
    }
}

void Workspace::Init()
{
    m_selectedEditor = nullptr;

    if (m_textureEditor != nullptr)
    {
        delete m_textureEditor;
    }
    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
    }
    if (m_skeletonEditor != nullptr)
    {
        delete m_skeletonEditor;
    }

    m_textureEditor = new TextureEditor(this);
    m_modelEditor = new ModelEditor(this);
    m_skeletonEditor = new SkeletonEditor(this);
}

void Workspace::Open(mz_zip_archive& a_zip)
{
    m_selectedEditor = nullptr;

    if (m_textureEditor != nullptr)
    {
        delete m_textureEditor;
    }
    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
    }
    if (m_skeletonEditor != nullptr)
    {
        delete m_skeletonEditor;
    }

    m_textureEditor = TextureEditor::Load(a_zip, this);
    m_modelEditor = ModelEditor::Load(a_zip, this);
    m_skeletonEditor = SkeletonEditor::Load(a_zip, this);
}
void Workspace::Save(mz_zip_archive& a_zip) const
{
    m_textureEditor->Save(a_zip);
    m_modelEditor->Save(a_zip);
    m_skeletonEditor->Save(a_zip);
}

void Workspace::Update(double a_delta)
{
    if (m_textureEditor != nullptr)
    {
        m_textureEditor->Update(a_delta);
    }
    if (m_modelEditor != nullptr)
    {
        m_modelEditor->Update(a_delta);
    }
    if (m_skeletonEditor != nullptr)
    {
        m_skeletonEditor->Update(a_delta);
    }

    if (m_selectedEditor != nullptr)
    {
        if (ImGui::Begin("Editor"))
        {
            m_selectedEditor->DrawEditorWindow();
        }
        ImGui::End();

        if (ImGui::Begin("Properties"))
        {
            m_selectedEditor->DrawPropertiesWindow();
        }
        ImGui::End();
    }
}

bool Workspace::IsEnabled() const
{
    return m_textureEditor != nullptr;
}

void Workspace::SelectWorkspace(EditorController* a_editorController)
{
    m_selectedEditor = a_editorController;
}

Object* Workspace::GetBaseObject() const
{
    return m_skeletonEditor->GetBaseObject();
}

void Workspace::LoadTexture(const char* a_filepath)
{
    m_textureEditor->LoadTexture(a_filepath);
}
void Workspace::AddModel(const char* a_name, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount)
{
    m_modelEditor->AddModel(a_name, a_vertices, a_vertexCount, a_indices, a_indexCount);
}