#include "FileLoaders/KritaLoader.h"

#include <string.h>

#include "FileLoaders/XMLFile.h"

KritaImage::KritaImage()
{
    m_name = nullptr;
}
KritaImage::~KritaImage()
{
    if (m_name != nullptr)
    {
        delete[] m_name;
    }

    for (auto iter = m_layers.begin(); iter != m_layers.end(); ++iter)
    {
        delete *iter;
    }
}

void KritaImage::LoadLayers(mz_zip_archive& a_archive, KritaLayer* a_layer, XMLFileProperty* a_property)
{
    const std::list<XMLFileAttribute> attributes = a_property->Attributes();

    const char* name = a_property->GetName();

    const std::list<XMLFileProperty*> children = a_property->GetChildren();

    if (strcmp(name, "layers") == 0)
    {
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            LoadLayers(a_archive, a_layer, *iter);
        }
    }
    else if (strcmp(name, "layer") == 0)
    {
        KritaLayer* layer = new KritaLayer(this);
        layer->SetParent(a_layer);

        const std::list<XMLFileAttribute> attributes = a_property->Attributes();

        for (auto iter = attributes.begin(); iter != attributes.end(); ++iter)
        {
            const char* attName = iter->Name;

            if (strcmp(attName, "name") == 0)
            {
                layer->SetName(iter->Value);
            }
            else if (strcmp(attName, "filename") == 0)
            {
                layer->SetFilename(iter->Value);
            }
            else if (strcmp(attName, "x") == 0)
            {
                layer->SetXOffset(std::stoi(iter->Value));
            }
            else if (strcmp(attName, "y") == 0)
            {
                layer->SetYOffset(std::stoi(iter->Value));
            }
            else if (strcmp(attName, "nodetype") == 0)
            {
                const char* attValue = iter->Value;

                if (strcmp(attValue, "grouplayer") == 0)
                {
                    layer->SetLayerType(e_LayerType::Group);
                }
                else if (strcmp(attValue, "paintlayer") == 0)
                {
                    layer->SetLayerType(e_LayerType::Paint);
                }
            }
        }

        if (layer->GetName() != nullptr)
        {
            m_layers.emplace_back(layer);

            switch (layer->GetLayerType())
            {
            case e_LayerType::Group:
            {
                for (auto iter = children.begin(); iter != children.end(); ++iter)
                {
                    LoadLayers(a_archive, layer, *iter);
                }

                break;   
            }
            case e_LayerType::Paint:
            {
                layer->LoadData(a_archive);

                break;
            }
            } 
        }
    }
}

const char* KritaImage::GetName() const
{
    return m_name;
}
void KritaImage::SetName(const char* a_name)
{
    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        m_name = new char[len + 1];

        strcpy(m_name, a_name);
    }
}

int KritaImage::GetWidth() const
{
    return m_width;
}
void KritaImage::SetWidth(int a_width)
{
    m_width = a_width;
}

int KritaImage::GetHeight() const
{
    return m_height;
}
void KritaImage::SetHeight(int a_height)
{
    m_height = a_height;
}

std::list<KritaLayer*> KritaImage::GetLayers() const
{
    return m_layers;
}