#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cmath>
#include <immintrin.h>

//=================================================================================================

inline bool isKard(double x, double y)
{
    double po = sqrt(pow(x - 1./4, 2)+ pow(y,2));
    double poc = 1./2 - 1./2 * cos(atan2(y, x - 1./4));
    if (po <= poc) 
        return true;
    else 
        return false;
}

int main()
{
    const int window_width = 16 * 50,
              window_height = 9 * 50;

    const size_t screen_width = 1920,
                 screen_height = 1080;

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Mandelbrot");
    window.setPosition(sf::Vector2i(1920.f / 2 - window_width / 2, 1080.f / 2 - window_height / 2)); // Set window to the center of screen

    sf::Sprite sprite;
    sf::Texture texture;
    texture.create(window_width, window_height);

    sf::Uint32 screen[window_height][window_width];

    const int nMax = 256;

    double dx = 1.6f / window_width,
           dy = 0.9f / window_height, // rectangle 1.6x0.9
        rMax2 = 50.f;

    double x_offset = -0.743643900055,
           y_offset = 0.131825890901;

    int colwidth = 27;
    int coloffset = 81;

    int coloroffset = 0;

    __m256d _rMax2 = _mm256_set1_pd(rMax2);
    __m256d _0123 = _mm256_set_pd(0., 1., 2., 3.);

    sf::Clock clock;

    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            x_offset -= dx * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            x_offset += dx * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            y_offset -= dy * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            y_offset += dy * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
        {
            dx *= 1.1;
            dy *= 1.1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            dx /= 1.1;
            dy /= 1.1;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (int y_window = 0; y_window < window_height; y_window++)
        {
            double x0 = (-window_width / 2) * dx + x_offset,
                   y0 = (y_window - window_height / 2) * dy + y_offset;

            for (int x_window = 0; x_window < window_width; x_window += 4, x0 += 4 * dx)
            {
                if (isKard(x0, y0) && isKard(x0 + dx, y0) && isKard(x0 + 2*dx, y0) && isKard(x0 + 3*dx, y0))
                {
                    for (int i = 0; i < 4; i++)
                        screen[y_window][x_window + 3 - i] = sf::Color::Black.toInteger();
                    continue;
                }
                
                __m256d _X0 = _mm256_add_pd(_mm256_set1_pd(x0), _mm256_mul_pd(_mm256_set1_pd(dx), _0123));
                __m256d _Y0 = _mm256_set1_pd(y0);

                __m256d _Y = _Y0;
                __m256d _X = _X0;

                int N = 0;
                
                __m256i _N = _mm256_setzero_si256(); // epi64

                for (; N < nMax; N++)
                {
                    __m256d _x2 = _mm256_mul_pd(_X, _X);
                    __m256d _y2 = _mm256_mul_pd(_Y, _Y);

                    __m256d _r2 = _mm256_add_pd(_x2, _y2);

                    __m256d _cmp = _mm256_cmp_pd(_r2, _rMax2, _CMP_LT_OQ); // r2[i] < rmax2[i]
                    int mask = _mm256_movemask_pd(_cmp);
                    if (!mask)
                        break;

                    _N = _mm256_sub_epi64(_N, _mm256_castpd_si256(_cmp)); // N = N - (cmp), cmp = -1 if true

                    __m256d _xy = _mm256_mul_pd(_X, _Y);
                    _X = _mm256_add_pd(_mm256_sub_pd(_x2, _y2), _X0);
                    _Y = _mm256_add_pd(_mm256_add_pd(_xy, _xy), _Y0);
                }

                long long *Np = (long long *)&_N;
                for (int i = 0; i < 4; i++)
                {
                    sf::Color color = sf::Color::Black;
                    if (Np[i] < nMax)
                    {
                        Np[i] = (Np[i] + coloffset) % (3 * colwidth);
                        if (Np[i] / colwidth == 0)
                        {
                            color.r = 237 * Np[i] / colwidth;
                            color.g = 11 + 244 * Np[i] / colwidth;
                            color.b = 135 + 125 * Np[i] / colwidth;
                        }
                        else if (Np[i] / colwidth == 1)
                        {
                            Np[i] -= colwidth;
                            color.r = 238;
                            color.g = 255 - 123 * Np[i] / colwidth;
                            color.b = 255 - 253 * Np[i] / colwidth;
                        }
                        else
                        {
                            Np[i] -= 2 * colwidth;
                            color.r = 230 - 239 * Np[i] / colwidth;
                            color.g = 130 - 121 * Np[i] / colwidth;
                            color.b = 12 + 114 * Np[i] / colwidth;
                        }
                    }

                    screen[y_window][x_window + 3 - i] = __builtin_bswap32(color.toInteger());
                }
            }
        }

        sf::Time timer = clock.getElapsedTime();
        clock.restart();
        fflush(stdout);
        printf("%lg\r", 1. / timer.asSeconds());
        texture.update((sf::Uint8 *)screen);
        sprite.setTexture(texture);

        window.clear();
        window.draw(sprite);
        window.display();
    }
}
