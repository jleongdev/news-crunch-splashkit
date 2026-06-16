#ifndef USER_DATA_H
#define USER_DATA_H

#include "splashkit.h"
#include <vector>

/**
 * Represents user statistics. Keeps a list of favourite
 * news sources, search history, and summarised articles,
 * along with an article read counter and last active hour.
 */
struct user_data
{
    string username;
    string password;
    vector<string> favourite_sources;
    vector<string> search_history;
    vector<string> top_keywords;
    vector<string> summarised_articles;
    int articles_read_count;
    int last_active_hour;
};

/**
 * It manages the state of the active user session. Holds
 * the current_user of type user_data, UI text inputs for
 * password/username, and control variables for the user 
 * profile layout.
 */
struct login_state
{
    bool is_logged_in;
    string username_input;
    string password_input;
    string error_message;
    bool show_register;
    bool show_profile;
    bool profile_panel_open;
    int login_panel_id;
    int profile_panel_id;
    user_data current_user;
};

// JSON encoding and decoding of user profile
void save_user_to_json(const user_data &user);

bool load_user_from_json(const string &username, const string &password, user_data &out_user);

bool username_exists(const string &username);

void register_user(const string &username, const string &password);

// Preference tracking
void track_source_click(user_data &user, const string &source);

void track_search_history(user_data &user, const string &query);

void track_summarised_article(user_data &user, const string &title);

void increment_articles_read(user_data &user);

#endif