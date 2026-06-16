/**
 * ==== AI Integration ====
 * Uses Splashkit Generative AI tools by using the LLM engine QWEN_1_7B_INSTRUCT
 * to generate two types of summaries:
 * 1. A general one-sentence summary of the top 5 articles loaded.
 * 2. A detailed 3 to 4 sentence summary of a single article displayed ina popup panel
 */
#include "splashkit.h"
#include "news_crunch_data.h"
#include <thread>

void generate_ai_summary(news_app_data &data)
{
    if (length(data.articles) == 0)
    {
        return;
    }

    // Create a prompt from the top 5 article descriptions
    string prompt = "Summarise these news descriptions in one sentence:\n";
    int count = std::min(5, (int)length(data.articles));

    for (int i = 0; i < count; i++)
    {
        prompt += "- " + data.articles[i].description + "\n";
    }

    // QWEN3_1_7B_INSTRUCT model usage for a good balance of speed and quality
    data.ai_summary = generate_reply(QWEN3_1_7B_INSTRUCT, prompt);
}

static void generate_popup_summary_in_background(news_app_data *data, article_data article)
{
    string prompt = "Summarise this news article in 3 to 4 clear sentences:\n";
    prompt += "Title: " + article.title + "\n";

    if (article.description != "")
    {
        prompt += "Description: " + article.description + "\n";
    }

    if (article.content != "")
    {
        prompt += "Content: " + article.content + "\n";
    }

    data->popup_summary = generate_reply(QWEN3_1_7B_INSTRUCT, prompt);
    data->popup_is_loading = false;
}

void open_article_summary_popup(news_app_data &data, const article_data &article)
{
    data.popup_id++;
    data.show_popup = true;
    data.popup_article_title = article.title;
    data.popup_is_loading = true;
    data.popup_summary = "";

    std::thread t(generate_popup_summary_in_background, &data, article);
    t.detach();
}