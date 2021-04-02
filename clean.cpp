#include <cmath>
#include <cassert>
#include "src/config.hpp"
#include "src/fps_control.hpp"
#include <immintrin.h>

//===================================================================================

inline sf::Color get_color(long long n, const int colwidth, const int coloffset)
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

inline __m256i get_n(__m256d x0, __m256d y0, int n_max, __m256d r_max2, __m256i _129, __m256i one)
{
    __m256d y_curr = y0;
    __m256d x_curr = x0;

    __m256i n_vec = _mm256_setzero_si256();

    for (int n = 0; n < n_max; n++)
    {
        __m256d x2 = x_curr * x_curr;
        __m256d y2 = y_curr * y_curr;
        __m256d r2 = x2 + y2;
        
        __m256d cmp = _mm256_cmp_pd(r2, r_max2, _CMP_LT_OQ); // r2[i] < r_max2[i]
        int mask = _mm256_movemask_pd(cmp);
        
        if (!mask) // if all 0
            break;

        n_vec = n_vec - _mm256_castpd_si256(cmp);

        __m256d xy = x_curr * y_curr;
        x_curr = x0 + x2 - y2;
        y_curr = y0 + xy + xy;
    }
    
    return n_vec;
}
//===================================================================================

void fill_screen(sf::Uint32 *screen, int window_height, int window_width, double graph_dot, double x_offset, double y_offset, 
                        __m256d zero_to_three, int n_max, __m256d r_max2, __m256i _129, __m256i one, config cfg)
{
    #pragma omp parallel for num_threads(16)
    for (int y_window = 0; y_window < window_height; y_window++)
    {
        double x0 = (-window_width / 2) * graph_dot + x_offset,
               y0 = (y_window - window_height / 2) * graph_dot + y_offset;

        for (int x_window = 0; x_window < window_width; x_window += 4, x0 += 4 * graph_dot)
        {
            __m256i n = get_n(_mm256_set1_pd(x0) + _mm256_set1_pd(graph_dot) * zero_to_three,
                              _mm256_set1_pd(y0), n_max, r_max2, _129, one);
            
            long long *n_ptr = (long long*)&n;
            for (int i = 0; i < 4; i++)
            {
                sf::Color color = sf::Color::Black;
                if (n_ptr[i] < n_max)
                    color = get_color(n_ptr[i], cfg.color_width, cfg.color_offset);

                screen[y_window * window_width + x_window + 3 - i] = __builtin_bswap32(color.toInteger());
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
    fps.text.setFont(font);

    bool is_load = false;

    int window_width  = cfg.window_width - (cfg.window_width % 4),
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

    __m256d r_max2 = _mm256_set1_pd(r_max2_d);
    __m256d zero_to_three = _mm256_set_pd(0., 1., 2., 3.);
    __m256i one  = _mm256_set1_epi64x(1);
    __m256i _129 = _mm256_set1_epi64x(129);

    sf::Clock fps_clock;

    bool isF = false;
    bool isL = false;

    while (window.isOpen())
    {
        //------------------------------------------------------
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))     
            break;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  x_offset  -= graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) x_offset  += graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    y_offset  -= graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  y_offset  += graph_dot * 25.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))  graph_dot *= 1.05;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))     graph_dot /= 1.05;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        {
            if (!isF)
                fps.pressed = true; // is_pressed
            isF = true;
        }
        else isF = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        {
            if (!isL)
                is_load = true;
            isL = true;
        }
        else isL = false;
        //------------------------------------------------------

        if (!isF && fps.pressed)
        {
            fps.show = !fps.show;
            fps.pressed = false;
        }

        if (!isL && is_load)
        {
            is_load = false;
            if (load_config(cfg, x_offset, y_offset, graph_dot) == 0)
                printf("SUCCESSFUL LOAD\n");
            else
                printf("UNSUCCESSFUL LOAD\n");
        }
        //------------------------------------------------------

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //------------------------------------------------------
        
        fill_screen(screen, window_height, window_width, graph_dot, x_offset, y_offset, zero_to_three, n_max, r_max2, _129, one, cfg);

        texture.update((sf::Uint8 *)screen);
        sprite.setTexture(texture);
        window.clear();
        window.draw(sprite);

        if (fps.show)
        {
            sf::Time timer = fps_clock.getElapsedTime();
            sprintf(fps.line, "%i", ((int)(1. / timer.asSeconds())) % 1000);
            fps.text.setString(fps.line);
            window.draw(fps.text);
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

    __m256d r_max2 = _mm256_set1_pd(r_max2_d);
    __m256d zero_to_three = _mm256_set_pd(0., 1., 2., 3.);
     __m256i one  = _mm256_set1_epi64x(1);
    __m256i _129 = _mm256_set1_epi64x(129);

    fill_screen(screen, window_height, window_width, graph_dot, x_offset, y_offset, zero_to_three, n_max, r_max2, _129, one, cfg);

    image.create(window_width, window_height, (sf::Uint8*)screen);
    image.saveToFile(screen_name);

    free(screen);

    return 0;
}
//===================================================================================

int main(int argc, char* argv[])
{
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

