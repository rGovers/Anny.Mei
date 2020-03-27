#include "FileLoaders/KritaLoader.h"

#include <string.h>
#include <map>

#include "FileUtils.h"

extern "C"
{
    #include "lzf.h"
}

const static int INDEX_TABLE[] = { 2, 1, 0, 3};

KritaLayer::KritaLayer(KritaImage* a_image)
{
    m_image = a_image;

    m_parent = nullptr;

    m_layerType = e_LayerType::Null;

    m_data = nullptr;

    m_name = nullptr;
    m_filename = nullptr;
}
KritaLayer::~KritaLayer()
{
    if (m_name != nullptr)
    {
        delete[] m_name;
    }
    if (m_filename != nullptr)
    {
        delete[] m_filename;
    }

    if (m_data != nullptr)
    {
        delete[] m_data;
    }

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        (*iter)->m_parent = m_parent;
        
        if (m_parent != nullptr)
        {
            m_parent->m_children.emplace_back(*iter);
        }
    }

    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }
}

KritaLayer* KritaLayer::GetParent() const
{
    return m_parent;
}
void KritaLayer::SetParent(KritaLayer* a_parent)
{
    m_parent = a_parent;

    if (m_parent != nullptr)
    {
        m_parent->m_children.emplace_back(this);
    }
}
std::list<KritaLayer*> KritaLayer::GetChildren() const
{
    return m_children;
}

int KritaLayer::GetXOffset() const
{
    return m_xOffset;
}
void KritaLayer::SetXOffset(int a_offset)
{
    m_xOffset = a_offset;
}

int KritaLayer::GetYOffset() const
{
    return m_yOffset;
}
void KritaLayer::SetYOffset(int a_offset)
{
    m_yOffset = a_offset;
}

int KritaLayer::GetWidth() const
{
    return m_width;
}
int KritaLayer::GetHeight() const
{
    return m_height;
}

e_LayerType KritaLayer::GetLayerType() const
{
    return m_layerType;
}
void KritaLayer::SetLayerType(e_LayerType a_layerType)
{
    m_layerType = a_layerType;
}

const char* KritaLayer::GetName() const
{
    return m_name;
}
void KritaLayer::SetName(const char* a_name)
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

const char* KritaLayer::GetFilename() const
{
    return m_filename;
}
void KritaLayer::SetFilename(const char* a_name)
{
    if (m_filename != nullptr)
    {
        delete[] m_filename;
        m_filename = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        m_filename = new char[len + 1];
        
        strcpy(m_filename, a_name);
    }
}

// Grabbed function directly from Krita source
// KisAbstractComression::delinearizeColors
void KritaLayer::DelinearizeColors(char* a_input, char* a_output, unsigned int a_size, unsigned int a_pixelSize)
{
    char* outByte = a_output;
    char* last = a_output + a_size - 1;

    unsigned int stride = a_size / a_pixelSize;
    char* startByte = a_input;

    while (outByte <= last)
    {
        char* inByte = startByte;

        for (int i = 0; i < a_pixelSize; ++i)
        {
            *outByte = *inByte;
            ++outByte;
            inByte += stride;
        }

        ++startByte;
    }
}

