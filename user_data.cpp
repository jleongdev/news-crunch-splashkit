/**
 * ==== User Management ====
 * Handles registering and logging in users
 * Analyze user statistics and historical data
 * - Search queries
 * - Favourite news sources
 * - Summarized article list
 * - Read Count
 * It stors all these info to a local JSON file
 */
#include "user_data.h"
#include "news_crunch_data.h"
#include <vector>
#include <ctime>

const string USERS_JSON = "users.json";

void save_user_to_json(const user_data &user)
{
    json all_users;
    vector<json> users_array;

    all_users = json_from_file(USERS_JSON);
    json_read_array(all_users, "users", users_array);

    json user_json = create_json();
    json_set_string(user_json, "username", user.username);
    json_set_string(user_json, "password", user.password);
    json_set_array(user_json, "favourite_sources", user.favourite_sources);
    json_set_array(user_json, "search_history", user.search_history);
    json_set_array(user_json, "summarised_articles", user.summarised_articles);
    json_set_number(user_json, "articles_read_count", user.articles_read_count);
    json_set_number(user_json, "last_active_hour", user.last_active_hour);

    // replace existing user entry or add a new one
    bool found = false;
    for (int i = 0; i < users_array.size(); i++)
    {
        if (json_read_string(users_array[i], "username") == user.username)
        {
            users_array[i] = user_json;
            found = true;
            break;
        }
    }

    if (!found)
    {
        users_array.push_back(user_json);
    }

    // write it back to user json file
    json output = create_json();
    json_set_array(output, "users", users_array);
    json_to_file(output, USERS_JSON);

    free_json(all_users); 
    free_json(output);
}

bool load_user_from_json(const string &username, const string &password, user_data &out_user)
{
    json all_users = json_from_file(USERS_JSON);
    vector<json> users_array;
    json_read_array(all_users, "users", users_array);

    for (int i = 0; i < users_array.size(); i++)
    {
        string u = json_read_string(users_array[i], "username");
        string p = json_read_string(users_array[i], "password");

        if (u == username && p == password)
        {
            out_user.username = u;
            out_user.password = p;

            out_user.favourite_sources.clear();
            out_user.search_history.clear();

            if (json_has_key(users_array[i], "favourite_sources"))
            {
                json_read_array(users_array[i], "favourite_sources", out_user.favourite_sources);
            }

            if (json_has_key(users_array[i], "search_history"))
            {
                json_read_array(users_array[i], "search_history", out_user.search_history);
            }

            if (json_has_key(users_array[i], "summarised_articles"))
            {
                json_read_array(users_array[i], "summarised_articles", out_user.summarised_articles);
            }

            out_user.articles_read_count = json_has_key(users_array[i], "articles_read_count")
                ? (int)json_read_number(users_array[i], "articles_read_count") : 0;

            out_user.last_active_hour = json_has_key(users_array[i], "last_active_hour")
                ? (int)json_read_number(users_array[i], "last_active_hour") : 0;

            return true;
        }
    }

    return false;
}

bool username_exists(const string &username)
{
    json all_users = json_from_file(USERS_JSON);
    vector<json> users_array;
    json_read_array(all_users, "users", users_array);

    for (int i = 0; i < users_array.size(); i++)
    {
        if (json_read_string(users_array[i], "username") == username)
        {
            return true;
        }
    }

    return false;
}

void register_user(const string &username, const string &password)
{
    user_data new_user;
    new_user.username = username;
    new_user.password = password;
    new_user.articles_read_count = 0;
    new_user.last_active_hour = 0;
    save_user_to_json(new_user);
}

void track_source_click(user_data &user, const string &source)
{
    if (source.empty())
    {
        return;
    }

    // only add source if not already tracked
    for (const string &s : user.favourite_sources)
    {
        // already tracked so just return
        if (s == source)
        {
            return;
        }
    }

    user.favourite_sources.push_back(source);
    save_user_to_json(user);
}

void track_search_history(user_data &user, const string &query)
{
    if (query.empty())
    {
        return;
    }

    // set a cap at 20 most recent, remove the oldest if full
    if ((int)user.search_history.size() >= 20)
    {
        user.search_history.erase(user.search_history.begin());
    }

    user.search_history.push_back(query);
    save_user_to_json(user);
}

void track_summarised_article(user_data &user, const string &title)
{
    if (title.empty())
    {
        return;
    }

    // cap at 10 most recent articles
    if ((int)user.summarised_articles.size() >= 10)
    {
        user.summarised_articles.erase(user.summarised_articles.begin());
    }

    user.summarised_articles.push_back(short_text(title, 60));
    save_user_to_json(user);
}

void increment_articles_read(user_data &user)
{
    user.articles_read_count++;

    time_t now = time(nullptr);
    tm *local = localtime(&now);
    user.last_active_hour = local->tm_hour;

    save_user_to_json(user);
}