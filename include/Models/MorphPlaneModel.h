#include "Model.h"

const int INDEX_COUNT = 4;

struct MorphPlaneModelVertex : ModelVertex
{
    glm::vec2 MorphPlaneWeights[INDEX_COUNT];
};

class MorphPlaneModel : public Model
{
private:
    char* m_modelName;
    char* m_morphPlaneName;
protected:

public:
    MorphPlaneModel();

    virtual e_ModelType GetModelType() const;
};