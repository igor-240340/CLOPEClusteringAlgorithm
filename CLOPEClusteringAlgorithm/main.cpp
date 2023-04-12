#include <iostream>

#include "CLOPEClusteringAlgorithm.h"
#include "MushroomDataset.h"
#include "BookDataset.h"

static const float MUSHROOM_REPULSION = 2.6f;

static const float BOOK_REPULSION = 1.3f;
static const float WORD_REPULSION = 2.2f;

void main()
{
    MushroomDataset mushrooms("./Data/agaricus-lepiota.data");
    unsigned short iterationCount = CLOPEClusteringAlgorithm::Perform(mushrooms, MUSHROOM_REPULSION);
    std::cout << "Iterations for mushrooms: " << iterationCount << std::endl;

    BookDataset books("../CLOPEClusteringAlgorithm/Data/books.txt", WORD_REPULSION);
    iterationCount = CLOPEClusteringAlgorithm::Perform(books, BOOK_REPULSION);
    std::cout << "Iterations for books: " << iterationCount << std::endl;
}
