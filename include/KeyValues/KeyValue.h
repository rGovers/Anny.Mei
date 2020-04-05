#pragma once

enum class e_LerpMode
{
    Lerp,
    Slerp,
    Set
};

class KeyValue
{
private:
    const static char* ITEMS[];

    static int ID;

    KeyValue*     m_prevKeyValue;
    KeyValue*     m_nextKeyValue;

    e_LerpMode    m_lerpMode;

    const char*   m_selectedMode;

    unsigned char m_showLerpMode;
protected:

public:
    KeyValue();
    virtual ~KeyValue();

    void SetNextKeyValue(KeyValue* a_keyValue);
    void SetPrevKeyValue(KeyValue* a_keyValue);

    KeyValue* GetNextKeyValue() const;
    KeyValue* GetPrevKeyValue() const;

    bool GetDisplayLerp() const;
    void SetDisplayLerp(bool a_value);

    bool GetDisplaySlerp() const;
    void SetDisplaySlerp(bool a_value);

    bool GetDisplaySet() const;
    void SetDisplaySet(bool a_value);

    virtual void UpdateValue(double a_shift) = 0;

    void UpdateLerpGUI();

    void SetLerpMode(e_LerpMode a_lerpMode);
    e_LerpMode GetLerpMode() const;

    virtual void UpdateGUI(const char* a_label, bool a_showLerpMode = true) = 0; 

    virtual char* ToString() const = 0;
    virtual void Parse(const char* a_string) = 0;

    static void ResetGUI();
};