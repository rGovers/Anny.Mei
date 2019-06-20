#pragma once

#include <istream>
#include <vector>
#include <ZipLib/ZipArchive.h>

#include "FileLoaders/ImageLoader.h"

struct LayerTexture
{
    unsigned int Handle;
    unsigned char* Data;
    LayerMeta* Meta;
};

class ModelEditor
{
private:
    std::vector<LayerTexture>* m_layers;

    int                        m_selectedIndex;
    
protected:

public:
    ModelEditor();
    ~ModelEditor();

    void Update(double a_delta);

    void LoadTexture(const char* a_path);

    static ModelEditor* Load(ZipArchive::Ptr& a_archive);
    std::istream* SaveToStream() const;
};