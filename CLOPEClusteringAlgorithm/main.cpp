#include <iostream>

#include "MushroomDataset.h"
#include "CLOPEClusteringAlgorithm.h"

static const float REPULSION = 2.6f;

int main()
{
    MushroomDataset data("./Data/agaricus-lepiota.data");
    unsigned short iterationCount = CLOPEClusteringAlgorithm::Apply(data, REPULSION);
    std::cout << "Iterations: " << iterationCount << std::endl;
    data.Close();   // Результат - в файле dataset_copy.txt.
}
