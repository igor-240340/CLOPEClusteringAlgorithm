#include <sstream>

#include "BookDataset.h"
#include "WordDataset.h"
#include "CLOPEClusteringAlgorithm.h"

//
//
//
BookDataset::BookDataset(std::string filePath, const float wordRepulsion) : Dataset(filePath) {
    this->wordRepulsion = wordRepulsion;

    PreProcess();
    Reopen();
}

//
//
//
void BookDataset::PreProcess() {
    std::filesystem::path wordsFilePath = SplitNamesByWords();
    std::filesystem::path clusteredWordsFilePath = ClusterWords(wordsFilePath);
    ReplaceWordsWithClusterNames(clusteredWordsFilePath);

    // Удаляем исходный файл со словами, а кластеризованные слова (файл clustered_words) оставляем, чтобы можно было изучить.
    std::filesystem::remove(wordsFilePath);
}

//
// Удаляет из названия каждой книги список авторов и разбивает название слова, разделенные запятой.
//
std::filesystem::path BookDataset::SplitNamesByWords() {
    // Файл со словами будет лежать там же, где файл текущего датасета с названиями книг.
    std::filesystem::path wordsFilePath = std::filesystem::path(sourceFilePath).replace_filename("words.txt");
    std::ofstream wordsFile;
    wordsFile.open(wordsFilePath);
    if (wordsFile.fail())
        exit(1);

    do {
        std::string bookName;
        std::getline(fileIn, bookName);

        if (bookName == "")
            break;

        bookName = RemoveAuthorsFromName(bookName);

        // Разделяем слова в названии запятыми и попутно формируем датасет слов.
        std::stringstream bookNameStream(bookName);
        std::string splittedBookName;
        while (bookNameStream.good()) {
            std::string word;
            std::getline(bookNameStream, word, ' ');

            word = CleanWord(word);

            // Удаляем строки наподобие "1.".
            // Также удаляем строки, начинающиеся с цифры,
            // чтобы не спутать потом элемент транзакции с номером кластера.
            bool wordIsNumber = word.c_str()[0] >= 48 && word.c_str()[0] <= 57;
            if (word.length() == 1 || wordIsNumber)
                continue;

            splittedBookName += word + ",";

            wordsFile << word << std::endl;
        }
        splittedBookName.pop_back(); // Удаляем последнюю запятую.

        fileOut << splittedBookName << std::endl;
    } while (true);
    wordsFile.close();

    Reopen();

    return wordsFilePath;
}

//
// Удаляет с конца названия часть строки вида: "Петров, Иванов.pdf".
// Метод примитивный - читаем посимвольно с конца строки до второй точки
// и используем номер её позиции для обрезания строки.
// 
// Предполагается, что любое название содержит список авторов.
//
std::string BookDataset::RemoveAuthorsFromName(std::string bookName) {
    std::string::reverse_iterator secondDotIt;
    unsigned short dotCount = 0;
    for (secondDotIt = bookName.rbegin(); secondDotIt != bookName.rend(); secondDotIt++) {
        if (*secondDotIt == '.' && ++dotCount == 2)
            break;
    }

    bookName.erase(secondDotIt.base(), bookName.end());

    return bookName;
}

//
// На данный момент удаляет из слова только запятые и точки,
// поскольку запятая используется как разделитель элементов транзакции.
// Из искаженного представления кириллицы запятые (0x82) также удаляются.
// На кластеризацию это не влияет - одинаковые слова будут преобразованы одинаковым образом и все-равно попадут в один кластер.
// 
// NOTE: наверное имеет смысл работать в бинарном режиме.
//
std::string BookDataset::CleanWord(std::string word) {
    std::string cleaned;
    for (char& ch : word) {
        if (ch != ',' && ch != '.' && ch != (char)0x82)
            cleaned += ch;
    }

    return cleaned;
}

//
//
//
std::filesystem::path BookDataset::ClusterWords(std::filesystem::path wordsFilePath) {
    WordDataset words(wordsFilePath.string());
    CLOPEClusteringAlgorithm::Perform(words, wordRepulsion);

    // Кластеризованные слова будут находиться в копии исходного файла.
    return std::filesystem::path(wordsFilePath).replace_filename("clustered_" + wordsFilePath.filename().string());
}

//
// Заменяет каждое слово на кластер, которому это слово принадлежит, добавляя к номеру кластера символ,
// чтобы мы не спутали элемент транзакции и номер кластера, к которому относится книжка.
//
void BookDataset::ReplaceWordsWithClusterNames(std::filesystem::path clusteredWordsFilePath) {
    // Формируем отображение слов в идентификаторы их кластеров.
    std::ifstream clusteredWordsFile;
    clusteredWordsFile.open(clusteredWordsFilePath);
    if (clusteredWordsFile.fail())
        exit(1);

    std::unordered_map<std::string, std::string> wordToClusterIdStr;
    do {
        std::string line;
        std::getline(clusteredWordsFile, line);

        if (line == "")
            break;

        std::stringstream lineStream(line);
        while (lineStream.good()) {
            std::string word;
            std::string clusterIdStr;

            std::getline(lineStream, word, ',');
            std::getline(lineStream, clusterIdStr, ',');

            wordToClusterIdStr[word] = clusterIdStr;
        }
    } while (true);
    clusteredWordsFile.close();

    // Заменяем слова в строках на названия кластеров.
    do {
        std::string line;
        std::getline(fileIn, line);

        if (line == "")
            break;

        std::stringstream lineStream(line);
        std::string modifiedLine;
        std::unordered_map<std::string, unsigned short> clusterIdStrToCount;
        while (lineStream.good()) {
            std::string word;
            std::getline(lineStream, word, ',');

            // В названии одной книги может встретиться несколько слов из одного кластера,
            // поэтому удаляем дубликаты чтобы не завышать оценку качества кластера.
            std::string clusterIdStr = wordToClusterIdStr[word];
            if (++clusterIdStrToCount[clusterIdStr] > 1) {
                continue;
            }

            std::string wordClusterName = "c" + clusterIdStr;

            modifiedLine += wordClusterName;
            modifiedLine += ",";
        }
        modifiedLine.pop_back();

        fileOut << modifiedLine << std::endl;
    } while (true);
}

//
//
//
BookDataset::~BookDataset() {
    Reopen();
    PostProcess();
}

//
//
//
void BookDataset::PostProcess() {
    std::ifstream booksFile;
    booksFile.open(sourceFilePath);

    if (booksFile.fail())
        exit(1);

    Transaction t;
    while (ReadNextTransaction(t)) {
        std::string bookName;
        std::getline(booksFile, bookName);

        // Обрамляем название книги в кавычки, чтобы сформировать CSV-совместимый формат.
        fileOut << "\"" + bookName + "\"" + "," + std::to_string(t.clusterId) << std::endl;
    }

    booksFile.close();
}
