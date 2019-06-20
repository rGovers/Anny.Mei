#include "PropertyFile.h"

#include <assert.h>
#include <string.h>

int PropertyFile::LoadProperty(PropertyFileProperty* a_parent, const char* a_data)
{
    PropertyFileProperty* property = nullptr;

    int end = 0;

    int spc = -1;
    int open = -1;
    int colon = -1;
    int close = -1;
    int quote = -1;

    PropertyFileValue value;

    for (int i = 0; a_data[i] != 0; ++i)
    {
        const char chr = a_data[i];

        switch (chr)
        {
        case '<':
        {
            open = i;

            property = new PropertyFileProperty();

            break;
        }
        case '>':
        {
            close = i;

            assert(property != nullptr);
            property->SetParent(a_parent);
            
            m_properties->emplace_back(property);

            break;
        }
        case ' ':
        {
            if (open > spc)
            {
                const int len = i - open;

                char* name = new char[len];
                memcpy(name, a_data + open + 1, len - 1);
                name[len - 1] = 0;

                property->SetName(name);

                delete[] name;
            }
            
            spc = i;

            break;
        }
        case ':':
        {
            const int len = i - spc;
            
            value.Name = new char[len];
            memcpy(value.Name, a_data + spc + 1, len - 1);
            value.Name[len - 1] = 0;

            colon = i;

            break;
        }
        case '"':
        {
            if (quote > colon)
            {
                const int len = i - quote;

                value.Value = new char[len];
                memcpy(value.Value, a_data + quote + 1, len - 1);
                value.Value[len - 1] = 0;

                property->Values().emplace_back(value);
            }

            quote = i;

            break;
        }
        case '{':
        {
            i += LoadProperty(property, a_data + i);

            break;
        }
        case '}':
        {
            return i;
        }
        }

        end = i;
    }

    return end;
}

PropertyFile::PropertyFile()
{
    m_properties = new std::list<PropertyFileProperty*>();
}
PropertyFile::PropertyFile(const char* a_data) :
    PropertyFile()
{
    LoadProperty(nullptr, a_data);
}
PropertyFile::~PropertyFile()
{
    for (auto iter = m_properties->begin(); iter != m_properties->end(); ++iter)
    {
        delete *iter;
    }

    delete m_properties;
}

void PropertyFile::ToString(const PropertyFileProperty& a_property, std::string& a_string) const
{
    const std::list<PropertyFileProperty*> children = a_property.GetChildren();

    if (children.size() > 0)
    {
        a_string += "{\n";

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            PropertyFileProperty* prop = *iter;

            a_string += "<";
            a_string += prop->GetName();

            for (auto valIter = prop->Values().begin(); valIter != prop->Values().end(); ++valIter)
            {
                a_string += " ";
                a_string += valIter->Name;
                a_string += ":";
                a_string += valIter->Value;
            }

            a_string += ">\n";

            ToString(*prop, a_string);
        }

        a_string += "}\n";
    }
}
char* PropertyFile::ToString() const
{
    std::string data;

    for (auto iter = m_properties->begin(); iter != m_properties->end(); ++iter)
    {
        PropertyFileProperty* prop = *iter;

        if (prop->GetParent() == nullptr)
        {
            data += "<";
            data += prop->GetName();

            for (auto valIter = prop->Values().begin(); valIter != prop->Values().end(); ++valIter)
            {
                data += " ";
                data += valIter->Name;
                data += ":\"";
                data += valIter->Value;
                data += "\"";
            }

            data += ">\n";

            ToString(*prop, data);
        }
    }

    char* dataC = new char[data.length()];
    strcpy(dataC, data.c_str());

    return dataC;
}

std::list<PropertyFileProperty*> PropertyFile::GetProperties() const
{
    return *m_properties;
}

PropertyFileProperty* PropertyFile::InsertProperty()
{
    PropertyFileProperty* property = new PropertyFileProperty();

    m_properties->emplace_back(property);

    return property;
}
void PropertyFile::RemoveProperty(PropertyFileProperty* a_property)
{
    m_properties->remove(a_property);
}