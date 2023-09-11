#include "pch.h"
#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) : requests_({}), server_(search_server), current_time(0), no_result_count(0)
{
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status)
{
    std::vector<Document> result = server_.FindTopDocuments(raw_query, status);
    auto size = result.size();
    AddRequest(static_cast<int>(size));
    return result;
}
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query)
{
    std::vector<Document> result = server_.FindTopDocuments(raw_query);
    auto size = result.size();
    AddRequest(static_cast<int>(size));
    return result;
}
int RequestQueue::GetNoResultRequests() const {
    return no_result_count;
}
void RequestQueue::AddRequest(int result_size)
{
    ++current_time;
    if (!requests_.empty() && current_time > min_in_day_)
    {
        if (requests_.front().no_result == 0)
        {
            --no_result_count;

        }
        requests_.pop_front();
    }
    requests_.push_back({ result_size });
    if (result_size == 0)
    {
        ++no_result_count;
    }
}