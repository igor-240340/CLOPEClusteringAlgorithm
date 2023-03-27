#pragma once

#include "Dataset.h"

class MushroomDataset : public Dataset
{
public:
    MushroomDataset(std::string filePath);

private:
    virtual void Reformat();
};
