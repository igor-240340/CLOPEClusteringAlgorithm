#pragma once

#include "Dataset.h"

class MushroomDataset : public Dataset
{
public:
    MushroomDataset(std::string filePath);
    ~MushroomDataset();

protected:
    virtual void PreProcess();
    virtual void PostProcess();
};
