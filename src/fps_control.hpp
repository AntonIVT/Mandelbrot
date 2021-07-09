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

struct FpsManager
{
    sf::Text text;
    bool show;
    bool pressed;
    char line[10];
};
//===================================================================================

void FpsManagerSetup(FpsManager *fps)
{
    set_text_preferences(fps->text);
    fps->show = false;
    fps->pressed = false;
}
//===================================================================================
