#pragma once

#include <string.h>

template<typename T>
class PascalString
{
private:
    char*        m_data; 
    
    T GetStrLen(const char* a_str, int a_bytes)
    {
        const size_t bytes = sizeof(T);

        if (a_bytes == bytes)
        {
            return *(T*)a_str;
        }

        char* valBytes = new char[bytes];
        memset(valBytes, 0, bytes);

        int cnt = bytes < a_bytes ? bytes : a_bytes; 

        for (int i = 0; i < cnt; ++i)
        {
            valBytes[i] = a_str[i];
        }

        T val = *(T*)valBytes;

        delete[] valBytes;

        return val;
    } 
protected:

public:
    PascalString()
    {
        m_data = new char[sizeof(T)] { '0' };
    }
    PascalString(const PascalString<T>& a_string)
    {
        const size_t bytes = sizeof(T);
        const T len = a_string.GetLength();
        const size_t size = len + bytes;

        m_data = new char[size];

        memcpy(m_data, a_string.m_data, size);
    }
    PascalString(const char* a_cstr)
    {
        const size_t bytes = sizeof(T);

        T length = strlen(a_cstr);

        m_data = new char[length + bytes];
        memcpy(m_data, &length, bytes);

        for (int i = 0; i < length; ++i)
        {
            m_data[i + bytes] = a_cstr[i];
        }
    }
    PascalString(const char* a_pstr, int a_lengthBytes)
    {
        const size_t bytes = sizeof(T);

        T length = GetStrLen(a_pstr, a_lengthBytes);

        m_data = new char[length + bytes];
        memcpy(m_data, &length, bytes);

        for (int i = 0; i < length; ++i)
        {
            m_data[i + bytes] = a_pstr[i + a_lengthBytes];
        }
    }
    PascalString(T a_length, char* a_data)
    {
        const size_t bytes = sizeof(T);

        m_data = new char[bytes + a_length];
        memcpy(m_data, &a_length, bytes);

        for (int i = 0; i < a_length; ++i)
        {
            m_data[i + bytes] = a_data[i];
        }
    }
    ~PascalString()
    {
        delete[] m_data;
    }
    
    const char* GetPStr() const
    {
        return m_data;
    }
    T GetLength() const
    {
        return *(T*)m_data;
    }

    PascalString<T>& operator =(const PascalString<T> a_other)
    {
        delete[] m_data;

        const size_t bytes = sizeof(T);
        const T length = a_other.GetLength();

        const size_t size = bytes + length;

        m_data = new char[size];
        memcpy(m_data, a_other.m_data, size);

        return *this;
    }

    char* ToCStr() const
    {
        const size_t bytes = sizeof(T);

        T length = GetLength();

        char* data = new char[length + 1];

        for (int i = 0; i < length; ++i)
        {
            data[i] = m_data[i + bytes];
        }

        data[length] = 0;

        return data;
    }
};