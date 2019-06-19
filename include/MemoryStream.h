#pragma once

#include <istream>

struct MemoryBuffer : std::streambuf
{
    char* m_data;

    MemoryBuffer() = delete;
    MemoryBuffer(const char* a_base, size_t a_size)
    {
        // Was having issues with memory changing so I just copied the data over
        m_data = new char[a_size];
        memcpy(m_data, a_base, a_size);
        setg(m_data, m_data, m_data + a_size);
    }
    ~MemoryBuffer()
    {
        delete[] m_data;
    }
};

struct IMemoryStream : virtual MemoryBuffer, std::istream
{
    IMemoryStream() = delete;
    IMemoryStream(const char* a_base, size_t a_size) : 
        MemoryBuffer(a_base, a_size),
        std::istream(static_cast<std::streambuf*>(this))
    {
        
    }
};
