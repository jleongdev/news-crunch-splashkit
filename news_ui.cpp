/**
 * ==== Renderer ====
 * - Combines custom SplashKit shape drawing like rectangeles, text, images
 * with standard SplashKit GUI widgets like panels, text boxes, inset scrollbars
 * - Draws the login screens, profile stats, article cards, loading screen, and
 * summary popups.
 */
#include "splashkit.h"
#include "news_crunch_data.h"
#include "user_data.h"
#include <string>

void draw_loading_spinner(double center_x, double center_y, double radius)
{
    int segments = 12;
    int active = (int)(current_ticks() / 80) % segments;
    double angle_step = 2.0 * M_PI / segments;

    for (int i = 0; i < segments; i++)
    {
        double angle = i * angle_step - M_PI / 2.0;
        double x = center_x + cos(angle) * radius;
        double y = center_y + sin(angle) * radius;
        color dot_color = (i == active) ? COLOR_PURPLE : COLOR_LIGHT_GRAY;

        fill_circle(dot_color, x, y, (i == active) ? 10 : 7);
    }
}

void draw_login_screen(login_state &state)
{
    set_interface_style(SHADED_LIGHT_STYLE, hsb_color(0.67, 0.4, 1.0)); // purple tint
    set_interface_font(load_font("ui_font", "arial.ttf"));
    set_interface_font_size(13);

    clear_screen(COLOR_WHITE);

    draw_text("News Crunch", COLOR_PURPLE, "ui_font", 28, 380, 120);
    draw_text("AI-powered personalised news dashboard",
        COLOR_DARK_GRAY, "ui_font", 14, 310, 160);

    string panel_name = state.show_register
        ? "Create Account##" + to_string(state.login_panel_id)
        : "Welcome Back##" + to_string(state.login_panel_id);

    if (start_panel(panel_name, rectangle_from(350, 200, 300, 260)))
    {
        set_interface_label_width(80);

        // Username field
        state.username_input = text_box("Username", state.username_input);

        // Password field — SplashKit has no password masking so we label it clearly
        state.password_input = text_box("Password", state.password_input);

        // Error message
        if (state.error_message != "")
        {
            set_interface_text_color(rgba_color(200, 0, 0, 255));
            paragraph(state.error_message);
            set_interface_text_color(COLOR_BLACK);
        }

        // Login / Register button
        string action_label = state.show_register ? "Create Account" : "Login";
        if (button(action_label))
        {
            if (state.username_input == "" || state.password_input == "")
            {
                state.error_message = "Please fill in all fields.";
            }
            else if (state.show_register)
            {
                if (username_exists(state.username_input))
                {
                    state.error_message = "Username already taken.";
                }
                else
                {
                    register_user(state.username_input, state.password_input);
                    load_user_from_json(state.username_input,
                        state.password_input, state.current_user);
                    state.is_logged_in = true;
                    state.show_profile = false;
                    state.error_message = "";
                }
            }
            else
            {
                if (load_user_from_json(state.username_input,
                    state.password_input, state.current_user))
                {
                    state.is_logged_in = true;
                    state.show_profile = false;
                    state.error_message = "";
                }
                else
                {
                    state.error_message = "Invalid username or password.";
                }
            }
        }

        // Toggle between login and register
        string toggle_label = state.show_register
            ? "Already have an account? Login"
            : "No account? Register";

        if (button(toggle_label))
        {
            state.show_register = !state.show_register;
            state.error_message = "";
            state.login_panel_id++;   // force new panel so it reopens fresh
        }

        end_panel(panel_name);
    }

    draw_interface();
}

void draw_profile_panel(login_state &login)
{
    if (!login.show_profile)
    {
        return;
    }

    string panel_name = "My Profile##profile_" + to_string(login.profile_panel_id);

    if (start_panel(panel_name, rectangle_from(650, 80, 300, 350)))
    {
        login.profile_panel_open = true;
        paragraph("User: " + login.current_user.username);
        paragraph("Number of articles read: " + to_string(login.current_user.articles_read_count));
        paragraph("Last active hour: " + to_string(login.current_user.last_active_hour));

        // Favourite sources
        if (header("Favourite Sources"))
        {
            if (login.current_user.favourite_sources.empty())
            {
                paragraph("No sources tracked yet. Open some articles to track!");
            }
            else
            {
                start_inset("sources_list", 80);
                for (const string &s : login.current_user.favourite_sources)
                {
                    paragraph(s);
                }
                
                end_inset("sources_list");
            }
        }

        if (header("Search History"))
        {
            if (login.current_user.search_history.empty())
            {
                paragraph("No search history, search for articles to track!");
            }
            else
            {
                start_inset("search_history_list", 80);
                for (const string &s : login.current_user.search_history)
                {
                    paragraph(s);
                }
                
                end_inset("search_history_list");
            }
        }

        if (header("Summarised Articles Collection"))
        {
            if (login.current_user.summarised_articles.empty())
            {
                paragraph("No summarised articles yet, click AI Summary to track!");
            }
            else
            {
                start_inset("summarised_articles_list", 80);
                for (const string &s : login.current_user.summarised_articles)
                {
                    paragraph(s);
                }
                
                end_inset("summarised_articles_list");
            }
        }

        if (button("Logout"))
        {
            login.is_logged_in = false;
            login.show_profile = false;
            login.profile_panel_open = false;
            login.username_input = "";
            login.password_input = "";
            login.current_user = user_data();
            login.login_panel_id++;
            login.profile_panel_id++;
        }

        end_panel(panel_name);
    }
    else if (login.profile_panel_open)
    {
        login.show_profile = false;
        login.profile_panel_open = false;
        login.profile_panel_id++;
    }
}

