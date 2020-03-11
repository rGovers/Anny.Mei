#include "FileLoaders/XMLFile.h"

#include <string.h>

int XMLFile::LoadProperty(XMLFileProperty* a_parent, const char* a_data)
{
    long end = 0;

    long openProp = -1;
    long closeProp = -1;
    long equals = -1;
    long terminator = -1;
    long descriptor = -1;
    long quote = -1;
    long closeQuote = -1;
    long space = -1;

    char** toSet = nullptr;

    XMLFileProperty* property = nullptr;
    XMLFileAttribute attribute;

    for (unsigned int i = 0; a_data[i] != 0; ++i)
    {
        const char chr = a_data[i];

        switch (chr)
        {
        case '<':
        {
            if (closeQuote == quote)
            {
                openProp = i;

                property = new XMLFileProperty();
            }
            
            break;
        }
        case '>':
        {
            if (closeQuote == quote)
            {
                if (openProp > space)
                {
                    const int len = i - openProp;

                    char* name = new char[len];
                    memcpy(name, a_data + openProp + 1, len - 1);
                    name[len - 1] = 0;

                    property->SetName(name);

                    delete[] name;
                }

                if (property != nullptr)
                {
                    property->SetParent(a_parent);
                }

                if (terminator > openProp)
                {
                    if (a_parent != nullptr)
                    {
                        const int len = i - terminator;

                        char* name = new char[len];
                        memcpy(name, a_data + terminator + 1, len - 1);
                        name[len - 1] = 0;

                        if (strcmp(name, a_parent->GetName()) == 0)
                        {
                            delete property;

                            delete[] name;

                            return i + 1;
                        }

                        delete[] name;
                    }

                    m_properties.emplace_back(property);
                }
                else if (descriptor < openProp)
                {
                    m_properties.emplace_back(property);

                    i += LoadProperty(property, a_data + i + 1);
                }

                property = nullptr;

                closeProp = i;
            }
            
            break;
        }
        case '"':
        {
            if (quote > equals)
            {
                const int len = i - quote;

                char* value = new char[len];
                memcpy(value, a_data + quote + 1, len - 1);
                value[len - 1] = 0;

                if (property != nullptr)
                {
                    attribute.Value = value;

                    property->Attributes().emplace_back(attribute);
                }
                else if (toSet != nullptr)
                {
                    *toSet = value;

                    toSet = nullptr;
                }
                
                closeQuote = i;
            }

            quote = i;

            break;
        }
        case '=':
        {
            if (space > quote)
            {
                const int len = i - space;
                char* name = new char[len];
                memcpy(name, a_data + space + 1, len - 1);
                name[len - 1] = 0;

                if (property != nullptr)
                {
                    attribute.Name = name;
                }
                else 
                {
                    if (strcmp(name, "version") == 0)
                    {
                        toSet = &m_version;
                    }
                    else if (strcmp(name, "encoding") == 0)
                    {
                        toSet = &m_encoding;
                    }

                    delete name;
                }
            
                equals = i;
            }

            break;
        }
        case ' ':
        {
            if (closeQuote == quote)
            {
                if (openProp > space && property != nullptr)
                {
                    const int len = i - openProp;

                    char* name = new char[len];
                    memcpy(name, a_data + openProp + 1, len - 1);
                    name[len - 1] = 0;

                    property->SetName(name);

                    delete[] name;
                }

                space = i;
            }

            break;
        }
        case '!':
        case '/':
        {
            if (closeQuote == quote)
            {
                terminator = i;
            }

            break;
        }
        case '?':
        {
            if (closeQuote == quote)
            {
                descriptor = i;

                if (property != nullptr)
                {
                    delete property;
                    property = nullptr;
                }
            }
            
            break;
        }
        };

        end = i;
    }

    return end;
}

XMLFile::XMLFile()
{
    m_version = nullptr;
    m_encoding = nullptr;
}
XMLFile::XMLFile(const char* a_data) : 
    XMLFile()
{
    LoadProperty(nullptr, a_data);
}
XMLFile::~XMLFile()
{
    for (auto iter = m_properties.begin(); iter != m_properties.end(); ++iter)
    {
        delete *iter;
    }
}

std::list<XMLFileProperty*> XMLFile::GetProperties() const
{
    return m_properties;
}
std::list<XMLFileProperty*> XMLFile::GetBaseProperties() const
{
    std::list<XMLFileProperty*> properties;

    for (auto iter = m_properties.begin(); iter != m_properties.end(); ++iter)
    {
        if ((*iter)->GetParent() == nullptr)
        {
            properties.emplace_back(*iter);
        }
    }

    return properties;
}