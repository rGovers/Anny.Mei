#include "FileDialog.h"

#include <string.h>

#include "tinyfiledialogs.h"

FileDialog* FileDialog::m_fileDialogSingleton = nullptr;

FileDialog::FileDialog()
{
}
FileDialog::~FileDialog()
{
}

bool FileDialog::Create()
{
    FileDialog* s = m_fileDialogSingleton = new FileDialog();

    bool graphic = tinyfd_inputBox("tinyfd_query", nullptr, nullptr);

    if (graphic)
    {
        strcpy(s->m_buffer, "graphic mode: ");
    }
    else
    {
        strcpy(s->m_buffer, "console mode: ");
    }
    
    strcat(s->m_buffer, tinyfd_response);

    return true;
}
void FileDialog::Destroy()
{
    if (m_fileDialogSingleton != nullptr)
    {
        delete m_fileDialogSingleton;
        m_fileDialogSingleton = nullptr;
    }
}

const char* FileDialog::SaveFile(const char* a_title, const char* const* const a_filters, int a_filterNum)
{
    return tinyfd_saveFileDialog(a_title, "", a_filterNum, a_filters, nullptr);
}
const char* FileDialog::OpenFile(const char* a_title, const char* const* const a_filters, int a_filterNum)
{
    return tinyfd_openFileDialog(a_title, "", a_filterNum, a_filters, NULL, 0);
}
const char* FileDialog::SelectFolder(const char* a_title)
{
    return tinyfd_selectFolderDialog(a_title, "");
}