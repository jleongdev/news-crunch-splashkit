/**
 * ==== User Input and Scrolling ====
 * - Listens for mouse wheel scrolling that is locked between top
 * and bottom bounds of the card grid.
 * 
 * - Tracks for mouse clicks on article cards and buttons, also apply
 * coordinate guards to ensure clicks on AI summary button dont register on
 * cards behind. Or even clicks in the profile panel dont register on the cards behind.
 * 
 * - Handles text reading for the search bar, like return to search,
 * escape to clear.
 */
#include "splashkit.h"
#include "news_crunch_data.h"
#include "user_data.h"

void handle_search_input(news_app_data &data, login_state &login)
{
    rectangle search_box;

    search_box = rectangle_from(500, 22, 270, 35);
    
    if(mouse_clicked(LEFT_BUTTON) &&
        mouse_x() >= 500 && mouse_x() <= 770 && mouse_y() >= 22 && mouse_y() <= 57)
    {
        start_reading_text(search_box);
    }

    if(reading_text())
    {
        data.search_text = text_input();
    }

    if(key_typed(RETURN_KEY) && !data.is_loading)
    {
        end_reading_text();
        track_search_history(login.current_user, data.search_text);
        string api_query = data.search_text;

        size_t pos = 0;
        while ((pos = api_query.find(" ", pos)) != std::string::npos) 
        {
            api_query.replace(pos, 1, "%20");
            pos += 3;
        }

        search_articles_async(data, api_query);
    }

    if(key_typed(ESCAPE_KEY) && !data.is_loading)
    {
        data.search_text = "";
        end_reading_text();
        search_articles_async(data, "");
    }
}

void handle_article_click(news_app_data &data, login_state &login)
{
    if (!mouse_clicked(LEFT_BUTTON))
    {
        return;
    }

    // If profile panel is open, block clicks that land inside its area
    // so they don't fall through to article cards behind it
    if (login.show_profile)
    {
        double panel_x = 650;
        double panel_y = 80;
        double panel_w = 300;
        double panel_h = 430;

        if (mouse_x() >= panel_x && mouse_x() <= panel_x + panel_w &&
            mouse_y() >= panel_y && mouse_y() <= panel_y + panel_h)
        {
            return;  // click was inside profile panel — don't process article clicks
        }
    }

    // Same guard for the AI summary popup
    if (data.show_popup)
    {
        double popup_x = (screen_width()  - POPUP_WIDTH)  / 2.0;
        double popup_y = (screen_height() - POPUP_HEIGHT) / 2.0;

        if (mouse_x() >= popup_x && mouse_x() <= popup_x + POPUP_WIDTH &&
            mouse_y() >= popup_y && mouse_y() <= popup_y + POPUP_HEIGHT)
        {
            return;  // click was inside popup — don't process article clicks
        }
    }


    for (int i = 0; i < length(data.filtered_articles); i++)
    {
        int column = i % 3;
        int row = i / 3;

        double x = START_X + column * (CARD_WIDTH + GAP);
        double y = GRID_TOP + row * (CARD_HEIGHT + GAP) + data.scroll_y;

        if (mouse_y() < GRID_TOP)
        {
            continue;
        }

        if (mouse_x() >= x && mouse_x() <= x + CARD_WIDTH &&
            mouse_y() >= y && mouse_y() <= y + CARD_HEIGHT)
        {
            double btn_x = x + (CARD_WIDTH - BUTTON_WIDTH) / 2.0;
            double btn_y = y + 295;

            bool inside_summary_button = mouse_x() >= btn_x && mouse_x() <= btn_x + BUTTON_WIDTH &&
                mouse_y() >= btn_y && mouse_y() <= btn_y + BUTTON_HEIGHT;

            if (inside_summary_button)
            {
                open_article_summary_popup(data, data.filtered_articles[i]);
                track_summarised_article(login.current_user, data.filtered_articles[i].title);
            }
            else
            {
                track_source_click(login.current_user, data.filtered_articles[i].source.name);
                increment_articles_read(login.current_user);
                open_article_url(data.filtered_articles[i]);
            }

            return;
        }
    }
}

void handle_scrolling(news_app_data &data, login_state &login)
{
    if (data.is_loading || login.show_profile)
    {
        return;
    }

    data.scroll_y += mouse_wheel_scroll().y * SCROLL_SPEED;

    if (data.scroll_y > 0)
    {
        data.scroll_y = 0;
    }

    int rows = (length(data.filtered_articles) + 2) / 3;
    double content_height = rows * (CARD_HEIGHT + GAP);
    double visible_height = screen_height() - GRID_TOP;
    double min_scroll = -(content_height - visible_height);

    if (min_scroll > 0)
    {
        min_scroll = 0;
    }

    if (data.scroll_y < min_scroll)
    {
        data.scroll_y = min_scroll;
    }
}