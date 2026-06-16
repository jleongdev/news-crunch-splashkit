/**
 * ==== Article Parsing and Image Handling ====
 * Parses raw JSON responses into C++ structures
 * Pre-process JSON strings to prevent crashes on null fields
 * Creates multiple parallel threads to download images concurrently,
 * then joining them together once finished.
 */
#include "news_crunch_data.h"
#include <cstdlib>
#include <cstdio>
#include <thread>

article_data read_article_from_json(json article_json)
{
    // Convert the incoming article JSON to a raw string
    std::string raw_json_str = json_to_string(article_json);
    
    // Silence all occurrences of ":null" to point to empty quotes instead
    size_t pos = 0;
    while ((pos = raw_json_str.find(":null", pos)) != std::string::npos) 
    {
        raw_json_str.replace(pos, 5, ":\"\"");
        pos += 3;
    }
    
    // Catch occurrences where there is a space: ": null"
    pos = 0;
    while ((pos = raw_json_str.find(": null", pos)) != std::string::npos) 
    {
        raw_json_str.replace(pos, 6, ": \"\"");
        pos += 4; // Step past the new : ""
    }

    // 3. Parse string back into a temporary JSON object
    json clean_article_json = json_from_string(raw_json_str);
    article_data article;
    json source_json;

    source_json = json_read_object(clean_article_json, "source");
    article.source.id = json_has_key(source_json, "id") ? json_read_string(source_json, "id") : "";
    article.source.name = json_has_key(source_json, "name") ? json_read_string(source_json, "name") : "";

    article.author = json_has_key(clean_article_json, "author") ? json_read_string(clean_article_json, "author") : "";
    article.title = json_has_key(clean_article_json, "title") ? json_read_string(clean_article_json, "title") : "";
    article.description = json_has_key(clean_article_json, "description") ? json_read_string(clean_article_json, "description") : "";
    article.url = json_has_key(clean_article_json, "url") ? json_read_string(clean_article_json, "url") : "";
    article.url_to_image = json_has_key(clean_article_json, "urlToImage") ? json_read_string(clean_article_json, "urlToImage")  : "";
    article.published_at = json_has_key(clean_article_json, "publishedAt") ? json_read_string(clean_article_json, "publishedAt") : "";
    article.content = json_has_key(clean_article_json, "content") ? json_read_string(clean_article_json, "content") : "";
    article.has_image = false;

    // 5. Free up the temporary clean JSON object memory
    free_json(clean_article_json);

    return article;
}

void reset_filtered_articles(news_app_data &data)
{
    data.filtered_articles = dynamic_array<article_data>();

    for (int i = 0; i < length(data.articles); i++)
    {
        add(data.filtered_articles, data.articles[i]);
    }
}

bool article_matches_search(const article_data &article, string search_text)
{
    if (search_text == "")
    {
        return true;
    }

    return contains(article.title, search_text) ||
           contains(article.description, search_text) ||
           contains(article.source.name, search_text) ||
           contains(article.author, search_text);
}

void filter_articles(news_app_data &data)
{
    data.filtered_articles = dynamic_array<article_data>();

    for (int i = 0; i < length(data.articles); i++)
    {
        if (article_matches_search(data.articles[i], data.search_text))
        {
            add(data.filtered_articles, data.articles[i]);
        }
    }
}

void open_article_url(const article_data &article)
{
    string command;

    if (article.url == "")
    {
        return;
    }

    #ifdef _WIN32
        command = "start \"\" \"" + article.url + "\"";
    #elif __APPLE__
        command = "open \"" + article.url + "\"";
    #else
        command = "xdg-open \"" + article.url + "\"";
    #endif

    system(command.c_str());
}

// need methods to load images in parallel using multiple threads
// * in the function parameter receives that address
void load_single_image(article_data *article, int index, string query)
{
    string image_name = "article_image_" + query + "_" + to_string(index);
    article->image = download_bitmap(image_name, article->url_to_image, 443);

    if (bitmap_width(article->image) > 0 && bitmap_height(article->image) > 0)
    {
        article->has_image = true;
    }
    else
    {
        article->has_image = false;
    }
}

void load_article_images(news_app_data &data, string query)
{
    vector<std::thread> threads;

    for (int i = 0; i < length(data.articles); i++)
    {
        data.articles[i].has_image = false;

        if (data.articles[i].url_to_image != "")
        {
            // creates and immediately starts a new thread
            threads.push_back(std::thread(load_single_image, &data.articles[i], i, query));
        }
    }

    // Wait here until ALL threads have been finsihed before continuing
    for (auto &t : threads)
    {
        // join means wait for this thread to finish
        t.join();
    }

    reset_filtered_articles(data);
}