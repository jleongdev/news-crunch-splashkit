/**
 * ===== API Network Operations =====
 * Connects to newsapi.org using HTTP requests.
 * Implements asynchrounous network fetching using thread C++ library
 * by loading articles on a background thread to prevent any UI freezing
 */
#include "splashkit.h"
#include "news_crunch_data.h"
#include <thread>

string build_news_api_query_url(string query)
{
    if (query == "")
    {
        return NEWS_API_URL;
    }

    return "https://newsapi.org/v2/everything?language=en&pageSize=20&q="
        + query + "&apiKey=" + NEWS_API_KEY;
}

bool fetch_articles_from_url(news_app_data &data, string url)
{
    http_response response;
    string response_text;
    json news_json;
    string status;
    vector<json> article_json_values;

    response = http_get(url, 443);
    response_text = http_response_to_string(response);
    free_response(response);

    news_json = json_from_string(response_text);
    status = json_read_string(news_json, "status");

    if (status != "ok")
    {
        write_line("NewsAPI error: " + json_read_string(news_json, "message"));
        return false;
    }

    data.articles = dynamic_array<article_data>();
    json_read_array(news_json, "articles", article_json_values);

    for (int i = 0; i < article_json_values.size(); i++)
    {
        add(data.articles, read_article_from_json(article_json_values[i]));
    }

    return true;
}

void load_articles_in_background(news_app_data *data, string query)
{
    string url;

    url = build_news_api_query_url(query);

    if (!fetch_articles_from_url(*data, url))
    {
        data->is_loading = false;
        return;
    }

    
    filter_articles(*data);
    generate_ai_summary(*data);
    data->scroll_y = 0;
    data->search_text = "";
    data->is_loading = false;
    
    load_article_images(*data, query);
    
    write_line("Loaded " + to_string(length(data->articles)) + " articles for query: " + query);
}

void search_articles_async(news_app_data &data, string query)
{
    if (data.is_loading)
    {
        return;
    }

    data.is_loading = true;
    data.last_query = query;

    // main thread continues immediately — drawing, handling input, etc
    // background thread quietly finishes and sets is_loading = false
    std::thread t(load_articles_in_background, &data, query);
    t.detach();
}

void load_initial_articles_async(news_app_data &data)
{
    data.is_loading = true;
    data.last_query = "";

    // string("") is used instead of just "" because std::thread copies its
    // arguments — passing "" as a raw string literal can confuse the compiler
    // into treating it as const char* instead of std::string
    std::thread t(load_articles_in_background, &data, string(""));
    t.detach();
}