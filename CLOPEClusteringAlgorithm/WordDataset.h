#pragma once

#include "Dataset.h"

class WordDataset : public Dataset
{
public:
    WordDataset(std::string filePath);

private:
    virtual void Reformat();
};
