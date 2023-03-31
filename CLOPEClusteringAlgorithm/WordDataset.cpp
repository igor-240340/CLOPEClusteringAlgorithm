#include "WordDataset.h"

//
//
//
WordDataset::WordDataset(std::string filePath) : Dataset(filePath) {
    PreProcess();
    Reopen();
}

//
//
//
void WordDataset::PreProcess() {
    do {
        std::string word;
        std::getline(fileIn, word);

        if (word == "")
            break;

        // –аздел€ем буквы зап€тыми, присваива€ им уникальные номера.
        // ћы смотрим на слова как на грибы, а на позиции символов - как на атрибуты грибов, а на сами символы в позици€х - как на значени€ атрибутов.
        // ƒва совершенно неродственных слова могут быть составлены из одинаковых символов алфавита,
        // поэтому одинаковые символы в одинаковых позици€х должны совпадать.
        // ј те же одинаковые символы, но в разных позици€х - различатьс€.
        unsigned short attributeIndex = 0;
        std::string wordCommaSeparated;
        for (char& ch : word) {
            wordCommaSeparated += ch;
            wordCommaSeparated += std::to_string(attributeIndex++);
            wordCommaSeparated += ',';
        }
        wordCommaSeparated.pop_back();

        fileOut << wordCommaSeparated << std::endl;
    } while (true);
}

//
//
//
WordDataset::~WordDataset() {
    Reopen();
    PostProcess();
}

//
// Ѕерет строки исходного файла и проставл€ет им номера кластеров.
// —лова кавычками не обрамл€ем, потому что их значени€ нам нужны в чистом виде дл€ сопоставлени€ в датасете BookDataset.
//
void WordDataset::PostProcess() {
    std::ifstream wordsFile;
    wordsFile.open(sourceFilePath);

    if (wordsFile.fail())
        exit(1);

    Transaction t;
    while (ReadNextTransaction(t)) {
        std::string word;
        std::getline(wordsFile, word);
        fileOut << word + "," + std::to_string(t.clusterId) << std::endl;
    }

    wordsFile.close();
}
