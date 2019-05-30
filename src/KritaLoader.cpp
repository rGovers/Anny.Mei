#include "KritaLoader.h"

#include <assert.h>

#define IFSETTOATTVAL(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = attVal; }
#define IFSETTOATTVALI(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stoi(attVal); }

void ToLower(char* a_string)
{
    for (int i = 0; a_string[i] != 0; ++i)
    {
        if (a_string[i] <= 'Z' && a_string[i] >= 'A')
        {
            a_string[i] = a_string[i] + 32;
        }
    }
}

KritaImage* KritaLoader::GetImageMetaData(const rapidxml::xml_node<>* a_node) const
{
    KritaImage* image = new KritaImage();

    for (rapidxml::xml_attribute<>* att = a_node->first_attribute(); att; att = att->next_attribute())
    {
        char* nameCache;
        {
            char* name = att->name();
            int len = strlen(name);
            nameCache = new char[len];
            strcpy(nameCache, name);
        }

        ToLower(nameCache);

        char* val = att->value();

        IFSETTOATTVAL(nameCache, "name", image->Directory, val)
        else IFSETTOATTVALI(nameCache, "width", image->Width, val)
        else IFSETTOATTVALI(nameCache, "height", image->Height, val)

        delete[] nameCache;
    }

    // I forgot to return -_-
    // I forgot most c++ compilers dont scream at you about it
    return image;
}
KritaLayer* KritaLoader::GetLayerMetaData(const rapidxml::xml_node<>* a_node) const
{
    KritaLayer* layer = new KritaLayer();

    for (rapidxml::xml_attribute<>* att = a_node->first_attribute(); att; att = att->next_attribute())
    {
        char* nameCache;
        {
            char* name = att->name();
            int len = strlen(name);
            nameCache = new char[len];
            strcpy(nameCache, name);
        }

        ToLower(nameCache);

        char* val = att->value();

        IFSETTOATTVAL(nameCache, "filename", layer->FileName, val)
        else IFSETTOATTVAL(nameCache, "name", layer->Name, val)

        delete[] nameCache;  
    }

    return layer;
}

void KritaLoader::GetMainFileNodeData(const rapidxml::xml_node<>* a_node)
{
    for (rapidxml::xml_node<>* node = a_node->first_node(); node; node = node->next_sibling())
    {
        char* nameCache;
        // To prevent me from using name later down the line
        {
            char* name = node->name();
            int len = strlen(name);
            nameCache = new char[len];
            strcpy(nameCache, name);    
        }
        
        ToLower(nameCache);

        if (strcmp(nameCache, "image") == 0)
        {
            m_currentImage = GetImageMetaData(node);

            m_images.push_back(m_currentImage);
        }
        else if (strcmp(nameCache, "layer") == 0)
        {
            KritaLayer* layer = GetLayerMetaData(node);

            // Should not happen unless something is wrong with the file
            // Should being the keyword
            assert(m_currentImage != nullptr);

            m_currentImage->Layers.push_back(layer);
        }

        delete[] nameCache;

        GetMainFileNodeData(node);
    }
}

void KritaLoader::LoadMainFile()
{
    std::shared_ptr<ZipArchiveEntry> mainFile = m_file->GetEntry("maindoc.xml");
    std::istream* stream = mainFile->GetDecompressionStream();  
    
    size_t size = mainFile->GetSize();
    char* data = new char[size];

    stream->read(data, size);

    rapidxml::xml_document<> doc;
    doc.parse<0>(data);

    GetMainFileNodeData(doc.first_node());    
}

KritaLoader::KritaLoader(const char* a_path) :
    m_currentImage(nullptr)
{
    m_file = ZipFile::Open(a_path);

    LoadMainFile();
}
KritaLoader::~KritaLoader()
{
    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        delete *iter;
    }
}
