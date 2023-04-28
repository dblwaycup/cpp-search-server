#include <algorithm> 
#include <cmath> 
#include <iostream> 
#include <map> 
#include <set> 
#include <string> 
#include <utility> 
#include <vector> 
#include <numeric>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

enum class StatusOfDocuments {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
    int rating;

};

struct DefaultStatus {
    StatusOfDocuments status;
    int rating;
};

class SearchServer {
public:

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, StatusOfDocuments status, const vector<int>& ratings) {
        ++document_count_;
        DefaultStatus status_doc;
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double tf_count = 1.0 / words.size();
        for (const string& word : words) {
            documents_[word][document_id] += tf_count;
        }
        document_ratings_[document_id] = ComputeAverageRating(ratings);
        status_doc.status = status;
        status_doc.rating = ComputeAverageRating(ratings);
        status_documents_.emplace(document_id, status_doc);
    }

    vector<Document> FindTopDocuments(const string& raw_query, StatusOfDocuments status = StatusOfDocuments::ACTUAL) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);
        vector<Document> new_status_documents;
        for (const Document& document : matched_documents) {
            if (status_documents_.at(document.id).status == status) {
                new_status_documents.push_back(document);
            }
        }

        sort(new_status_documents.begin(), new_status_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return new_status_documents;
    }

private:

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    set<string> stop_words_;
    map<string, map<int, double>> documents_;
    map<int, int> document_ratings_;
    map<int, DefaultStatus> status_documents_;
    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word) || !IsStopWord(word.substr(1))) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if ((word[0] == '-') && (!IsStopWord(word.substr(1)))) {
                query_words.minus_words.insert(word.substr(1));
            }
            else {
                query_words.plus_words.insert(word);
            }
        }
        return query_words;
    }

    double RevelanceSum(const string& word) const {
        return log(document_count_ * 1.0 / documents_.at(word).size());
    }

    vector<Document> FindAllDocuments(const Query& query) const {
        map<int, double> document_to_relevance;
        vector<Document> answer_document;
        map<int, int> rating_docs;
        int rating;
        for (const string& word : query.plus_words) {
            if (documents_.count(word) == 0) {
                continue;
            }
            const double complete_count = RevelanceSum(word);
            for (const auto [document_id, count_relevance] : documents_.at(word)) {
                rating = document_ratings_.at(document_id);
                rating_docs.insert({ document_id, rating });
                document_to_relevance[document_id] += count_relevance * complete_count;
            }
        }

        for (const string& word : query.minus_words) {
            if (documents_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : documents_.at(word)) {
                document_to_relevance.erase(document_id);
                rating_docs.erase(document_id);
            }
        }
        for (const auto& [document_id, relevance] : document_to_relevance) {
            answer_document.push_back({ document_id, relevance, rating_docs.at(document_id) });
        }
        return answer_document;
    }
    int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int sum = 0;
        for (const int& num : ratings) {
            sum += num;
        }
        return (sum - ratings[0]) / static_cast<int>(ratings.size() - 1);
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    vector<int> rating;
    search_server.SetStopWords(ReadLine());
    string document = ""s;
    StatusOfDocuments status = StatusOfDocuments::ACTUAL;

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        document = ReadLine();
        for (const string& text_rating : SplitIntoWords(ReadLine())) {
            rating.push_back(stoi(text_rating));
        }
        search_server.AddDocument(document_id, document, status, rating);
        rating.clear();
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    StatusOfDocuments status = StatusOfDocuments::ACTUAL;

    const string query = ReadLine();
    for (const auto& [document_id, relevance, rating] : search_server.FindTopDocuments(query, status)) {
        cout << "{ document_id = "s << document_id << ", " << "relevance = "s << relevance << ", "s << "rating = "s << rating << " }"s << endl;
    }
}
