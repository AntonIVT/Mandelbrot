#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>
#include <immintrin.h>

//===================================================================================
/*                                      Class                                      */
union _vector_ll
{
    _vector_ll();

    _vector_ll(long long var);
    _vector_ll(long long var1, long long var2, long long var3, long long var4);
    _vector_ll(__m256i other);
    _vector_ll(const _vector_ll &other);
    _vector_ll(_vector_ll &&other);

    long long &operator[](size_t i);
    _vector_ll &operator=(const _vector_ll &other);
    _vector_ll &operator=(_vector_ll &&other);

    operator __m256i() const;

    ~_vector_ll();

private:
    __m256i vector_epi64;
    long long vector_ptr[4];
};

//===================================================================================
/*                                     Methods                                     */
inline _vector_ll::_vector_ll()
{
}
//-----------------------------------------------
inline _vector_ll::_vector_ll(long long var)
{
    vector_epi64 = _mm256_set1_epi64x(var);
}
inline _vector_ll::_vector_ll(long long var1, long long var2, long long var3, long long var4)
{
    vector_epi64 = _mm256_set_epi64x(var1, var2, var3, var4);
}
inline _vector_ll::_vector_ll(__m256i other)
{
    vector_epi64 = other;
}
inline _vector_ll::_vector_ll(const _vector_ll &other)
{
    vector_epi64 = other.vector_epi64;
}
inline _vector_ll::_vector_ll(_vector_ll &&other)
{
    vector_epi64 = other.vector_epi64;
}
//-----------------------------------------------
inline long long &_vector_ll::operator[](size_t i)
{
    return vector_ptr[3 - i];
}
inline _vector_ll &_vector_ll::operator=(const _vector_ll &other)
{
    vector_epi64 = other.vector_epi64;
    return *this;
}
inline _vector_ll &_vector_ll::operator=(_vector_ll &&other)
{
    vector_epi64 = other.vector_epi64;
    return *this;
}
//-----------------------------------------------
_vector_ll::operator __m256i() const
{
    return vector_epi64;
}
//-----------------------------------------------
inline _vector_ll::~_vector_ll()
{
}
//===================================================================================
/*                                    Functions                                    */

inline _vector_ll operator+(const _vector_ll &first, const _vector_ll &second)
{
    return _mm256_add_epi64(first, second); // 2 casts and constructor
}
inline _vector_ll operator-(const _vector_ll &first, const _vector_ll &second)
{
    return _mm256_sub_epi64(first, second); // 2 casts and constructor
}
//===================================================================================

//===================================================================================
/*                                      Class                                      */
union _vector_d
{
    _vector_d();

    _vector_d(double var);
    _vector_d(double var1, double var2, double var3, double var4);
    _vector_d(__m256d other);
    _vector_d(const _vector_d &other);
    _vector_d(_vector_d &&other);

    double &operator[](size_t i);
    _vector_d &operator=(const _vector_d &other);
    _vector_d &operator=(_vector_d &&other);

    operator __m256d() const;
    operator _vector_ll() const;

    ~_vector_d();

private:
    __m256d vector_pd;
    double vector_ptr[4];
};

//===================================================================================
/*                                     Methods                                     */
inline _vector_d::_vector_d()
{
}
//-----------------------------------------------
inline _vector_d::_vector_d(double var)
{
    vector_pd = _mm256_set1_pd(var);
}
inline _vector_d::_vector_d(double var1, double var2, double var3, double var4)
{
    vector_pd = _mm256_set_pd(var1, var2, var3, var4);
}
inline _vector_d::_vector_d(__m256d other)
{
    vector_pd = other;
}
inline _vector_d::_vector_d(const _vector_d &other)
{
    vector_pd = other.vector_pd;
}
inline _vector_d::_vector_d(_vector_d &&other)
{
    vector_pd = other.vector_pd;
}
//-----------------------------------------------
inline double &_vector_d::operator[](size_t i)
{
    return vector_ptr[3 - i];
}
inline _vector_d &_vector_d::operator=(const _vector_d &other)
{
    vector_pd = other.vector_pd;
    return *this;
}
inline _vector_d &_vector_d::operator=(_vector_d &&other)
{
    vector_pd = other.vector_pd;
    return *this;
}
//-----------------------------------------------
_vector_d::operator __m256d() const
{
    return vector_pd;
}
_vector_d::operator _vector_ll() const
{
    return _mm256_castpd_si256(vector_pd);
}
//-----------------------------------------------
inline _vector_d::~_vector_d()
{
}
//===================================================================================
/*                                    Functions                                    */

