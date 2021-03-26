#include <cmath>
#include <cassert>
#include "src/vector_avx.hpp"
#include "src/config.hpp"
#include "src/fps_control.hpp"

//===================================================================================

inline sf::Color get_color(int n, const int colwidth, const int coloffset)
{
    sf::Color color;
    n = (n + coloffset) % (3 * colwidth);

    if (n / colwidth == 0)
    {
        color.r = 237 * n / colwidth;
        color.g = 11  + 244 * n / colwidth;
        color.b = 135 + 125 * n / colwidth;
    }
    else if (n / colwidth == 1)
    {
        n -= colwidth;
        color.r = 238;
        color.g = 255 - 123 * n / colwidth;
        color.b = 255 - 253 * n / colwidth;
    }
    else
    {
        n -= 2 * colwidth;
        color.r = 230 - 239 * n / colwidth;
        color.g = 130 - 121 * n / colwidth;
        color.b = 12  + 114 * n / colwidth;
    }
    color.a = 255;

    return color;
}
//===================================================================================

inline _vector_ll get_n(_vector_d x0, _vector_d y0, int n_max, _vector_d r_max2)
{
    _vector_d y_curr = y0;
    _vector_d x_curr = x0;

    _vector_ll n_vec = 0;

    for (int n = 0; n < n_max; n++)
    {
        _vector_d x2 = x_curr * x_curr;
        _vector_d y2 = y_curr * y_curr;
        _vector_d r2 = x2 + y2;

        _vector_d cmp = get_cmp<_CMP_LT_OQ>(r2, r_max2); // r2[i] < r_max2[i]
        int mask = get_mask(cmp);
        if (!mask)
            break;

        n_vec = n_vec - cmp;

        _vector_d xy = x_curr * y_curr;
        x_curr = x0 + x2 - y2;
        y_curr = y0 + xy + xy;
    }
    
    return n_vec;
}
//===================================================================================

inline bool is_kard(double x, double y, double graph_dot)
{
    double po   = 0;
    double poc  = 0;
    bool   flag = true;

    for (int i = 0; i < 4; i++)
    {
        po = sqrt(pow(graph_dot*i + x -1./4, 2) + pow(y, 2));
        poc = 1./2 - 1./2*cos(atan2(y, graph_dot * i + x - 1./4));

        if (po > poc)
            flag = false;
    }
    return flag;
}
//===================================================================================

inline void fill_screen(sf::Uint32 *screen, int window_height, int window_width, double graph_dot, double x_offset, double y_offset, 
                        _vector_d zero_to_three, int n_max, _vector_d r_max2, config cfg)
{
    for (int y_window = 0; y_window < window_height; y_window++)
    {
        double x0 = (-window_width / 2) * graph_dot + x_offset,
               y0 = (y_window - window_height / 2) * graph_dot + y_offset;

        for (int x_window = 0; x_window < window_width; x_window += 4, x0 += 4 * graph_dot)
        {
            if (is_kard(x0, y0, graph_dot))
            {
                for (int i = 0; i < 4; i++)
                    screen[y_window * window_width + x_window + i] = __builtin_bswap32(sf::Color::Black.toInteger());
                continue;
            }

            _vector_ll n = get_n(x0 + graph_dot * zero_to_three, y0, n_max, r_max2);

            for (int i = 0; i < 4; i++)
            {
                sf::Color color = sf::Color::Black;
                if (n[i] < n_max)
                    color = get_color(n[i], cfg.color_width, cfg.color_offset);

                screen[y_window * window_width + x_window + i] = __builtin_bswap32(color.toInteger());
            }
        }
    }
}
//===================================================================================