void draw_search_bar(const news_app_data &data, font ui_font)
{
    string display_text;

    fill_rectangle(COLOR_WHITE, 500, 22, 270, 35);
    draw_rectangle(COLOR_GRAY, 500, 22, 270, 35);

    if(data.search_text == "")
    {
        if(reading_text())
        {
            draw_text("|", COLOR_BLACK, ui_font, 12, 510, 32);
        }
        else
        {
            draw_text("Search titles or sources...", COLOR_DARK_GRAY, ui_font, 12, 510, 32);
        }
    }
    else
    {
        display_text = short_text(data.search_text, 27);

        if(reading_text())
        {
            display_text += "|";
        }

        draw_text(display_text, COLOR_BLACK, ui_font, 12, 510, 32);
    }
}

void draw_summary_popup(news_app_data &data)
{
    if (!data.show_popup)
    {
        return;
    }

    double popup_x = (screen_width() - POPUP_WIDTH) / 2.0;
    double popup_y = (screen_height() - POPUP_HEIGHT) / 2.0;
    string panel_name = "AI Article Summary#" + to_string(data.popup_id);

    if (start_panel(panel_name, rectangle_from(popup_x, popup_y, POPUP_WIDTH, POPUP_HEIGHT)))
    {
        // Title
        paragraph(short_text(data.popup_article_title, 80));

        if (data.popup_is_loading)
        {
            paragraph("AI is reading the article...");
        }
        else
        {
            // Use an inset with scrollbar for long summaries
            start_inset("summary_content", 280);
            paragraph(data.popup_summary);
            end_inset("summary_content");
        }

        end_panel(panel_name);
    }
    else
    {
        data.show_popup = false;
        data.popup_summary = "";
    }
}

void draw_article_card(const article_data &article, font ui_font, double x, double y, double width, double height)
{
    fill_rectangle(COLOR_WHITE, x, y, width, height);
    draw_rectangle(COLOR_GRAY, x, y, width, height);

    fill_rectangle(COLOR_LIGHT_GRAY, x, y, width, 32);

    draw_text(short_text(article.source.name, 24), COLOR_BLACK, ui_font, 15, x + 10, y + 9);

    if (article.has_image)
    {
        double img_w = bitmap_width(article.image);
        double img_h = bitmap_height(article.image);
        double target_w = width - 20;
        double target_h = 105.0;
        double scale = std::min(target_w / img_w, target_h / img_h);
        double scaled_w = img_w * scale;
        double scaled_h = img_h * scale;

        // Top-left of the scaled image inside the grey box
        double box_x = x + 10 + (target_w - scaled_w) / 2.0;
        double box_y = y + 32 + (target_h - scaled_h) / 2.0;

        // SplashKit only treats (x, y) as top-left when scale is 1; otherwise it
        // shifts by (bitmap_size / 2) * (1 - scale)
        double draw_x = box_x - (img_w / 2.0) * (1.0 - scale);
        double draw_y = box_y - (img_h / 2.0) * (1.0 - scale);

        draw_bitmap(article.image, draw_x, draw_y, option_scale_bmp(scale, scale));
    }
    else
    {
        fill_rectangle(COLOR_LIGHT_GRAY, x + 10, y + 30, width - 20, 105);       
        draw_text("No image available", COLOR_DARK_GRAY, ui_font, 14, x + 70, y + 77);
    }

    draw_wrapped_text(article.title, COLOR_BLACK, ui_font, 15, x + 10, y + 145, 35, 2);
    draw_text("By " + short_text(article.author, 28), COLOR_DARK_GRAY, ui_font, 13, x + 10, y + 195);
    draw_wrapped_text(short_text(article.description, 80), COLOR_GRAY, ui_font, 13, x + 10, y + 225, 40, 2);
    draw_text(format_published_date(article.published_at), COLOR_DARK_GRAY, ui_font, 13, x + 10, y + 270);

    double btn_x = x + (width - BUTTON_WIDTH) / 2.0;
    double btn_y = y + 295;

    fill_rectangle(COLOR_LIGHT_BLUE, btn_x, btn_y, BUTTON_WIDTH, BUTTON_HEIGHT);
    draw_rectangle(COLOR_BLUE, btn_x, btn_y, BUTTON_WIDTH, BUTTON_HEIGHT);
    draw_text("AI Summarise", COLOR_NAVY, ui_font, 12, btn_x + 12, btn_y + 6);
}

