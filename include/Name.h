#pragma once

class Namer;

class Name
{
private:
    Namer* m_namer;
    
    char* m_name;
    char* m_trueName;
protected:

public:
    Name() = delete;
    Name(const char* a_trueName, Namer* a_namer);
    ~Name();

    void SetTrueName(const char* a_name);
    const char* GetTrueName() const;

    void SetName(const char* a_name, bool a_inc = true);
    const char* GetName() const;
};