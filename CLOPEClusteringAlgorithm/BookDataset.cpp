#include <sstream>

#include "BookDataset.h"
#include "WordDataset.h"
#include "CLOPEClusteringAlgorithm.h"

//
//
//
BookDataset::BookDataset(std::string filePath) : Dataset(filePath) {
    Open();
    Reformat();
    Reopen();
}

//
//
//
void BookDataset::Reformat() {
    std::ofstream wordDatasetFile;
    std::fstream rawWordsFile;
    std::ofstream clusteredWords;

    std::string wordDatasetFilePath = "D:/Temp/words.txt";
    std::string rawWordsFilePath = "D:/Temp/raw-words.txt";
    std::string clusteredWordsPath = "D:/Temp/clustered-words.txt";

    wordDatasetFile.open(wordDatasetFilePath);
    rawWordsFile.open(rawWordsFilePath, std::ios_base::out);
    clusteredWords.open(clusteredWordsPath);

    if (wordDatasetFile.fail() || rawWordsFile.fail() || clusteredWords.fail())
        exit(1);

    char c;
    std::string origLine;
    while (fileIn.get(c)) {
        if (c != '\n') {
            origLine += c;
        }
        else {
            // Удаляем лишние пробелы справа от названия книги (лишние пробелы - результат работы PowerShell-команды, которая собирает названия).
            // Я решил не тратить время на дальнейшее углубление в PowerShell, поэтому весь парсинг находится здесь.
            std::string::iterator p;
            for (p = origLine.end(); p != origLine.begin() && *--p == ' ';) {
                if (*p != ' ')
                    p++;

                origLine.erase(p, origLine.end());
            }

            // Разбиваем название на отдельные слова, разделенные запятыми.
            std::stringstream ss(origLine);
            std::string reformattedLine;
            std::string item;
            while (ss.good()) {
                std::getline(ss, item, ' ');

                // Формируем датасет из отдельных слов,
                // к которому мы также применим кластеризацию CLOPE,
                // чтобы сгруппировать похожие слова между собой,
                // чтобы потом в оригинальном датасете заменить разные по написанию но близкие по смыслу слова
                // на некий общий символ.
                // Таким образом, если две книги содержат в своих названиях однокоренные слова, в которых варьирует, скажем, окончание,
                // слова эти будут заменены на номер кластера и мы получим в пересечении названий одинаковый item получив перекрытие между названиями.
                // TODO: описать более вразумительно.

                // Разделяем буквы в слове запятыми.
                // Реализация метода Reformat в WordDataset затем проставит каждой букве уникальный номер.
                // Уникальные номера букв необходимы, ведь два совершенно неродственных слова могут быть составлены из одинаковых символов алфавита,
                // но это не значит, что между словами есть перекрытие,
                // поэтому важно учитывать и позицию символа в слове.
                if (item != "") {
                    std::string itemCommaSeparated;
                    for (char& ch : item) {
                        itemCommaSeparated += ch;
                        itemCommaSeparated += ',';
                    }
                    itemCommaSeparated.pop_back();
                    wordDatasetFile << itemCommaSeparated << std::endl;

                    // Формируем файл с исходными словами, чтобы потом сопоставить кластеризацию и слова.
                    rawWordsFile << item << std::endl;
                }

                reformattedLine += item + ',';
            }
            reformattedLine.pop_back();

            fileOut << reformattedLine << std::endl;
            origLine = "";
        }
    }
    wordDatasetFile.close();
    rawWordsFile.close();

    // Группируем близкие слова.
    WordDataset words(wordDatasetFilePath);
    // Нужно учитывать, что при большом значении, сгруппируются только очень близкие слова, вплоть до одинаковых.
    // Поэтому управлять параметром отталкивания нужно аккуратно.
    unsigned short iterations = CLOPEClusteringAlgorithm::Apply(words, 3.0f);

    // Формируем хэш-таблицу исходных слов и номеров кластеров.
    // Поскольку номер кластера становится элементом транзакции, добавляем ему символьный префикс, чтобы различать номер кластера уже в самом названии.
    words.Reopen();
    Transaction wordTransac;
    // В файле кластеризованных слов символы слов разделены запятыми, а нам нужно восстановить их исходное представление, чтобы получить соответствие слово->номер кластера.
    // Читая n-ю транзакцию, мы по сути читаем n-ю строку файла.
    // Поэтому мы можем попутно открыть файл с исходными словами и читать его последовательно вместе с кластеризованным.
    rawWordsFile.open(rawWordsFilePath, std::ios_base::in);
    std::string origWord;
    std::unordered_map<std::string, std::string> wordToClusterId;
    while (words.ReadNextTransaction(wordTransac)) {
        unsigned short clusterId = wordTransac.clusterId;
        rawWordsFile >> origWord;

        wordToClusterId[origWord] = "c" + std::to_string(clusterId); // Добавляем символ "c", чтобы различать номер кластера и элемент транзакции.

        clusteredWords << "\"" + origWord + "\"," + std::to_string(clusterId) << std::endl;
    }
    rawWordsFile.close();
    clusteredWords.close();
    words.Close();

    // Заменяем слова на названия кластеров,
    // чтобы получить перекрытие между названиями книг с разными по написанию,
    // но близкими по смыслу словами.
    Reopen(); // Переоткрываем текущий датасет.
    do {
        std::string srcLine;
        fileIn >> srcLine;

        if (srcLine == "")
            break;

        std::stringstream ss(srcLine);
        std::string reformattedLine;
        std::string itemValue;
        while (ss.good()) {
            std::getline(ss, itemValue, ',');

            // Заменяем слово на кластер.
            std::string wordClusterName = wordToClusterId[itemValue];
            reformattedLine += wordClusterName + ",";
        }
        reformattedLine.pop_back(); // Удаляет последнюю запятую.

        fileOut << reformattedLine << std::endl;
    } while (true);
}

//
//
//
void BookDataset::PostProcess() {
    Close();
    std::filesystem::remove(fileOutPath);
    Open();

    std::ifstream origDatasetFile;
    origDatasetFile.open(std::filesystem::path(fileInPath).replace_filename("my-books-clean.txt"));

    if (origDatasetFile.fail())
        exit(1);

    Transaction t;
    while (ReadNextTransaction(t)) {
        unsigned int clusterId = t.clusterId;

        // Названия в датасете содержат лишние пробелы после PowerShell, поэтому читаем как и в PreProcess.
        char c;
        std::string origBookName;
        while (origDatasetFile.get(c)) {
            if (c != '\n') {
                origBookName += c;
            }
            else { // Дочитали до \n
                // Удаляем лишние пробелы справа от названия книги (лишние пробелы - результат работы PowerShell-команды, которая собирает названия).
                // Я решил не тратить время на дальнейшее углубление в PowerShell, поэтому весь парсинг находится здесь.
                std::string::iterator p;
                for (p = origBookName.end(); p != origBookName.begin() && *--p == ' ';) {
                    if (*p != ' ')
                        p++;

                    origBookName.erase(p, origBookName.end());
                }

                break;
            }
        }

        fileOut << "\"" + origBookName + "\"" + ',' + std::to_string(clusterId) << std::endl;
    }
    origDatasetFile.close();

    Reopen();
}
