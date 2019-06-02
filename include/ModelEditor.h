#pragma once

#include <list>
#include <vector>

#include "ImageLoader.h"

struct LayerTexture
{
    int Handle;
    Layer* LayerData;
};

class ModelEditor
{
private:
    ImageLoader*               m_imageLoader;

    std::list<LayerMeta*>*     m_layerMeta;
    std::vector<LayerTexture>* m_layers;

    int                        m_selectedIndex;
protected:

public:
    ModelEditor() = delete;
    ModelEditor(const char* a_path);
    ~ModelEditor();

    void Update(double a_delta);
};