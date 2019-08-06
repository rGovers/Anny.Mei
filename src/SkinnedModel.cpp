#include "Models/SkinnedModel.h"

#include <glad/glad.h>

SkinnedModel::SkinnedModel() : Model()
{
    const size_t vertexSize = sizeof(SkinnedVertex);

    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(SkinnedVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)offsetof(SkinnedVertex, TexCoord));
    glVertexAttribPointer(2, 2, GL_INT,   false, vertexSize, (void*)offsetof(SkinnedVertex, Index));
    glVertexAttribPointer(3, 2, GL_FLOAT, false, vertexSize, (void*)offsetof(SkinnedVertex, Weight));
}