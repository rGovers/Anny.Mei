#pragma once

#include "Model.h"

const int INDEX_COUNT = 4;

struct MorphPlaneModelVertex
{
    glm::vec2 MorphPlaneWeights[INDEX_COUNT];
};

class MorphPlaneModel : public Model
{
private:
    unsigned int m_vao;
    unsigned int m_vbo;
protected:

public:
    MorphPlaneModel();
    MorphPlaneModel(const Model& a_model);
    virtual ~MorphPlaneModel();

    virtual unsigned int GetVAO() const;
    unsigned int GetMorphVBO() const;

    virtual e_ModelType GetModelType() const;
};