int video_mod(config cfg)
{
    fps_control fps;
    sf::Font font;
    assert(font.loadFromFile("src/arial.ttf"));
    get_fps_control(&fps);
    fps.text_fps.setFont(font);

    sf::Time when_load_pressed;
    bool is_load_pressed = false;

    int window_width  = cfg.window_width,
        window_height = cfg.window_height;

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Mandelbrot");
    window.setPosition(sf::Vector2i(1920.f / 2 - window_width / 2, 1080.f / 2 - window_height / 2)); // Set window to the center of screen

    sf::Sprite  sprite;
    sf::Texture texture;
    texture.create(window_width, window_height);

    sf::Uint32 *screen = (sf::Uint32 *)calloc(window_height * window_width, sizeof(sf::Uint32));

    int n_max = cfg.n_max;

    double graph_dot = cfg.graph_scale,
           r_max2_d  = cfg.r_max2;

    double x_offset = cfg.x_offset,
           y_offset = cfg.y_offset;

    _vector_d r_max2(r_max2_d);
    _vector_d zero_to_three(0., 1., 2., 3.);

    sf::Clock fps_clock;
    sf::Clock key_clock;

    while (window.isOpen())
    {
        //------------------------------------------------------
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            x_offset -= graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            x_offset += graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            y_offset -= graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            y_offset += graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) // Zoom-
            graph_dot *= 1.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))    // Zoom+
            graph_dot /= 1.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            free(screen);
            return 0;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        {
            fps.when_fps_pressed = key_clock.getElapsedTime();
            if (fps.when_fps_pressed.asSeconds() > 0.05 && fps.when_fps_pressed.asSeconds() < 0.25)
                fps.is_fps_pressed = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        {
            when_load_pressed = key_clock.getElapsedTime();
            if (when_load_pressed.asSeconds() > 0.05 && when_load_pressed.asSeconds() < 0.25)
                is_load_pressed = true;
        }
        //------------------------------------------------------
    
        sf::Time one_sec = key_clock.getElapsedTime();
        if (one_sec.asSeconds() >= 0.3)
        {
            if (fps.is_fps_pressed)
            {
                fps.is_fps_show = !fps.is_fps_show;
                fps.is_fps_pressed = false;
            }

            if (is_load_pressed)
            {
                is_load_pressed = false;
                if (load_config(cfg, x_offset, y_offset, graph_dot) == 0)
                    printf("SUCCESSFUL LOAD\n");
                else
                    printf("UNSUCCESSFUL LOAD\n");
            }

            key_clock.restart();
        }
        //------------------------------------------------------

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //------------------------------------------------------
        
        fill_screen(screen, window_height, window_width, graph_dot, x_offset, y_offset, zero_to_three, n_max, r_max2, cfg);

        texture.update((sf::Uint8 *)screen);
        sprite.setTexture(texture);
        window.clear();
        window.draw(sprite);

        if (fps.is_fps_show)
        {
            sf::Time timer = fps_clock.getElapsedTime();
            sprintf(fps.fps_line, "%i", ((int)(1. / timer.asSeconds())) % 1000);
            fps.text_fps.setString(fps.fps_line);
            window.draw(fps.text_fps);
        }
        fps_clock.restart();
        
        window.display();
        //------------------------------------------------------
    }

    free(screen);
    
    return 0;
}
//===================================================================================

int screen_mod(config cfg, const char *screen_name)
{
    int window_width = cfg.window_width,
        window_height = cfg.window_height;

    sf::Image image;

    sf::Uint32 *screen = (sf::Uint32 *)calloc(window_height * window_width, sizeof(sf::Uint32));

    int n_max = cfg.n_max;

    double graph_dot = cfg.graph_scale,
           r_max2_d = cfg.r_max2;

    double x_offset = cfg.x_offset,
           y_offset = cfg.y_offset;

    _vector_d r_max2(r_max2_d);
    _vector_d zero_to_three(0., 1., 2., 3.);

    fill_screen(screen, window_height, window_width, graph_dot, x_offset, y_offset, zero_to_three, n_max, r_max2, cfg);

    image.create(window_width, window_height, (sf::Uint8*)screen);
    image.saveToFile(screen_name);

    free(screen);

    return 0;
}
//===================================================================================

int main(int argc, char* argv[])
{
    const int screen_width  = 1920,
              screen_height = 1080;

    config cfg = get_config();

    if (argc > 1 && !strcmp(argv[1], "screen"))
    {
        const char* screen_name = "image.jpg";
        if (argc > 2)
            screen_name = argv[2];

        screen_mod(cfg, screen_name);
    }
    else
        video_mod(cfg);
}
//===================================================================================

