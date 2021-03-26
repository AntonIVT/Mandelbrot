#include <cmath>
#include <immintrin.h>
#include <SFML/Graphics.hpp>

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

inline __m256i get_n(__m256 x0, __m256 y0, int n_max, __m256 r_max2)
{
    __m256 y_curr = y0;
    __m256 x_curr = x0;

    __m256i n_vec = _mm256_setzero_si256();

    for (int n = 0; n < n_max; n++)
    {
        __m256 x2 = _mm256_mul_ps(x_curr, x_curr);
        __m256 y2 = _mm256_mul_ps(y_curr, y_curr);
        __m256 r2 = _mm256_add_ps(x2, y2);

        __m256 cmp = _mm256_cmp_ps(r2, r_max2, _CMP_LT_OQ); // r2[i] < r_max2[i]
        int mask = _mm256_movemask_ps(cmp);
        if (!mask) // if all 0
            break;

        n_vec = _mm256_sub_epi32(n_vec, _mm256_castps_si256(cmp));

        __m256 xy = _mm256_mul_ps(x_curr, y_curr);
        x_curr = _mm256_add_ps(x0, _mm256_sub_ps(x2, y2));
        y_curr = _mm256_add_ps(y0, _mm256_add_ps(xy, xy));
    }
    
    return n_vec;
}
//===================================================================================

inline bool is_kard(double x, double y, double graph_dot)
{
    double po   = 0;
    double poc  = 0;
    bool   flag = true;

    for (int i = 0; i < 8; i++)
    {
        po = sqrt(pow(graph_dot*i + x -1./4, 2) + pow(y, 2));
        poc = 1./2 - 1./2*cos(atan2(y, graph_dot * i + x - 1./4));

        if (po > poc)
            flag = false;
    }
    return flag;
}
//===================================================================================

const int window_width  = 16 * 50,
          window_height = 9  * 50;

inline void fill_screen(sf::Uint32 screen[window_height][window_width], double graph_dot, double x_offset, double y_offset, 
                        __m256 zero_to_three, int n_max, __m256 r_max2)
{
    for (int y_window = 0; y_window < window_height; y_window++)
    {
        double x0 = (-window_width / 2) * graph_dot + x_offset,
               y0 = (y_window - window_height / 2) * graph_dot + y_offset;

        for (int x_window = 0; x_window < window_width; x_window += 8, x0 += 8 * graph_dot)
        {
            if (is_kard(x0, y0, graph_dot))
            {
                for (int i = 0; i < 8; i++)
                    screen[y_window][x_window + i] = __builtin_bswap32(sf::Color::Black.toInteger());
                continue;
            }

            __m256i n = get_n(_mm256_add_ps(_mm256_set1_ps(x0),_mm256_mul_ps(_mm256_set1_ps(graph_dot), zero_to_three)),
                              _mm256_set1_ps(y0), 
                              n_max, 
                              r_max2);
            
            int *n_ptr = (int*)&n;
            for (int i = 0; i < 8; i++)
            {
                sf::Color color = sf::Color::Black;
                if (n_ptr[i] < n_max)
                    color = get_color(n_ptr[i], 27, 81);

                screen[y_window][x_window + 7 - i] = __builtin_bswap32(color.toInteger());
            }
        }
    }
}
//===================================================================================

int video_mod()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Mandelbrot");
    window.setPosition(sf::Vector2i(1920.f / 2 - window_width / 2, 1080.f / 2 - window_height / 2)); // Set window to the center of screen

    sf::Sprite  sprite;
    sf::Texture texture;
    texture.create(window_width, window_height);

    sf::Uint32 screen[window_height][window_width] = {0};

    int n_max = 100;

    double graph_dot = 0.001,
           r_max2_d  = 4;

    double x_offset = 0,
           y_offset = 0;

    __m256 r_max2 = _mm256_set1_ps(r_max2_d);
    __m256 zero_to_three = _mm256_set_ps(0., 1., 2., 3., 4., 5., 6., 7.);

    sf::Clock fps_clock;

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
        //------------------------------------------------------
    
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //------------------------------------------------------
        
        fill_screen(screen, graph_dot, x_offset, y_offset, zero_to_three, n_max, r_max2);

        sf::Time timer = fps_clock.getElapsedTime();
        fps_clock.restart();
        fflush(stdout);
        printf("%lg\r", 1. / timer.asSeconds());
        fps_clock.restart();

        texture.update((sf::Uint8 *)screen);
        sprite.setTexture(texture);
        window.clear();
        window.draw(sprite);
        
        window.display();
        //------------------------------------------------------
    }
    
    return 0;
}
//===================================================================================

int main(int argc, char* argv[])
{
    video_mod();
}
//===================================================================================