inline _vector_d operator+(const _vector_d &first, const _vector_d &second)
{
    return _mm256_add_pd(first, second); // 2 casts and constructor
}
inline _vector_d operator-(const _vector_d &first, const _vector_d &second)
{
    return _mm256_sub_pd(first, second); // 2 casts and constructor
}
inline _vector_d operator*(const _vector_d &first, const _vector_d &second)
{
    return _mm256_mul_pd(first, second); // 2 casts and constructor
}
inline _vector_d operator/(const _vector_d &first, const _vector_d &second)
{
    return _mm256_div_pd(first, second); // 2 casts and constructor
}
//-----------------------------------------------
template <int imm8>
inline bool cmp(const _vector_d &first, const _vector_d &second)
{
    int mask = _mm256_movemask_pd(_mm256_cmp_pd(first, second, imm8)); // 2 casts
    return (mask == 16 - 1) ? true : false;                            // 0...01111
}
inline _vector_d sqrt(const _vector_d &other)
{
    return _mm256_sqrt_pd(other); //cast and constructor
}
template <int imm8>
inline _vector_d get_cmp(const _vector_d &first, const _vector_d &second)
{
    return _mm256_cmp_pd(first, second, imm8);
}
inline int get_mask(const _vector_d &cmp)
{
    return _mm256_movemask_pd(cmp);
}
//===================================================================================

