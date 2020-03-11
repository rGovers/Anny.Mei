#pragma once

#include <list>

struct XMLFileAttribute
{
    char* Name;
    char* Value;
};

class XMLFileProperty
{
private:
    char*                       m_name;
    char*                       m_value;

    std::list<XMLFileAttribute> m_attributes;

    XMLFileProperty*            m_parent;
    std::list<XMLFileProperty*> m_children;
protected:

public:
    XMLFileProperty();
    ~XMLFileProperty();

    const char* GetName() const;
    void SetName(const char* a_name);

    std::list<XMLFileAttribute>& Attributes();

    void EmplaceAttribute(const char* a_name, const char* a_value);

    const char* GetValue() const;
    void SetValue(const char* a_value);

    void SetParent(XMLFileProperty* a_parent);
    XMLFileProperty* GetParent() const;

    std::list<XMLFileProperty*> GetChildren() const;
};

// Time to reinvent the wheel again
// The last wheel was broken and no one could direct me to one that works
// This seems to happen alot with me
// This is the reason I had created my own file format to avoid this nonsense
class XMLFile
{
private:
    char*                       m_version;
    char*                       m_encoding;

    std::list<XMLFileProperty*> m_properties;

    int LoadProperty(XMLFileProperty* a_parent, const char* a_data);
protected:

public:
    XMLFile();
    XMLFile(const char* a_data);
    ~XMLFile();

    std::list<XMLFileProperty*> GetProperties() const;
    std::list<XMLFileProperty*> GetBaseProperties() const;
};