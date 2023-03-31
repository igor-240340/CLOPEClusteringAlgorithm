#pragma once

#include <filesystem>

#include "Dataset.h"

class BookDataset : public Dataset
{
public:
    BookDataset(std::string filePath, float wordRepulsion);
    ~BookDataset();

protected:
    virtual void PreProcess();
    virtual void PostProcess();

private:
    std::filesystem::path SplitNamesByWords();
    std::filesystem::path ClusterWords(std::filesystem::path wordsFilePath);
    void ReplaceWordsWithClusterNames(std::filesystem::path clusteredWordsFilePath);

    std::string CleanWord(std::string word);
    std::string RemoveAuthorsFromName(std::string bookName);

    float wordRepulsion;
};
