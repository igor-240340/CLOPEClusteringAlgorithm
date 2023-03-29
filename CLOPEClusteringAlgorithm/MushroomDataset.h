#pragma once

#include "Dataset.h"

class MushroomDataset : public Dataset
{
public:
    MushroomDataset(std::string filePath);

protected:
    virtual void PreProcess();
    virtual void PostProcess() {}; // В этом датасете постобработка не требуется.
};
