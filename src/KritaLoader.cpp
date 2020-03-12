#include "FileLoaders/KritaLoader.h"

#include "FileLoaders/XMLFile.h"
#include "FileUtils.h"

KritaLoader::KritaLoader()
{
    
}
KritaLoader::~KritaLoader()
{
    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        delete *iter;
    }
}

void KritaLoader::LoadImages(mz_zip_archive& a_archive, XMLFileProperty* a_property)
{
    const std::list<XMLFileProperty*> properties = a_property->GetChildren();
    const std::list<XMLFileAttribute> attributes = a_property->Attributes();

    KritaImage* image = new KritaImage();

    for (auto iter = attributes.begin(); iter != attributes.end(); ++iter)
    {
        const char* name = iter->Name;

        if (strcmp(name, "name") == 0)
        {
            image->SetName(iter->Value);
        }
        else if (strcmp(name, "width") == 0)
        {
            image->SetWidth(std::stoi(iter->Value));
        }
        else if (strcmp(name, "height") == 0)
        {
            image->SetHeight(std::stoi(iter->Value));
        }
    }

    for (auto iter = properties.begin(); iter != properties.end(); ++iter)
    {
        image->LoadLayers(a_archive, nullptr, *iter);
    }

    m_images.emplace_back(image);
}
bool KritaLoader::Load(mz_zip_archive& a_archive)
{
    char* data = ExtractFileFromArchive("maindoc.xml", a_archive);

    if (data != nullptr)
    {
        XMLFile* mainDoc = new XMLFile(data);

        const std::list<XMLFileProperty*> properties = mainDoc->GetBaseProperties();

        for (auto iter = properties.begin(); iter != properties.end(); ++iter)
        {
            if (strcmp((*iter)->GetName(), "DOC") == 0)
            {
                const std::list<XMLFileProperty*> cProperties = (*iter)->GetChildren();

                for (auto iIter = cProperties.begin(); iIter != cProperties.end(); ++iIter)
                {
                    if (strcmp((*iIter)->GetName(), "IMAGE") == 0)
                    {
                        LoadImages(a_archive, *iIter);
                    }
                }
            }
        }

        delete mainDoc;

        return true; 
    }

    return false;
}

KritaLoader* KritaLoader::Load(const char* a_filename)
{
    KritaLoader* kritaLoader = nullptr;

    mz_zip_archive archive;
    memset(&archive, 0, sizeof(archive));

    if (mz_zip_reader_init_file(&archive, a_filename, 0))
    {
        kritaLoader = new KritaLoader();

        if (!kritaLoader->Load(archive))
        {
            delete kritaLoader;
            kritaLoader = nullptr;
        }
    }

    mz_zip_reader_end(&archive);

    return kritaLoader;
}

int KritaLoader::GetLayerCount() const
{
    int count = 0;

    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        const std::list<KritaLayer*> layers = (*iter)->GetLayers();

        for (auto iIter = layers.begin(); iIter != layers.end(); ++iIter)
        {
            switch ((*iIter)->GetLayerType())
            {
            case e_LayerType::Paint:
            {
                ++count;

                break;
            }
            }
        }
    }

    return count;
}
LayerMeta KritaLoader::ToLayerMeta(int a_index) const
{
    int count = 0;

    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        const std::list<KritaLayer*> layers = (*iter)->GetLayers();

        for (auto iIter = layers.begin(); iIter != layers.end(); ++iIter)
        {
            switch ((*iIter)->GetLayerType())
            {
            case e_LayerType::Paint:
            {
                if (count++ == a_index)
                {
                    LayerMeta layerMeta = (*iIter)->ToLayerMeta();

                    layerMeta.ImageWidth = (*iter)->GetWidth();
                    layerMeta.ImageHeight = (*iter)->GetHeight();

                    return layerMeta;
                }

                break;
            }
            }
        }
    }

    LayerMeta meta;
    meta.Name = nullptr;
    meta.Width = -1;
    meta.Height = -1;

    return meta;
}
Layer* KritaLoader::ToLayer(int a_index) const
{
    int count = 0;

    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        const std::list<KritaLayer*> layers = (*iter)->GetLayers();

        for (auto iIter = layers.begin(); iIter != layers.end(); ++iIter)
        {
            switch ((*iIter)->GetLayerType())
            {
            case e_LayerType::Paint:
            {
                if (count++ == a_index)
                {
                    Layer* layer = (*iIter)->ToLayer();

                    layer->MetaData.ImageWidth = (*iter)->GetWidth();
                    layer->MetaData.ImageHeight = (*iter)->GetHeight();

                    return layer;
                }

                break;
            }
            }
        }
    }

    return nullptr;
}