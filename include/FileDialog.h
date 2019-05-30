#pragma once

class FileDialog
{
private:
    static FileDialog* m_fileDialogSingleton;

    char               m_buffer[1024];

    FileDialog();
    ~FileDialog();
protected:

public:
    static bool Create();
    static void Destroy();

    static const char* OpenFile(const char* a_title, const char* const* const a_filters, int a_filterNum);
};