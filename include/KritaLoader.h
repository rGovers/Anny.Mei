#pragma once

#include "ImageLoader.h"

// I have some weird stuff going on that I need to do some digging
// Standard library is doing some weird things I need to work out the cause
#include <list>
#include <string.h>
#include <vector>
#include "ZipLib/ZipFile.h"

#include "rapidxml.hpp"

struct KritaImage;
struct KritaLayer;

struct KritaLayer
{
    KritaImage* Image;

    std::string Name;
    std::string FileName;

    int PixelSize;
    
    int DefaultPixel;
};
struct KritaImage
{
    std::string Directory;
    
    int Width;
    int Height;

    std::list<KritaLayer*> Layers;
};

class KritaLoader : public ImageLoader
{
private:
    KritaImage*              m_currentImage;
  
    std::list<KritaImage*>   m_images; 
  
    ZipArchive::Ptr          m_file;
  
    std::vector<KritaLayer*> m_layers;

    KritaImage* GetImageMetaData(const rapidxml::xml_node<>* a_node) const;
    KritaLayer* GetLayerMetaData(const rapidxml::xml_node<>* a_node) const;

    void GetMainFileNodeData(const rapidxml::xml_node<>* a_node);

    void LoadMainFile();
    void LoadLayerMetaFiles();
protected:

public:
    KritaLoader() = delete;
    KritaLoader(const char* a_path);
    ~KritaLoader();

    virtual int GetLayerCount() const;
    virtual LayerMeta* GetLayerMeta(int a_index) const;
    virtual Layer* GetLayer(int a_index) const;
};