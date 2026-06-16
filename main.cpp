/**
 * ==== Main entry point and app loop ====
 * - It orchestrates the main game loop
 * - Manages the state transitions (From logged out to logged in to
 *   news dashboard)
 * - Ensures that actions are checked and drawn every frame at 60 FPS
 */
#include "splashkit.h"
#include "splashkit-arrays.h"
#include "news_crunch_data.h"
#include "user_data.h"

int main()
{   
    open_window("News Crunch", 1000, 800);
    font ui_font = load_font("ui_font", "arial.ttf");

    login_state login;
    login.is_logged_in = false;
    login.show_register = false;
    login.show_profile = false;
    login.login_panel_id = 0;
    login.profile_panel_id = 0;
    login.profile_panel_open = false;
    login.error_message = "";
    login.username_input = "";
    login.password_input = "";

    news_app_data data;
    data.scroll_y = 0;
    data.is_loading = false;
    data.search_text = "";
    data.last_query = "";
    data.ai_summary = "";
    data.show_popup = false;
    data.popup_id = 0;
    data.ui_font = ui_font;

    bool articles_started = false;

    while (!quit_requested())
    {
        process_events();

        if (!login.is_logged_in)
        {
            // show login screen until logged in
            draw_login_screen(login);
            refresh_screen(60);
            continue;
        }

        // first time after login - start loading articles
        if (!articles_started)
        {
            load_initial_articles_async(data);
            articles_started = true;
        }

        handle_search_input(data, login);
        handle_scrolling(data, login);
        filter_articles(data);
        handle_article_click(data, login);

        draw_news_app(data, login);
        refresh_screen(60);
    }

    return 0;
}