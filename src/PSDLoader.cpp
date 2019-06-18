#include "FileLoaders/PSDLoader.h"

#include <assert.h>
#include <fstream>

#include "VariableUtils.h"

#define READUINT(val, data, index) { val = GetReversedUInt(data + index); index += 4; }
#define READUSHORT(val, data, index) { val = GetReversedUShort(data + index); index += 2; }

unsigned int PSDLoader::ReadHeader(const char* a_data)
{
    const int HEADERSIZE = 26;

    m_header = new Header();

    m_header->Signature = *(unsigned int*)(a_data + 0);
    m_header->Version = GetReversedUShort(a_data + 4);
    m_header->Channels = GetReversedUShort(a_data + 12);
    m_header->Height = GetReversedUInt(a_data + 14);
    m_header->Width = GetReversedUInt(a_data + 18);
    m_header->ColorDepth = GetReversedShort(a_data + 22);
    m_header->ColorMode = (e_ColorMode)GetReversedUShort(a_data + 24);

    return HEADERSIZE; 
}
unsigned int PSDLoader::ReadColorModeData(const char* a_data)
{
    unsigned int len = GetReversedUInt(a_data + 0);

    return len + 4;
}
unsigned int PSDLoader::ReadImageResources(const char* a_data)
{
    unsigned int len = GetReversedUInt(a_data + 0);

    unsigned int index = 4;

    while (index < len + 4)
    {
        ImageResourceData resource;
        
        unsigned short iden;

        READUINT(resource.Signature, a_data, index);
        READUSHORT(iden, a_data, index);
        resource.Identifier = (e_ImageResourceID)iden;
        // So it was giving free errors when I did not have a copy operator
        // Implemented the copy operator did nothing but fixed crashes
        // The assign operator is the one being used not the copy operator
        // Either the compiler is doing funny buisness or breakpoints are not triggering and it is actually doing something
        // I hope it is the later 
        // This is why I prefer C# I actually know what is going on
        resource.String = PascalString<unsigned int>(a_data + index, 1);
        const unsigned int length = resource.String.GetLength();
        if (length == 0)
        {
            index += 2;
        }
        else
        {
            index += length + 1;
        }
        READUINT(resource.ResourceSize, a_data, index);
        resource.Data = new char[resource.ResourceSize];
        memcpy(resource.Data, a_data + index, resource.ResourceSize);
        index += resource.ResourceSize;

        m_imageResourceData.emplace_back(resource);
    }

    return len + 4;
}
unsigned int PSDLoader::ReadLayerMaskInfo(const char* a_data)
{
    unsigned int len = GetReversedUInt(a_data + 0);

    return len + 4;
}
unsigned int PSDLoader::ReadImageData(const char* a_data)
{
    unsigned short compression = GetReversedUShort(a_data);

    return 2;
}

PSDLoader::PSDLoader(const char* a_path)
{
    std::fstream stream;
    stream.open(a_path, std::fstream::in);

    unsigned int offset = 0;

    if (stream.good())
    {
        stream.seekg(0, stream.end);
        size_t len = stream.tellg();
        stream.seekg(0, stream.beg);

        char* data = new char[len];

        stream.read(data, len);

        offset += ReadHeader(data + offset);

        assert(m_header->Signature == 0x53504238);
        assert(m_header->Version == 1);

        offset += ReadColorModeData(data + offset);
        offset += ReadImageResources(data + offset);
        offset += ReadLayerMaskInfo(data + offset);
        offset += ReadImageData(data + offset);

        delete[] data;
    }

    stream.close();
}
PSDLoader::~PSDLoader()
{
    delete m_header;

    for (auto iter = m_imageResourceData.begin(); iter != m_imageResourceData.end(); ++iter)
    {
        delete[] iter->Data;
    }
}

int PSDLoader::GetLayerCount() const
{
    return 0;
}
LayerMeta* PSDLoader::GetLayerMeta(int a_index) const
{
    return nullptr;
}
Layer* PSDLoader::GetLayer(int a_index) const
{
    return nullptr;
}