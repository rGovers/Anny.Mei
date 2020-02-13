#include "Models/MorphPlaneModel.h"

#include <glad/glad.h>

MorphPlaneModel::MorphPlaneModel()
{
    static const size_t vertexSize = sizeof(MorphPlaneModelVertex);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(MorphPlaneModelVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)offsetof(MorphPlaneModelVertex, TexCoord));
    
    static const size_t morphPlaneOffset = offsetof(MorphPlaneModelVertex, MorphPlaneWeights);

    for (int i = 0; i < INDEX_COUNT; ++i)
    {
        glEnableVertexAttribArray(2 + i);

        glVertexAttribPointer(2 + i, 2, GL_FLOAT, false, vertexSize, (void*)(morphPlaneOffset + (sizeof(glm::vec2) * i)));
    }
}

e_ModelType MorphPlaneModel::GetModelType() const
{
    return e_ModelType::MorphPlane;
}