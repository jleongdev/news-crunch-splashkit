#ifndef NEWS_CRUNCH_DATA_H
#define NEWS_CRUNCH_DATA_H

#include "splashkit.h"
#include "splashkit-arrays.h"
#include "user_data.h"

// Constants for the dimensions of an article card
const double START_X = 40;
const double START_Y = 180;
const double CARD_WIDTH = 290;
const double CARD_HEIGHT = 330;
const double GAP = 25;

const double SCROLL_SPEED = 30;
const double GRID_TOP = 230; // top of scrollable area (below header)

const double BUTTON_WIDTH = 110;
const double BUTTON_HEIGHT = 25;

const double POPUP_WIDTH = 600;
const double POPUP_HEIGHT = 400;

const string NEWS_API_KEY = "===PASTE YOUR NEWSAPI API KEY HERE===";
const string NEWS_API_URL = "https://newsapi.org/v2/top-headlines?language=en&pageSize=20&apiKey=" + NEWS_API_KEY;

struct source_data
{
    string id;
    string name;
};

/**
 * This struct represents a single news article. It holds data
 * strings like author, title, description, url, url_to_image, published_at,
 * content, a nested source_data struct with the API source id and name,
 * and variables like bool has_image and bitmap image
 */
struct article_data
{
    source_data source;
    string author;
    string title;
    string description;
    string url;
    string url_to_image;
    bool has_image;
    bitmap image;
    string published_at;
    string content;
};

/**
 * This struct represents the main state container for the dashboard.
 * It holds the articles array (original API response) and filtered_articles
 * (currently searched subset). It tracks for UI flags like is_loading, show_popup,
 * scroll_y, font configs, search text, and AI responses
 */
struct news_app_data
{
    dynamic_array<article_data> articles;
    dynamic_array<article_data> filtered_articles;
    string search_text;
    string last_query;
    string ai_summary;
    double scroll_y;
    font ui_font;
    bool is_loading;
    bool images_loading;
    bool show_popup;
    bool popup_is_loading;
    int popup_id;
    string popup_summary;
    string popup_article_title;
};

// Backend functionalities

article_data read_article_from_json(json article_json);

bool article_matches_search(const article_data &article, string search_text);

void filter_articles(news_app_data &data);

void handle_search_input(news_app_data &data, login_state &login);

void open_article_url(const article_data &article);

void handle_article_click(news_app_data &data, login_state &login);

void load_single_image(article_data *article, int index, string query);

void load_article_images(news_app_data &data, string query);

void handle_scrolling(news_app_data &data, login_state &login);

void search_articles_async(news_app_data &data, string query);

void load_initial_articles_async(news_app_data &data);

void generate_ai_summary(news_app_data &data);

void open_article_summary_popup(news_app_data &data, const article_data &article);

// Frontend functionalities

string short_text(string text, int max_length);

void draw_wrapped_text(string text, color text_color, font ui_font, double font_size, double x, double y, int max_chars, int max_lines);

string format_published_date(string iso_date);

// static vector<string> split_csv(const string &s);

// static string join_csv(const vector<string> &v);

void draw_login_screen(login_state &state);

void draw_profile_panel(login_state &state);

void draw_search_bar(const news_app_data &data, font ui_font);

void draw_fixed_header(const news_app_data &data, login_state &login, font ui_font);

void draw_summary_popup(news_app_data &data);

void draw_article_card(const article_data &article, font ui_font, double x, double y, double width, double height);

void draw_news_grid(news_app_data &data);

void draw_loading_screen(const news_app_data &data);

void draw_news_app(news_app_data &data, login_state &login);

#endif
