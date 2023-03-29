#pragma once

#include "Dataset.h"

class BookDataset : public Dataset
{
public:
    BookDataset(std::string filePath);

    void PostProcess();

private:
    virtual void Reformat();
};
