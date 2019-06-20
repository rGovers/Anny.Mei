#pragma once

#include <list>
#include <ZipLib/ZipArchive.h>

struct PropertyFileValue
{
    char* Name;
    char* Value;
};

class PropertyFileProperty
{
private:
    char*                            m_name;
    std::list<PropertyFileValue>     m_values;

    PropertyFileProperty*            m_parent;
    std::list<PropertyFileProperty*> m_children;

protected:

public:
    PropertyFileProperty();
    ~PropertyFileProperty();

    const char* GetName() const;
    void SetName(const char* a_value);

    std::list<PropertyFileValue>& Values();

    void EmplaceValue(const char* a_name, const char* a_value);

    void SetParent(PropertyFileProperty* a_parent);
    PropertyFileProperty* GetParent() const;

    std::list<PropertyFileProperty*> GetChildren() const;
};

class PropertyFile
{
private:
    std::list<PropertyFileProperty*>* m_properties;

    int LoadProperty(PropertyFileProperty* a_parent, const char* a_data);

    void ToString(const PropertyFileProperty& a_property, std::string& a_data) const;
protected:

public:
    PropertyFile();
    PropertyFile(const char* a_data);
    ~PropertyFile();

    std::list<PropertyFileProperty*> GetProperties() const;
    
    char* ToString() const;

    PropertyFileProperty* InsertProperty();
    void RemoveProperty(PropertyFileProperty* a_property);
};