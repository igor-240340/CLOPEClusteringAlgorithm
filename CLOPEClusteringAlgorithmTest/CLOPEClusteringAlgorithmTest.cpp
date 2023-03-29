#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CLOPEClusteringAlgorithmTest
{
    TEST_CLASS(CLOPEClusteringAlgorithmTest)
    {
    private:
        std::filesystem::path currentPath;

    public:
        CLOPEClusteringAlgorithmTest()
        {
            Logger::WriteMessage("In CLOPEClusteringAlgorithmTest");

            currentPath = std::filesystem::current_path();
        }

        ~CLOPEClusteringAlgorithmTest()
        {
            Logger::WriteMessage("In ~CLOPEClusteringAlgorithmTest");
        }

        TEST_CLASS_INITIALIZE(ClassInitialize)
        {
            Logger::WriteMessage("In Class Initialize");
        }

        TEST_METHOD_INITIALIZE(MethodInitialize)
        {
            Logger::WriteMessage("In MethodInitialize");
        }

        TEST_METHOD_CLEANUP(MethodCleanup)
        {
            Logger::WriteMessage("In MethodCleanup");

            // Файл dataset_copy.txt после работы всегда остается в файловой системе.
            // Закрытие файлов происходит в деструкторе, поэтому подчищаем за датасетом уже в этом методе,
            // когда для объекта вызван деструктор и файлы закрыты.
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            Logger::WriteMessage("In ClassCleanup");
        }

        //
        // MushroomDataset.h
        //
        TEST_METHOD(MushroomDataset_Constructor)
        {
            // Здесь мы проверяем в т.ч. и работу деструктора, поэтому создаем scope.
            // Закрытие файлов и удаление временного файла происходит в деструкторе.
            {
                MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

                Assert::IsTrue(std::filesystem::exists("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt"));
                Assert::IsTrue(std::filesystem::exists("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy_tmp.txt"));
            }

            Assert::IsTrue(std::filesystem::exists("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt"));
            Assert::IsFalse(std::filesystem::exists("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy_tmp.txt"));
        }

        //
        // MushroomDataset.h
        // Чтение на фазе инициализации.
        TEST_METHOD(MushroomDataset_ReadNextTransaction_WithoutClusterId)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t1;
            Assert::IsTrue(data.ReadNextTransaction(t1));
            std::string expectedItems1 = "x0,y1,b2,t3,n4,f5,c6,b7,e8,e9,s11,s12,e13,w14,p15,w16,t17,e18,w19,c20,w21";
            std::string actualItems1;
            for (auto const& item : t1.items)
                actualItems1 += item + ",";
            actualItems1.pop_back(); // Удаляет последнюю запятую.
            Assert::AreEqual(expectedItems1, actualItems1);

            Transaction t2;
            Assert::IsTrue(data.ReadNextTransaction(t2));
            std::string expectedItems2 = "x0,y1,e2,f3,y4,f5,c6,n7,b8,t9,k11,s12,w13,w14,p15,w16,o17,e18,w19,v20,p21";
            std::string actualItems2;
            for (auto const& item : t2.items)
                actualItems2 += item + ",";
            actualItems2.pop_back();
            Assert::AreEqual(expectedItems2, actualItems2);

            Transaction t3;
            Assert::IsTrue(data.ReadNextTransaction(t3));

            Transaction t4;
            Assert::IsFalse(data.ReadNextTransaction(t4));
        }

        //
        // MushroomDataset.h
        // Повторное чтение после кластеризации транзакций.
        TEST_METHOD(MushroomDataset_WriteTransaction)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);
            t.clusterId = 1;
            data.WriteTransaction(t);

            data.ReadNextTransaction(t);
            t.clusterId = 2;
            data.WriteTransaction(t);

            data.Rewind();

            data.ReadNextTransaction(t);
            std::string expectedItems1 = "x0,y1,b2,t3,n4,f5,c6,b7,e8,e9,s11,s12,e13,w14,p15,w16,t17,e18,w19,c20,w21";
            std::string actualItems1;
            for (auto const& item : t.items)
                actualItems1 += item + ",";
            actualItems1.pop_back(); // Удаляет последнюю запятую.
            Assert::AreEqual(expectedItems1, actualItems1);
            Assert::IsTrue(t.clusterId == 1);

            data.ReadNextTransaction(t);
            std::string expectedItems2 = "x0,y1,e2,f3,y4,f5,c6,n7,b8,t9,k11,s12,w13,w14,p15,w16,o17,e18,w19,v20,p21";
            std::string actualItems2;
            for (auto const& item : t.items)
                actualItems2 += item + ",";
            actualItems2.pop_back();
            Assert::AreEqual(expectedItems2, actualItems2);
            Assert::IsTrue(t.clusterId == 2);
        }

        //
        // Cluster.h
        // Создание нового кластера.
        TEST_METHOD(Cluster_Constructor)
        {
            Cluster c1;
            Assert::IsTrue(c1.IsEmpty());

            Cluster c2;
            Assert::AreEqual(c2.id, c1.id + 1); // Не могу пока изменить порядок выполнения тестов, поэтому абсолютные значения идентификаторов не проверяю.
            Assert::IsTrue(c2.IsEmpty());
        }

        //
        // Cluster.h
        // Оценка дельты качества пустого кластера при добавлении транзакции.
        TEST_METHOD(Cluster_CalcDeltaQuality_Empty)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            float deltaQualityActual = c.CalcDeltaQuality(t, 2.6f);
            float deltaQualityExpected = t.items.size() / pow(t.items.size(), 2.6f);
            Assert::AreEqual(deltaQualityExpected, deltaQualityActual);
        }

        //
        // Cluster.h
        // Добавление транзакции в пустой кластер.
        TEST_METHOD(Cluster_Add_Empty)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);
            Assert::IsTrue(!c.IsEmpty());
            Assert::IsTrue(t.clusterId == c.id);
        }

        //
        // Cluster.h
        // Оценка дельты качества непустого кластера при добавлении транзакции.
        // Если дельта качества непустого кластера правильная, значит текущее качество кластера тоже было вычислено правильно.
        // То есть, этот тест также проверяет, что добавление в пустой кластер правильно вычисляет качество кластера, 
        // прямой доступ к которому у нас закрыт.
        TEST_METHOD(Cluster_CalcDeltaQuality_NotEmpty)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);

            // Ожидаемое текущее качество кластера, к которому у нас закрыт доступ.
            int transactionCountCurrentExpected = 1;
            int sizeCurrentExpected = t.items.size();
            int widthCurrentExpected = sizeCurrentExpected;
            float qualityCurrentExpected = sizeCurrentExpected * transactionCountCurrentExpected / pow(widthCurrentExpected, 2.6f);

            data.ReadNextTransaction(t);
            float deltaQualityActual = c.CalcDeltaQuality(t, 2.6f);

            // Ожидаемое новое качество кластера после добавления второй транзакции.
            int transactionCountNewExpected = 2;
            int sizeNewExpected = t.items.size() * 2;
            int widthNewExpected = 32; // Уникальные значения атрибутов (айтемы).
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float deltaQualityExpected = qualityNewExpected - qualityCurrentExpected;
            Assert::AreEqual(deltaQualityExpected, deltaQualityActual);
        }

        //
        // Cluster.h
        // Добавление транзакции в непустой кластер.
        TEST_METHOD(Cluster_Add_NotEmpty)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);

            data.ReadNextTransaction(t);
            c.Add(t, 2.6f);

            Assert::IsTrue(!c.IsEmpty());
            Assert::IsTrue(t.clusterId == c.id);

            // Чтобы оценить, правильно ли вычисляется новое качество кластера при добавлении транзакции в непустой кластер
            // мы оцениваем дельту качества при добавлении третьей транзакции.
            // Дело в том, что если добавление второй транзакции правильно вычисляет новое качество кластера,
            // то мы получим правильную оценку изменения качества кластера при добавлении третьей транзакции.

            // Ожидаемое текущее качество кластера для двух транзакций.
            int transactionCountCurrentExpected = 2;
            int sizeCurrentExpected = 21 * 2; // Первые две транзакции имеют одинаковое количество айтемов.
            int widthCurrentExpected = 32;
            float qualityCurrentExpected = sizeCurrentExpected * transactionCountCurrentExpected / pow(widthCurrentExpected, 2.6f);

            // Ожидаемое новое качество кластера после добавления третьей транзакции.
            int transactionCountNewExpected = 3;
            int sizeNewExpected = 21 * 2 + 22; // Первые две транзакции имеют один пропущенный атрибут, поэтому у них айтемов меньше на 1.
            int widthNewExpected = 41;
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float dQualityExpected = qualityNewExpected - qualityCurrentExpected;

            data.ReadNextTransaction(t);
            float dQualityActual = c.CalcDeltaQuality(t, 2.6f);
            Assert::AreEqual(dQualityExpected, dQualityActual);
        }

        //
        // Cluster.h
        // Удаление транзакции из непустого кластера. Кластер становится пустым.
        TEST_METHOD(Cluster_Remove_Empty)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);

            c.Remove(t, 2.6f);
            Assert::IsTrue(c.IsEmpty());

            // Так же, как и в предыдущих тестах, косвенно убеждаемся, что качество кластера пересчитывается кооректно.
            // Для этого пробуем оценить добавление этой же транзакции обратно в кластер.

            // Ожидаемое текущее качество пустого кластера.
            float qualityCurrentExpected = 0;

            // Ожидаемое новое качество кластера после добавления транзакции.
            int transactionCountNewExpected = 1;
            int sizeNewExpected = 21;
            int widthNewExpected = 21;
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float dQualityExpected = qualityNewExpected - qualityCurrentExpected;

            float dQualityActual = c.CalcDeltaQuality(t, 2.6f);
            Assert::AreEqual(dQualityExpected, dQualityActual);
        }

        //
        // Cluster.h
        // Удаление транзакции из непустого кластера. Кластер остается непустым.
        TEST_METHOD(Cluster_Remove_NotEmpty)
        {
            MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            // Добавляет первую транзакцию.
            Cluster c;
            c.Add(t, 2.6f);

            // Добавляет вторую транзакцию.
            data.ReadNextTransaction(t);
            c.Add(t, 2.6f);

            // Удаляет вторую транзакцию.
            c.Remove(t, 2.6f);
            Assert::IsTrue(!c.IsEmpty());

            // Косвенно убеждаемся, что после удаления второй транзакции качество кластера пересчитывается корректно.
            // Дельта качества является функцией текущего качества, поэтому если она корректная, то и качество корректное.

            // Ожидаемое текущее качество кластера для для одной оставшейся транзакции.
            int transactionCountCurrentExpected = 1;
            int sizeCurrentExpected = 21;
            int widthCurrentExpected = 21;
            float qualityCurrentExpected = sizeCurrentExpected * transactionCountCurrentExpected / pow(widthCurrentExpected, 2.6f);

            // Ожидаемое новое качество кластера после возвращения второй транзакции обратно в кластер.
            int transactionCountNewExpected = 2;
            int sizeNewExpected = 21 * 2;
            int widthNewExpected = 32;
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float dQualityExpected = qualityNewExpected - qualityCurrentExpected;

            float dQualityActual = c.CalcDeltaQuality(t, 2.6f);
            Assert::AreEqual(dQualityExpected, dQualityActual);
        }

        //
        // CLOPEClusteringAlgorithm.h
        // Ожидаемые параметры получены ручной кластеризацией с параметром 2.6.
        TEST_METHOD(CLOPEAlgorithm_Apply) {
            int iterationCountExpected = 3;
            int clusterIdExpected[] = { 1, 6, 3, 3, 1, 3, 3, 7, 3, 3 }; // Индекс - номер строки в файле датасета. Число - номер кластера.

            // Нам необходимо заглянуть в выходной файл и проверить результаты, а закрытие происходит в деструкторе,
            // Поэтому создаем scope, чтобы спровоцировать вызов деструктора, а значит и закрытие обоих и удаление временного файла.
            {
                MushroomDataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset.txt");
                unsigned int iterationCount = CLOPEClusteringAlgorithm::Apply(data, 2.6f);
                Assert::IsTrue(iterationCount == iterationCountExpected);
            }

            // Проверяет выходной файл после кластеризации.
            std::fstream resultFile;
            resultFile.open("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
            if (resultFile.fail())
                exit(1);

            int transactionIndex = 0;
            do {
                std::string line;
                resultFile >> line;

                if (line == "")
                    break;

                std::stringstream ss(line);
                std::string itemValue;
                while (ss.good()) {
                    std::getline(ss, itemValue, ',');

                    if (itemValue.c_str()[0] >= 48 && itemValue.c_str()[0] <= 57) {
                        int clusterIdActual = std::stoi(itemValue.c_str(), nullptr, 10);
                        Assert::AreEqual(clusterIdExpected[transactionIndex++], clusterIdActual);
                    }
                }
            } while (true);
            resultFile.close();
        }
    };
}
