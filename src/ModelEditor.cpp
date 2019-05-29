#include "ModelEditor.h"

ModelEditor::ModelEditor(const char* a_path)
{
    m_file = ZipFile::Open(a_path);
}
ModelEditor::~ModelEditor()
{
}