// Documentation was incorrect so I had to resort to source
// https://github.com/KDE/krita/blob/master/libs/image/tiles3/kis_tiled_data_manager.cc
// https://github.com/KDE/krita/blob/master/libs/image/tiles3/swap/kis_tile_compressor_2.cpp
// Functions of interest:
//  KisTiledDataManager::read
//  KisTiledDataManager::processTilesHeader
//  KisTileCompressor2::readTile
//  KisTileCompressor2::decompressTileData
void KritaLayer::LoadData(mz_zip_archive& a_archive)
{
    std::string filename = std::string(m_image->GetName()) + "/layers/" + m_filename; 

    char* data = ExtractFileFromArchive(filename.c_str(), a_archive);

    if (data != nullptr)
    {
        const int imageWidth = m_image->GetWidth();
        const int imageHeight = m_image->GetHeight();

        int version = -1;
        int tileWidth = -1;
        int tileHeight = -1;
        int pixelSize = -1;
        int tileCount = 1;

        char* start = data;
        for (int i = 0; i < 5; ++i)
        {
            int* modInt = nullptr;

            char* spc = strchr(start, ' ');
            char* lineEnd = strchr(start, '\n');

            const size_t nameLen = spc - start + 1;

            char* name = new char[nameLen];
            memcpy(name, start, nameLen - 1);
            name[nameLen - 1] = 0;

            if (strcmp(name, "VERSION") == 0)
            {
                modInt = &version;
            }
            // These values seem to be ignored by krita and just used as a sanity check by what I can tell
            // Useful for me however because I do not want to hard bake the values when they are available to me
            else if (strcmp(name, "TILEWIDTH") == 0)
            {
                modInt = &tileWidth;
            }
            else if (strcmp(name, "TILEHEIGHT") == 0)
            {
                modInt = &tileHeight;
            }
            else if (strcmp(name, "PIXELSIZE") == 0)
            {
                modInt = &pixelSize;
            }
            // So accoring to the examples in documentation this does not have a value however code and live examples use this to dictate the number of tiles?
            else if (strcmp(name, "DATA") == 0)
            {
                modInt = &tileCount;
            }

            if (modInt != nullptr)
            {
                spc += 1;
                const size_t valLen = lineEnd - spc + 1;

                char* value = new char[valLen];
                memcpy(value, spc, valLen - 1);
                value[valLen - 1] = 0;

                *modInt = std::stoi(value);

                delete[] value;
            }

            delete[] name;

            start = lineEnd + 1;
        }

        // No plans to support old versions for now
        if (version >= 2 && tileWidth > 0 && tileHeight > 0 && pixelSize > 0)
        {
            std::map<int, std::map<int, std::list<char*>>> tileGraph; 

            const unsigned int size = pixelSize * tileWidth * tileHeight;

            char* tiles = new char[size];

            glm::ivec2 max = glm::ivec2(-std::numeric_limits<int>::min());
            glm::ivec2 min = glm::ivec2(std::numeric_limits<int>::max());

            for (int i = 0; i < tileCount; ++i)
            {
                char* lineEnd = strchr(start, '\n');

                if (lineEnd == nullptr || lineEnd[0] == 0)
                {
                    break;
                }

                char* brkA = strchr(start, ',');
                char* brkB = strchr(brkA + 1, ',');
                char* brkC = strchr(brkB + 1, ',');

                size_t len = brkA - start + 1;
                char* str = new char[len];
                memcpy(str, start, len - 1);
                str[len - 1] = 0;

                int xPos = std::stoi(str);

                delete[] str;

                len = (brkB - 1) - brkA + 1;
                str = new char[len];
                memcpy(str, brkA + 1, len - 1);
                str[len - 1] = 0;

                int yPos = std::stoi(str);

                delete[] str;

                len = (brkC - 1) - brkB + 1;
                char* compression = new char[len];
                memcpy(compression, brkB + 1, len - 1);
                compression[len - 1] = 0;

                len = (lineEnd - 1) - brkC + 1;
                str = new char[len];
                memcpy(str, brkC + 1, len - 1);
                str[len - 1] = 0;

                int dataSize = std::stoi(str);

                delete[] str;

                if (dataSize > 0)
                {
                    char* data = lineEnd + 1;

                    if (strcmp(compression, "LZF") == 0)
                    {
                        char* nonLinearTiles = new char[size];

                        switch (data[0])
                        {
                        // Compressed
                        case 1:
                        {
                            const unsigned int decompSize = lzf_decompress(data + 1, dataSize - 1, tiles, size);

                            if (decompSize == size)
                            {
                                DelinearizeColors(tiles, nonLinearTiles, size, pixelSize);
                            }
                            else
                            {
                                return;
                            }
                            
                            break;
                        }
                        case 0:
                        {
                            memcpy(nonLinearTiles, data + 1, size);

                            break;
                        }
                        }

                        const int xIndex = xPos / tileWidth;
                        const int yIndex = yPos / tileHeight;

                        min.x = glm::min(xIndex, min.x);
                        min.y = glm::min(yIndex, min.y);

                        max.x = glm::max(xIndex, max.x);
                        max.y = glm::max(yIndex, max.y);

                        auto iter = tileGraph.find(xIndex);

                        if (iter != tileGraph.end())
                        {
                            auto iIter = iter->second.find(yIndex);

                            if (iIter != iter->second.end())
                            {
                                iIter->second.emplace_back(nonLinearTiles);
                            }
                            else
                            {
                                std::list<char*> tilesList;
                                tilesList.emplace_back(nonLinearTiles);

                                iter->second.emplace(yIndex, tilesList);
                            }
                        }
                        else
                        {
                            std::map<int, std::list<char*>> innerMap;

                            std::list<char*> tilesList;
                            tilesList.emplace_back(nonLinearTiles);

                            innerMap.emplace(yIndex, tilesList);

                            tileGraph.emplace(xIndex, innerMap);
                        }
                        
                    }
                    else
                    {
                        // For now need to add more but unsure of values
                        assert(0);
                    }
                }

                start = lineEnd + dataSize + 1;

                delete[] compression;
            }

            delete[] tiles;

            glm::vec2 diff = max - min;

            diff.x = glm::abs(diff.x);
            diff.y = glm::abs(diff.y);

            m_width = diff.x * tileWidth + tileWidth;
            m_height = diff.y * tileWidth + tileWidth;

            m_xOffset += min.x * tileWidth;
            m_yOffset += min.y * tileHeight;

            const unsigned int imageSize = m_width * m_height * 4;

            m_data = new char[imageSize];
            memset(m_data, 0, imageSize);

            const int pMax = glm::min(pixelSize, 4);

            for (unsigned int x = 0; x < m_width; ++x)
            {
                const int tX = x + min.x * tileWidth;

                const int gridX = tX / tileWidth;
                const int relIndX = x % tileWidth;

                for (unsigned int y = 0; y < m_height; ++y)
                {
                    const int tY = y + min.y * tileHeight;

                    const int gridY = tY / tileHeight;
                    const int relIndY = y % tileHeight;

                    auto iter = tileGraph.find(gridX);
                    if (iter != tileGraph.end())
                    {
                        auto iIter = iter->second.find(gridY);

                        if (iIter != iter->second.end())
                        {
                            for (auto lIter = iIter->second.begin(); lIter != iIter->second.end(); ++lIter)
                            {
                                const char* tileGrid = *lIter;

                                for (int i = 0; i < pMax; ++i)
                                {
                                    const int colInd = INDEX_TABLE[i];

                                    const unsigned int index = y * m_width * 4 + x * 4 + i;
                                    const unsigned int relIndex = relIndY * tileWidth * pixelSize + relIndX * pixelSize + colInd;

                                    m_data[index] += tileGrid[relIndex];
                                }
                            }
                        }
                    }
                }
            }

            for (auto iter = tileGraph.begin(); iter != tileGraph.end(); ++iter)
            {
                for (auto iIter = iter->second.begin(); iIter != iter->second.end(); ++iIter)
                {
                    for (auto lIter = iIter->second.begin(); lIter != iIter->second.end(); ++lIter)
                    {
                        delete[] *lIter;
                    }
                }
            }
        }

        mz_free(data);
    }
}

LayerMeta KritaLayer::ToLayerMeta() const
{
    LayerMeta meta;

    std::string name = GetFullName();

    const size_t len = name.length();
    meta.Name = new char[len + 1];
    strcpy(meta.Name, name.c_str());

    meta.Width = m_width;
    meta.Height = m_height;
    meta.xOffset = GetXOffset();
    meta.yOffset = GetYOffset();

    return meta;
}
Layer* KritaLayer::ToLayer() const
{
    Layer* layer = new Layer();
    layer->MetaData = ToLayerMeta();

    const unsigned int size = m_width * m_height * 4;

    layer->Data = new char[size];
    memcpy(layer->Data, m_data, size);

    return layer;
}

std::string KritaLayer::GetFullName() const
{
    if (m_parent != nullptr)
    {
        return "[" + m_parent->GetFullName() + "] " + m_name;
    }

    return m_name;
}