#pragma once

#include <istream>
#include <map>

#include "Models/Model.h"
#include "ZipLib/ZipFile.h"

class Object;

struct ModelData
{
    unsigned int* Indices;
    ModelVertex* Vertices;
    unsigned int IndexCount;
    unsigned int VertexCount;
    Model* GModel;
};

class SkeletonController
{
private:
    std::map<const char*, ModelData> m_models;

    Object*                          m_baseObject;
    
    Object*                          m_selectedObject;

    void ListObjects(Object* a_object, int& a_node);

protected:

public:
    SkeletonController();
    ~SkeletonController();

    void Update(double a_delta);

    void SetModel(const char* a_name, ModelData a_modelData);

    static SkeletonController* Load(ZipArchive::Ptr a_archive);

    int GetModelCount() const;

    std::istream* SaveToStream() const;
    const char* GetModelName(int a_index) const;
    std::istream* SaveModel(const char* a_name) const;
    std::istream* SaveModel(int a_index) const;
};