#pragma once

#include <list>
#include <map>
#include <string>

class Name;

class Namer
{   
private:
    struct ID
    {
        long ID;
        long Objects;
    };

    std::map<std::string, ID>* m_objectNames;
    std::list<const char*>*    m_nameList;

    char* CreateName(ID& a_id, const char* a_trueName);
protected:

public:
    Namer();
    ~Namer();

    void RemoveName(const Name& a_name);
    void ClearName(const Name& a_name);

    void SetUniqueName(Name& a_name);
    
    void RemoveUniqueName(const char* a_name);
    void AddUniqueName(const char* a_name);
};