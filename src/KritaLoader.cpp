#include "KritaLoader.h"

#include <assert.h>

#include "ICCFile.h"

// File IO helper defines
// I dont normally use defines but IO can be a pain sometimes
#define IFSETTOATTVAL(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = attVal; }
#define IFSETTOATTVALI(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stoi(attVal); }
#define GETNAME(val, node) char* name = node->name(); int len = strlen(name); val = new char[len]; strcpy(val, name)
#define GETFILEDATA(dataBuffer, file) { std::istream* stream = file->GetDecompressionStream(); std::size_t len = file->GetSize(); dataBuffer = new char[len]; stream->read(data, len); file->CloseDecompressionStream(); }

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
std::string ToLower(const std::string& a_string)
{
    const int len = a_string.length();

    std::string cache = a_string;

    for (int i = 0; i < len; ++i)
    {
        if (a_string[i] <= 'Z' && a_string[i] >= 'A')
        {
            cache[i] = cache[i] + 32;
        }
    }

    return cache;
}

std::string GetFilePathNoExtension(const KritaLayer& a_layer)
{
    return (a_layer.Image->Directory + "/layers/" + a_layer.FileName).c_str();
}

KritaImage* KritaLoader::GetImageMetaData(const rapidxml::xml_node<>* a_node) const
{
    KritaImage* image = new KritaImage();

    for (rapidxml::xml_attribute<>* att = a_node->first_attribute(); att; att = att->next_attribute())
    {
        char* nameCache;
        GETNAME(nameCache, att);

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
        GETNAME(nameCache, att);

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
        GETNAME(nameCache, node);

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

            layer->Image = m_currentImage;

            m_currentImage->Layers.push_back(layer);
            m_layers.push_back(layer);
        }

        delete[] nameCache;

        GetMainFileNodeData(node);
    }
}

void KritaLoader::LoadMainFile()
{
    std::shared_ptr<ZipArchiveEntry> mainFile = m_file->GetEntry("maindoc.xml");
    
    char* data;
    GETFILEDATA(data, mainFile);

    rapidxml::xml_document<> doc;
    doc.parse<0>(data);

    delete data;

    mainFile->CloseDecompressionStream();

    GetMainFileNodeData(doc.first_node());    
}
void KritaLoader::LoadLayerMetaFiles()
{
    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        KritaImage* image = *iter;
        for (auto layerIter = image->Layers.begin(); layerIter != image->Layers.end(); ++layerIter)
        {
            KritaLayer* layer = *layerIter;

            std::string filePath = GetFilePathNoExtension(*layer);

            std::shared_ptr<ZipArchiveEntry> file = m_file->GetEntry(filePath.c_str());
            
            char* data;
            GETFILEDATA(data, file);

            std::string str = data;

            int prev = 0;
            int index = str.find('\n');

            while (index != -1)
            {
                int spc = str.find(' ', prev);

                std::string name = str.substr(prev, spc - prev);
                std::string val = str.substr(spc, index - spc);

                name = ToLower(name);

                const char* cStr = name.c_str();
                
                IFSETTOATTVALI(cStr, "pixelsize", layer->PixelSize, val)

                prev = index + 1;
                index = str.find('\n', index + 1);
            }
            
            delete data;
            data = nullptr;

            std::shared_ptr<ZipArchiveEntry> defaultPixelFile = m_file->GetEntry((filePath + ".defaultpixel").c_str());

            GETFILEDATA(data, defaultPixelFile);
            if (data != nullptr)
            {
                layer->DefaultPixel = *((int*)data);
            }
        }
    }
}

KritaLoader::KritaLoader(const char* a_path) :
    m_currentImage(nullptr)
{
    m_file = ZipFile::Open(a_path);

    LoadMainFile();
    LoadLayerMetaFiles();
}
KritaLoader::~KritaLoader()
{
    for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
    {
        delete *iter;
    }

    for (auto iter = m_layers.begin(); iter != m_layers.end(); ++iter)
    {
        delete *iter;
    }
}

int KritaLoader::GetLayerCount() const
{
    return m_layers.size();
}
LayerMeta* KritaLoader::GetLayerMeta(int a_index) const
{
    KritaLayer* layer = m_layers[a_index];

    LayerMeta* layerMeta = new LayerMeta();
    layerMeta->Name = layer->Name.c_str();
    layerMeta->Width = layer->Image->Width;
    layerMeta->Height = layer->Image->Height;

    return layerMeta;
}
Layer* KritaLoader::GetLayer(int a_index) const
{
    KritaLayer* layer = m_layers[a_index];

    std::string filePath = GetFilePathNoExtension(*layer) + ".icc";

    char* data;
    std::shared_ptr<ZipArchiveEntry> file = m_file->GetEntry(filePath);
    GETFILEDATA(data, file);

    ICCFile* iccFile = new ICCFile(data);

    delete data;

    delete iccFile;
}