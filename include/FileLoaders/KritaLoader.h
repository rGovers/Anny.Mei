#pragma once

#include <glm/glm.hpp>
#include <list>
#include <string>

#include "ImageLoader.h"
#include "miniz.h"

class KritaImage;
class XMLFileProperty;

enum class e_LayerType
{
    Null,
    Group,
    Paint
};

class KritaLayer 
{
private:
    KritaImage*            m_image;

    KritaLayer*            m_parent;
    std::list<KritaLayer*> m_children;

    char*                  m_data;

    int                    m_xOffset;
    int                    m_yOffset;

    int                    m_width;
    int                    m_height;

    e_LayerType            m_layerType;

    char*                  m_name;
    char*                  m_filename;

    std::string GetFullName() const;

    void DelinearizeColors(char* a_input, char* a_output, unsigned int a_size, unsigned int a_pixelSize);
protected:

public:
    KritaLayer(KritaImage* a_image);
    ~KritaLayer();

    int GetXOffset() const;
    void SetXOffset(int a_offset);

    int GetYOffset() const;
    void SetYOffset(int a_offset);

    int GetWidth() const;
    int GetHeight() const;

    KritaLayer* GetParent() const;
    void SetParent(KritaLayer* a_parent);
    std::list<KritaLayer*> GetChildren() const;

    e_LayerType GetLayerType() const;
    void SetLayerType(e_LayerType a_layerType);

    const char* GetName() const;
    void SetName(const char* a_name);

    const char* GetFilename() const;
    void SetFilename(const char* a_name);

    void LoadData(mz_zip_archive& a_archive);

    LayerMeta ToLayerMeta() const;
    Layer* ToLayer() const;
};

class KritaImage
{
private:
    std::list<KritaLayer*> m_layers;

    char*                  m_name;

    int                    m_width;
    int                    m_height;
protected:

public: 
    KritaImage();
    ~KritaImage();

    const char* GetName() const;
    void SetName(const char* a_name);

    int GetWidth() const;
    void SetWidth(int a_width);

    int GetHeight() const;
    void SetHeight(int a_height); 

    void LoadLayers(mz_zip_archive& a_archive, KritaLayer* a_layer, XMLFileProperty* a_property);

    std::list<KritaLayer*> GetLayers() const;
};

class KritaLoader : public ImageLoader
{
private:
    std::list<KritaImage*> m_images;

    void LoadImages(mz_zip_archive& a_archive, XMLFileProperty* a_property);
    bool Load(mz_zip_archive& a_archive);

protected:
    KritaLoader();

public:
    ~KritaLoader();

    static KritaLoader* Load(const char* a_filename);

    virtual int GetLayerCount() const;
    virtual LayerMeta ToLayerMeta(int a_index) const;
    virtual Layer* ToLayer(int a_index) const;
};