#include "PropertyFile.h"

#include <string.h>

PropertyFile::PropertyFile()
{
    m_properties = new std::list<PropertyFileProperty*>();
}
PropertyFile::PropertyFile(const char* a_data)
{
   
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