inline sf::Color get_color(int n, const int colwidth, const int coloffset) // add config
{
    sf::Color color;
    n = (n + coloffset) % (3 * colwidth);

    if (n / colwidth == 0)
    {
        //color.r = 237 * n / colwidth;
        //color.g = 11 + 244 * n / colwidth;
        //color.b = 135 + 125 * n / colwidth;
        color.r = 237 * n / colwidth;
        color.g = 11 + 244 * n / colwidth;
        color.b = 116 + 125 * n / colwidth;
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
        color.b = 12 + 114 * n / colwidth;
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

struct config
{
    int window_width;
    int window_height;
    
    int color_width;
    int color_offset;

    int n_max;
    double r_max2;

    double x_offset;
    double y_offset;

    double graph_scale;
};

//===================================================================================

int get_file_size(FILE *file)
{
    assert(file != NULL);

	int _file_size = -1;

	fseek(file, 0, SEEK_END); //Puts pointer in file to the end
	_file_size = ftell(file); //ftell() checks how many bytes from start of file to the pointer on file
    fseek(file, 0, SEEK_SET);

	return _file_size;
}

//===================================================================================

config get_config()
{
    config std_config =    {800, 450, // window parametres
                            27, 81,   // color parametres
                            256,      // n max
                            40.,      // r max ^ 2
                            0., 0.,     // offset
                            0.00001     // scale
                            };

    FILE* cfg_file = fopen("config.cfg", "rb");
    
    if (cfg_file == NULL)
        return std_config;
    
    int cfg_size = get_file_size(cfg_file);
    char* cfg = (char *)calloc(cfg_size + 1, sizeof(char));

    fread(cfg, sizeof(char), cfg_size, cfg_file);   

    char *cfg_param = NULL;
    int int_param = 0;
    double doub_param = 0.;

    const char *cfg_param_names[] = {"WINDOW_WIDTH:",
                                     "WINDOW_HEIGHT:",
                                     "COLOR_WIDTH:",
                                     "COLOR_OFFSET:",
                                     "MAX_ITERATIONS:",
                                     "MAX_RADIUS^2:",
                                     "OFFSET_X:",
                                     "OFFSET_Y:",
                                     "SCALE:"};

    for (int i = 0; i < sizeof(cfg_param_names) / sizeof(char *); i++)
    {
        if ((cfg_param = strstr(cfg, cfg_param_names[i])) != NULL)
        {
            if (i >= 5)
            {
                sscanf(cfg_param + strlen(cfg_param_names[i]), "%lg", &doub_param);
                if      (i == 5) std_config.r_max2      = doub_param;
                else if (i == 6) std_config.x_offset    = doub_param;
                else if (i == 7) std_config.y_offset    = doub_param;
                else if (i == 8) std_config.graph_scale = doub_param;
            }
            else
            {
                sscanf(cfg_param + strlen(cfg_param_names[i]), "%i", &int_param);
                if      (i == 0) std_config.window_width  = int_param;
                else if (i == 1) std_config.window_height = int_param;
                else if (i == 2) std_config.color_width   = int_param;
                else if (i == 3) std_config.color_offset  = int_param;
                else if (i == 4) std_config.n_max         = int_param;
            }
        }
    }

    free(cfg);
    fclose(cfg_file);

    return std_config;
}

//===================================================================================

void add_config(config cfg)
{
    
}

//===================================================================================

inline bool isKard(double x, double y, double graph_dot)
{
    double po = 0;
    double poc = 0;
    bool flag = true;

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

inline void fill_screen(sf::Uint32 *screen, int window_height, int window_width, double graph_dot, double x_offset, double y_offset, 
                        _vector_d zero_to_three, int n_max, _vector_d r_max2, config cfg)
{
    for (int y_window = 0; y_window < window_height; y_window++)
    {
        double x0 = (-window_width / 2) * graph_dot + x_offset,
               y0 = (y_window - window_height / 2) * graph_dot + y_offset;

        for (int x_window = 0; x_window < window_width; x_window += 4, x0 += 4 * graph_dot)
        {
            if (isKard(x0, y0, graph_dot))
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

void video_mod(config cfg)
{
    fps_control fps;
    sf::Font font;
    assert(font.loadFromFile("arial.ttf"));
    get_fps_control(&fps);
    fps.text_fps.setFont(font);

    int       window_width  = cfg.window_width,
              window_height = cfg.window_height;

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Mandelbrot");
    window.setPosition(sf::Vector2i(1920.f / 2 - window_width / 2, 1080.f / 2 - window_height / 2)); // Set window to the center of screen

    sf::Sprite sprite;
    sf::Texture texture;
    texture.create(window_width, window_height);

    sf::Uint32 *screen = (sf::Uint32 *)calloc(window_height * window_width, sizeof(sf::Uint32));

    int n_max = cfg.n_max;

    double graph_dot  = window_width  * cfg.graph_scale,
           r_max2_d = cfg.r_max2;

    double x_offset = cfg.x_offset,
           y_offset = cfg.y_offset;

    _vector_d r_max2(r_max2_d);
    _vector_d zero_to_three(0., 1., 2., 3.);

    sf::Clock fps_clock;
    sf::Clock key_clock;

    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            x_offset -= graph_dot * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            x_offset += graph_dot * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            y_offset -= graph_dot * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            y_offset += graph_dot * 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
        {
            graph_dot *= 1.1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            graph_dot /= 1.1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        {
            fps.when_fps_pressed = key_clock.getElapsedTime();
            if (fps.when_fps_pressed.asSeconds() > 0.05 && fps.when_fps_pressed.asSeconds() < 0.25)
                fps.is_fps_pressed = true;
        }
    
        sf::Time one_sec = key_clock.getElapsedTime();
        if (one_sec.asSeconds() >= 0.3)
        {
            if (fps.is_fps_pressed)
            {
                fps.is_fps_show = !fps.is_fps_show;
                fps.is_fps_pressed = false;
            }

            key_clock.restart();
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

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
    }

    free(screen);
}

//===================================================================================

void screen_mod(config cfg)
{
    int window_width = cfg.window_width,
        window_height = cfg.window_height;

    sf::Image image;

    sf::Uint32 *screen = (sf::Uint32 *)calloc(window_height * window_width, sizeof(sf::Uint32));

    int n_max = cfg.n_max;

    double graph_dot = window_width * cfg.graph_scale,
           r_max2_d = cfg.r_max2;

    double x_offset = cfg.x_offset,
           y_offset = cfg.y_offset;

    _vector_d r_max2(r_max2_d);
    _vector_d zero_to_three(0., 1., 2., 3.);

    fill_screen(screen, window_height, window_width, graph_dot, x_offset, y_offset, zero_to_three, n_max, r_max2, cfg);

    image.create(window_width, window_height, (sf::Uint8*)screen);
    image.saveToFile("image.jpg");

    free(screen);
}

//===================================================================================

int main(int argc, char* argv[])
{
    const int screen_width  = 1920,
              screen_height = 1080;

    config cfg = get_config();

    if (argc > 1 && !strcmp(argv[1], "screen"))
        screen_mod(cfg);
    else
        video_mod(cfg);
}