void draw_news_grid(news_app_data &data)
{
    for (int i = 0; i < length(data.filtered_articles); i++)
    {
        int column = i % 3;
        int row = i / 3;

        double x = START_X + column * (CARD_WIDTH + GAP);
        double y = GRID_TOP + row * (CARD_HEIGHT + GAP) + data.scroll_y;

        if (y + CARD_HEIGHT < GRID_TOP)
        {
            continue;
        }

        if (y > screen_height())
        {
            continue;
        }

        draw_article_card(data.filtered_articles[i], data.ui_font, x, y, CARD_WIDTH, CARD_HEIGHT);
    }
}

// Draw a fixed area where the Title, search bar, and AI summary lies
// This is to ensure that the article grid does not scroll on top of it
void draw_fixed_header(const news_app_data &data, login_state &login, font ui_font)
{
    fill_rectangle(COLOR_WHITE, 0, 0, screen_width(), GRID_TOP);

    draw_text("News Crunch", COLOR_PURPLE, ui_font, 17, 40, 30);
    draw_text("AI-powered personalised news dashboard", COLOR_DARK_GRAY, ui_font, 14, 40, 60);
    draw_search_bar(data, ui_font);

    double profile_btn_x = 790;
    double profile_btn_y = 22;
    double profile_btn_w = 160;
    double profile_btn_h = 35;

    bool hovered = mouse_x() >= profile_btn_x && mouse_x() <= profile_btn_x + profile_btn_w &&
                   mouse_y() >= profile_btn_y && mouse_y() <= profile_btn_y + profile_btn_h;

    fill_rectangle(hovered ? rgb_color(220, 210, 255) : rgb_color(235, 225, 255),
                   profile_btn_x, profile_btn_y, profile_btn_w, profile_btn_h);
    draw_rectangle(COLOR_PURPLE, profile_btn_x, profile_btn_y, profile_btn_w, profile_btn_h);
    draw_text("My Profile", COLOR_NAVY, ui_font, 13, profile_btn_x + 35, profile_btn_y + 10);

    if (mouse_clicked(LEFT_BUTTON) && hovered)
    {
        if (login.show_profile)
        {
            login.show_profile = false;
            login.profile_panel_id++;
        }
        else
        {
            login.show_profile = true;
        }
    }

    // AI summary banner
    fill_rectangle(COLOR_LIGHT_BLUE, 40, 95, 920, 120);
    draw_rectangle(COLOR_BLUE, 40, 95, 920, 120);
    draw_text("AI Summary:", COLOR_NAVY, ui_font, 17, 60, 112);
    if(data.is_loading)
    {
        int dots = (int)(current_ticks() / 500) % 4;
        string loading_text = "Fetching articles" + string(dots, '.');
        draw_text(loading_text, COLOR_DARK_GRAY, ui_font, 13, 60, 140);
    }
    else
    {
        string summary = data.ai_summary == "" ? "Loading summary..." : data.ai_summary;
        draw_wrapped_text(summary, COLOR_NAVY, ui_font, 14, 60, 133, 110, 4);
    }
}

void draw_loading_screen(const news_app_data &data)
{
    double center_x = screen_width() / 2.0;
    double center_y = screen_height() / 2.0 + 20;

    fill_rectangle(COLOR_WHITE, 0, GRID_TOP, screen_width(), screen_height() - GRID_TOP);

    draw_loading_spinner(center_x, center_y - 30, 42);
    draw_text("Loading articles...", COLOR_BLACK, data.ui_font, 16, center_x - 95, center_y + 30);
    draw_text("Downloading images from the web", COLOR_DARK_GRAY, data.ui_font, 13, center_x - 125, center_y + 58);
}

// Draw the whole dashboard
void draw_news_app(news_app_data &data, login_state &login)
{
    rectangle grid_area;

    clear_screen(COLOR_WHITE);

    if (data.is_loading)
    {
        draw_loading_screen(data);
    }
    else
    {
        if (data.filtered_articles.length() == 0)
        {
            draw_text("No articles found.", COLOR_DARK_GRAY, data.ui_font, 24, 300, 400); 
            draw_text("Try a different search query.", COLOR_GRAY, data.ui_font, 16, 300, 440);
        }
        else
        {
            grid_area = rectangle_from(0, GRID_TOP, screen_width(), screen_height() - GRID_TOP);
            push_clip(grid_area);
            draw_news_grid(data);
            pop_clip();
        }
    }

    draw_fixed_header(data, login, data.ui_font);
    if (data.show_popup)
    {
        draw_summary_popup(data);
    }
    draw_profile_panel(login);
    draw_interface();
}