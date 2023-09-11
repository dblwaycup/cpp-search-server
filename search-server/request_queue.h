#pragma once
#include "search_server.h"
#include <vector>
#include <deque>


class RequestQueue {
public:
    RequestQueue(const SearchServer& search_server);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;
    void AddRequest(int result_size);

private:

    struct QueryResult {
        int no_result;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& server_;
    unsigned long long current_time;
    int no_result_count;

    public:

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate)
    {
        std::vector<Document> result = server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(static_cast<int>(result.size()));
        return result;
    }
};
