#pragma once

#include "Dataset.h"

class WordDataset : public Dataset
{
public:
    WordDataset(std::string filePath);
    ~WordDataset();

protected:
    virtual void PreProcess();
    virtual void PostProcess();
};
