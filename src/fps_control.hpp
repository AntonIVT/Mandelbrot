#pragma once

#include <SFML/Graphics.hpp>

//===================================================================================

void set_text_preferences(sf::Text &text)
{
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Green);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1);
}
//===================================================================================

struct fps_control
{
    sf::Text text_fps;
    bool is_fps_show;
    bool is_fps_pressed;
    sf::Time when_fps_pressed;
    char fps_line[10];
};
//===================================================================================

void get_fps_control(fps_control *fps)
{
    set_text_preferences(fps->text_fps);
    fps->is_fps_show = false;
    fps->is_fps_pressed = false;
}
//===================================================================================
