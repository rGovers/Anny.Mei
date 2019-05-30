#include "ModelEditor.h"

#include "KritaLoader.h"

ModelEditor::ModelEditor(const char* a_path)
{
    KritaLoader* kritaLoader = new KritaLoader(a_path);

    delete kritaLoader;
}
ModelEditor::~ModelEditor()
{
}