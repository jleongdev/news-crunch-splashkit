/**
 * Provides string helper functions like shortening long titles,
 * custom word-wrapping, and ISO date formatting
 */
#include "splashkit.h"

string short_text(string text, int max_length)
{
    if (text.length() <= max_length)
    {
        return text;
    }

    return text.substr(0, max_length) + "...";
}

// iso date is like MM/DD/YY and HH/MM
string format_published_date(string iso_date)
{
    // Slice out each part of the given date using substr
    string year = iso_date.substr(0, 4);
    string month = iso_date.substr(5, 2);
    string day = iso_date.substr(8, 2);
    string hour = iso_date.substr(11, 2);
    string min = iso_date.substr(14, 2);

    string month_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    // convert say "05" to 5 then subtract 1 to get array index
    int month_index = stoi(month) - 1;

    if (month_index < 0 || month_index > 11)
    {
        return "Published at: " + iso_date;
    }
    
    string month_name = month_names[month_index];

    if (day[0] == '0')
    {
        day = day.substr(1);
    }

    return "Published at: " + month_name + " " + day + ", " + year + " · " + hour + ":" + min;
}

void draw_wrapped_text(string text, color text_color, font ui_font, double font_size, double x, double y, int max_chars, int max_lines)
{
    string line;
    int line_count;

    line = "";
    line_count = 0;

    for (int i = 0; i < text.length(); i++)
    {
        line += text[i];

        if (line.length() >= max_chars || i == text.length() - 1)
        {
            if (line_count < max_lines)
            {
                draw_text(line, text_color, ui_font, font_size, x, y + line_count * 22);
                line_count++;
            }
            
            line = "";
        }
    }
}