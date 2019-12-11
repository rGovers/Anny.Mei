#pragma once

#include <istream>
#include <map>

#include "miniz.h"
#include "Models/Model.h"

class Object;

struct ModelData
{
    unsigned int* Indices;
    ModelVertex* Vertices;
    unsigned int IndexCount;
    unsigned int VertexCount;
    Model* GModel;
};

class SkeletonEditor
{
private:
    Object*                          m_baseObject;
    
    Object*                          m_selectedObject;

    void ListObjects(Object* a_object, int& a_node);

protected:

public:
    SkeletonEditor();
    ~SkeletonEditor();

    void Update(double a_delta);

    static SkeletonEditor* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;
};