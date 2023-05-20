#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "search_server.h"

using namespace std;

void TestAddDocuments() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT((server.FindTopDocuments("cat in the"s))[0].id == 42);
    }
}

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
            "Stop words must be excluded from documents"s);
    }
}

//проверка учёта минус-слов
void TestEliminateMinusWordsFromAddedDocumentContent() {
    const int doc_id = 54;
    const string content = "i love practicum and c++"s;
    const vector<int> ratings = { 5, 5, 5 };
    const int doc_id1 = 54;
    const string content1 = "i like practicum and c++"s;
    const vector<int> ratings1 = { 5, 5, 5 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        ASSERT_EQUAL_HINT(server.FindTopDocuments("-love like"s).size(), 1,
            "Documents with minus words must be excluded"s);

    }

}


//проверка правильности сортировки
void TestSortingRel() {
    const int doc_id1 = 12;
    const string content1 = "hi my name is tikatika slim shady";
    const vector<int> ratings1 = { 2, 3, 4 };

    const int doc_id2 = 90;
    const string content2 = "Slim shady has become eminem after his most popular album"s;
    const vector<int> ratings2 = { 2, 3, 4 };

    const int doc_id3 = 45;
    const string content3 = "All i see is you words"s;
    const vector<int> ratings3 = { 2, 3, 4 };

    const int doc_id4 = 42;
    const string content4 = "Hard to study easy to work"s;
    const vector<int> ratings4 = { 2, 3, 4 };

    const int doc_id5 = 40;
    const string content5 = "Drums in this song are very nice"s;
    const vector<int> ratings5 = { 2, 3, 4 };

    const int doc_id6 = 49;
    const string content6 = "I love this gutair"s;
    const vector<int> ratings6 = { 2, 3, 4 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::ACTUAL, ratings4);
        server.AddDocument(doc_id5, content5, DocumentStatus::ACTUAL, ratings5);
        server.AddDocument(doc_id6, content6, DocumentStatus::ACTUAL, ratings6);
        //проверяем, что было отобрано только 4 документа из 6
        auto result = server.FindTopDocuments("slim shady love nice");
        ASSERT_EQUAL(result.size(), 4);
        //проверяем, что они находятся в нужной последовательности
        bool isright = true;
        for (size_t i = 0; i < result.size(); i++) {
            if (result[i].relevance < result[i + 1].relevance) {
                ASSERT(isright = false);
            }
        }
    }
}

//Проверка правильности подсчёта рейтинга
void TestCountingRating() {
    SearchServer server;
    const int doc_id1 = 12;
    const string content1 = "hi my name is tikatika slim shady";
    const vector<int> ratings1 = { 2, 3, 4 };
    {
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        ASSERT((server.FindTopDocuments("tikatika slim shady"s)).at(doc_id1).rating == (2 + 3 + 4) / 3);


    }
}

//проверка поиска по определенному статусу
void TestStatus() {
    const int doc_id1 = 12;
    const string content1 = "hi my name is tikatika slim shady";
    const vector<int> ratings1 = { 2, 3, 4 };

    const int doc_id2 = 90;
    const string content2 = "Slim shady has become eminem after his most popular album"s;
    const vector<int> ratings2 = { 2, 3, 4 };

    const int doc_id3 = 45;
    const string content3 = "All i see is you words"s;
    const vector<int> ratings3 = { 2, 3, 4 };

    const int doc_id4 = 48;
    const string content4 = "wow it is timati"s;
    const vector<int> ratings4 = { 2, 3, 4 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::BANNED, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::ACTUAL, ratings4);

        ASSERT_EQUAL((server.FindTopDocuments("wow it is timati"s, DocumentStatus::BANNED)).size(), 1);
    }
}
/*
Разместите код остальных тестов здесь
*/



// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestAddDocuments);
    RUN_TEST(TestEliminateMinusWordsFromAddedDocumentContent);
    RUN_TEST(TestCountingRating);
    RUN_TEST(TestSortingRel);
    RUN_TEST(TestStatus);
    // Не забудьте вызывать остальные тесты здесь
}



// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}
