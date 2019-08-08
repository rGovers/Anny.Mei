#pragma once

#include <map>

#include "ZipLib/ZipFile.h"

class Model;
class Object;

class SkeletonController
{
private:
    std::map<int, Model*> m_models;

    Object*               m_baseObject;

    Object*               m_selectedObject;

    void ListObjects(Object* a_object, float a_offset);

protected:

public:
    SkeletonController();
    ~SkeletonController();

    void Update(double a_delta);

    void SetModel(int a_index, Model* a_model);

    static SkeletonController* Load(ZipArchive::Ptr a_archive);
};