﻿#include <iostream>

#include "Dataset.h"
#include "CLOPEClusteringAlgorithm.h"

static const float REPULSION = 2.6f;

int main()
{
    Dataset data("./Data/agaricus-lepiota.data");
    unsigned short iterationCount = CLOPEClusteringAlgorithm::Apply(data, REPULSION);
    std::cout << "Iterations: " << iterationCount << std::endl;
    data.Close();   // Результат - в файле dataset_copy.txt.
}
