#include "SkeletonController.h"

#include "imgui.h"
#include "MemoryStream.h"
#include "Models/Model.h"
#include "Object.h"
#include "PropertyFile.h"

SkeletonController::SkeletonController() :
    m_baseObject(new Object()),
    m_selectedObject(nullptr)
{
    m_baseObject->SetName("Root Object");
}

SkeletonController::~SkeletonController()
{
    delete m_baseObject;
}

void SkeletonController::ListObjects(Object* a_object, float a_offset)
{
    ImGui::SetCursorPosX(a_offset);

    const char* name = a_object->GetName();

    if (name == nullptr || name[0] == 0)
    {
        name = "NULL";
    }

    if (ImGui::Button(name))
    {
        m_selectedObject = a_object;
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        ListObjects(*iter, a_offset + 10.0f);
    }
}

void SkeletonController::Update(double a_delta)
{
    if (m_selectedObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 400 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Object Properties"))
        {
            static const int BUFFER_SIZE = 1024;

            char* buffer = new char[BUFFER_SIZE];
            buffer[0] = 0;
            
            const char* name = m_selectedObject->GetName();

            if (name != nullptr)
            {
                strcpy(buffer, m_selectedObject->GetName());
            }

            ImGui::InputText("Name", buffer, BUFFER_SIZE);

            m_selectedObject->SetName(buffer);

            if (ImGui::Button("Add Child Object"))
            {
                Object* object = new Object();
                object->SetParent(m_selectedObject);
            }

            if (m_selectedObject != m_baseObject)
            {
                if (ImGui::Button("Remove Object"))
                {
                    delete m_selectedObject;

                    m_selectedObject = nullptr;
                }
            }

            ImGui::End();
        }
    }

    ImGui::SetNextWindowSize({ 200, 600 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Skeleton Hierarchy"))
    {
        ListObjects(m_baseObject, 0.0f);

        ImGui::End();
    }
}

void SkeletonController::SetModel(const char* a_name, ModelData a_modelData)
{
    auto iter = m_models.find(a_name);

    if (iter != m_models.end())
    {
        iter->second = a_modelData;
    }
    else
    {
        m_models.emplace(a_name, a_modelData);
    }
}

SkeletonController* SkeletonController::Load(ZipArchive::Ptr a_archive)
{
    SkeletonController* skeletonController = new SkeletonController();

    return skeletonController;
}

int SkeletonController::GetModelCount() const
{
    return m_models.size();
}

std::istream* SkeletonController::SaveToStream() const
{
    if (m_models.size() > 0)
    {
        PropertyFile* propertyFile = new PropertyFile();

        for (auto iter = m_models.begin(); iter != m_models.end(); ++iter)
        {
            PropertyFileProperty* prop = propertyFile->InsertProperty();

            prop->SetName("model");

            prop->EmplaceValue("name", iter->first);
            prop->EmplaceValue("vertices", std::to_string(iter->second.VertexCount).c_str());
            prop->EmplaceValue("indices", std::to_string(iter->second.IndexCount).c_str());
        }

        char* data = propertyFile->ToString();

        IMemoryStream* memoryStream = new IMemoryStream(data, strlen(data));

        delete[] data;

        return memoryStream;
    }

    return nullptr;
}

const char* SkeletonController::GetModelName(int a_index) const
{
    auto iter = m_models.begin();

    std::advance(iter, a_index);

    return iter->first;
}

std::istream* SkeletonController::SaveModel(const char* a_name) const
{
    auto iter = m_models.find(a_name);

    if (iter != m_models.end())
    {
        const unsigned int indexSize = iter->second.IndexCount * 4;
        const unsigned int vertexSize = iter->second.VertexCount * sizeof(ModelVertex);
        const unsigned int dataSize = indexSize + vertexSize;

        char* data = new char[dataSize];
        memcpy(data, iter->second.Vertices, vertexSize);
        memcpy(data + vertexSize, iter->second.Indices, indexSize);

        IMemoryStream* memoryStream = new IMemoryStream(data, dataSize);

        return memoryStream;
    }

    return nullptr;
}

std::istream* SkeletonController::SaveModel(int a_index) const
{
    auto iter = m_models.begin();

    std::advance(iter, a_index);

    const unsigned int indexSize = iter->second.IndexCount * 4;
    const unsigned int vertexSize = iter->second.VertexCount * sizeof(ModelVertex);
    const unsigned int dataSize = indexSize + vertexSize;

    char* data = new char[dataSize];
    memcpy(data, iter->second.Vertices, vertexSize);
    memcpy(data + vertexSize, iter->second.Indices, indexSize);

    IMemoryStream* memoryStream = new IMemoryStream(data, dataSize);

    return memoryStream;
}