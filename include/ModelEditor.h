#pragma once

#include "ZipLib/ZipFile.h"

class ModelEditor
{
private:
    ZipArchive::Ptr m_file;
protected:

public:
    ModelEditor() = delete;
    ModelEditor(const char* a_path);
    ~ModelEditor();
};