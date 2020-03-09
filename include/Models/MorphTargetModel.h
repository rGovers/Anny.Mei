#pragma once

#include "Model.h"

class MorphTargetModel : public Model
{
private:
    unsigned int  m_vao;
    unsigned int* m_vbo;
protected:

public:
    MorphTargetModel();
    MorphTargetModel(const Model& a_model);
    virtual ~MorphTargetModel();

    virtual unsigned int GetVAO() const;
    unsigned int GetMorphVBO(int a_index) const;

    virtual e_ModelType GetModelType() const;
};