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

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            Logger::WriteMessage("In Class Cleanup");
        }

        //
        // Dataset.h
        //
        TEST_METHOD(Dataset_Constructor)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Assert::IsTrue(std::filesystem::exists("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt"));

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Dataset.h
        // ������ �� ���� �������������.
        TEST_METHOD(Dataset_ReadNextTransaction_WithoutClusterId)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t1;
            Assert::IsTrue(data.ReadNextTransaction(t1));
            std::string expectedItems1 = "x0,y1,b2,t3,n4,f5,c6,b7,e8,e9,s11,s12,e13,w14,p15,w16,t17,e18,w19,c20,w21";
            std::string actualItems1;
            for (auto const& item : t1.items)
                actualItems1 += item + ",";
            actualItems1.pop_back(); // ������� ��������� �������.
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

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Dataset.h
        // ��������� ������ ����� ������������� ����������.
        TEST_METHOD(Dataset_WriteTransaction)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);
            t.clusterId = 1;
            data.WriteTransaction(t);

            data.ReadNextTransaction(t);
            t.clusterId = 2;
            data.WriteTransaction(t);

            data.Reopen();

            data.ReadNextTransaction(t);
            std::string expectedItems1 = "x0,y1,b2,t3,n4,f5,c6,b7,e8,e9,s11,s12,e13,w14,p15,w16,t17,e18,w19,c20,w21";
            std::string actualItems1;
            for (auto const& item : t.items)
                actualItems1 += item + ",";
            actualItems1.pop_back(); // ������� ��������� �������.
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

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Cluster.h
        // �������� ������ ��������.
        TEST_METHOD(Cluster_Constructor)
        {
            Cluster c1;
            Assert::IsTrue(c1.IsEmpty());

            Cluster c2;
            Assert::AreEqual(c2.id, c1.id + 1); // �� ���� ���� �������� ������� ���������� ������, ������� ���������� �������� ��������������� �� ��������.
            Assert::IsTrue(c2.IsEmpty());
        }

        //
        // Cluster.h
        // ������ ������ �������� ������� �������� ��� ���������� ����������.
        TEST_METHOD(Cluster_CalcDeltaQuality_Empty)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            float deltaQualityActual = c.CalcDeltaQuality(t, 2.6f);
            float deltaQualityExpected = t.items.size() / pow(t.items.size(), 2.6f);
            Assert::AreEqual(deltaQualityExpected, deltaQualityActual);

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Cluster.h
        // ���������� ���������� � ������ �������.
        TEST_METHOD(Cluster_Add_Empty)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);
            Assert::IsTrue(!c.IsEmpty());
            Assert::IsTrue(t.clusterId == c.id);

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Cluster.h
        // ������ ������ �������� ��������� �������� ��� ���������� ����������.
        // ���� ������ �������� ��������� �������� ����������, ������ ������� �������� �������� ���� ���� ��������� ���������.
        // �� ����, ���� ���� ����� ���������, ��� ���������� � ������ ������� ��������� ��������� �������� ��������, 
        // ������ ������ � �������� � ��� ������.
        TEST_METHOD(Cluster_CalcDeltaQuality_NotEmpty)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);

            // ��������� ������� �������� ��������, � �������� � ��� ������ ������.
            int transactionCountCurrentExpected = 1;
            int sizeCurrentExpected = t.items.size();
            int widthCurrentExpected = sizeCurrentExpected;
            float qualityCurrentExpected = sizeCurrentExpected * transactionCountCurrentExpected / pow(widthCurrentExpected, 2.6f);

            data.ReadNextTransaction(t);
            float deltaQualityActual = c.CalcDeltaQuality(t, 2.6f);

            // ��������� ����� �������� �������� ����� ���������� ������ ����������.
            int transactionCountNewExpected = 2;
            int sizeNewExpected = t.items.size() * 2;
            int widthNewExpected = 32; // ���������� �������� ��������� (������).
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float deltaQualityExpected = qualityNewExpected - qualityCurrentExpected;
            Assert::AreEqual(deltaQualityExpected, deltaQualityActual);

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Cluster.h
        // ���������� ���������� � �������� �������.
        TEST_METHOD(Cluster_Add_NotEmpty)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);

            data.ReadNextTransaction(t);
            c.Add(t, 2.6f);

            Assert::IsTrue(!c.IsEmpty());
            Assert::IsTrue(t.clusterId == c.id);

            // ����� �������, ��������� �� ����������� ����� �������� �������� ��� ���������� ���������� � �������� �������
            // �� ��������� ������ �������� ��� ���������� ������� ����������.
            // ���� � ���, ��� ���� ���������� ������ ���������� ��������� ��������� ����� �������� ��������,
            // �� �� ������� ���������� ������ ��������� �������� �������� ��� ���������� ������� ����������.

            // ��������� ������� �������� �������� ��� ���� ����������.
            int transactionCountCurrentExpected = 2;
            int sizeCurrentExpected = 21 * 2; // ������ ��� ���������� ����� ���������� ���������� �������.
            int widthCurrentExpected = 32;
            float qualityCurrentExpected = sizeCurrentExpected * transactionCountCurrentExpected / pow(widthCurrentExpected, 2.6f);

            // ��������� ����� �������� �������� ����� ���������� ������� ����������.
            int transactionCountNewExpected = 3;
            int sizeNewExpected = 21 * 2 + 22; // ������ ��� ���������� ����� ���� ����������� �������, ������� � ��� ������� ������ �� 1.
            int widthNewExpected = 41;
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float dQualityExpected = qualityNewExpected - qualityCurrentExpected;

            data.ReadNextTransaction(t);
            float dQualityActual = c.CalcDeltaQuality(t, 2.6f);
            Assert::AreEqual(dQualityExpected, dQualityActual);

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Cluster.h
        // �������� ���������� �� ��������� ��������. ������� ���������� ������.
        TEST_METHOD(Cluster_Remove_Empty)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            Cluster c;
            c.Add(t, 2.6f);

            c.Remove(t, 2.6f);
            Assert::IsTrue(c.IsEmpty());

            // ��� ��, ��� � � ���������� ������, �������� ����������, ��� �������� �������� ��������������� ���������.
            // ��� ����� ������� ������� ���������� ���� �� ���������� ������� � �������.

            // ��������� ������� �������� ������� ��������.
            float qualityCurrentExpected = 0;

            // ��������� ����� �������� �������� ����� ���������� ����������.
            int transactionCountNewExpected = 1;
            int sizeNewExpected = 21;
            int widthNewExpected = 21;
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float dQualityExpected = qualityNewExpected - qualityCurrentExpected;

            float dQualityActual = c.CalcDeltaQuality(t, 2.6f);
            Assert::AreEqual(dQualityExpected, dQualityActual);

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // Cluster.h
        // �������� ���������� �� ��������� ��������. ������� �������� ��������.
        TEST_METHOD(Cluster_Remove_NotEmpty)
        {
            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset2.txt");

            Transaction t;
            data.ReadNextTransaction(t);

            // ��������� ������ ����������.
            Cluster c;
            c.Add(t, 2.6f);

            // ��������� ������ ����������.
            data.ReadNextTransaction(t);
            c.Add(t, 2.6f);

            // ������� ������ ����������.
            c.Remove(t, 2.6f);
            Assert::IsTrue(!c.IsEmpty());

            // �������� ����������, ��� ����� �������� ������ ���������� �������� �������� ��������������� ���������.
            // ������ �������� �������� �������� �������� ��������, ������� ���� ��� ����������, �� � �������� ����������.

            // ��������� ������� �������� �������� ��� ��� ����� ���������� ����������.
            int transactionCountCurrentExpected = 1;
            int sizeCurrentExpected = 21;
            int widthCurrentExpected = 21;
            float qualityCurrentExpected = sizeCurrentExpected * transactionCountCurrentExpected / pow(widthCurrentExpected, 2.6f);

            // ��������� ����� �������� �������� ����� ����������� ������ ���������� ������� � �������.
            int transactionCountNewExpected = 2;
            int sizeNewExpected = 21 * 2;
            int widthNewExpected = 32;
            float qualityNewExpected = sizeNewExpected * transactionCountNewExpected / pow(widthNewExpected, 2.6f);

            float dQualityExpected = qualityNewExpected - qualityCurrentExpected;

            float dQualityActual = c.CalcDeltaQuality(t, 2.6f);
            Assert::AreEqual(dQualityExpected, dQualityActual);

            data.Close();
            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }

        //
        // CLOPEClusteringAlgorithm.h
        // ��������� ��������� �������� ������ �������������� � ���������� 2.6.
        TEST_METHOD(CLOPEAlgorithm_Apply) {
            int iterationCountExpected = 3;
            int clusterIdExpected[] = { 1, 6, 3, 3, 1, 3, 3, 7, 3, 3 }; // ������ - ����� ������ � ����� ��������. ����� - ����� ��������.

            Dataset data("../CLOPEClusteringAlgorithmTest/Fixtures/fake_dataset.txt");
            unsigned short iterationCount = CLOPEClusteringAlgorithm::Apply(data, 2.6f);
            data.Close();
            Assert::IsTrue(iterationCount == iterationCountExpected);

            // ��������� �������� ���� ����� �������������.
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

            std::filesystem::remove("../CLOPEClusteringAlgorithmTest/Fixtures/dataset_copy.txt");
        }
    };
}
