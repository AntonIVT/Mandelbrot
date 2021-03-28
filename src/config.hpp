#pragma once

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

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
                            0., 0.,   // offset
                            0.01      // scale
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

int get_line(char *buffer, size_t size)
{
    size_t cnt = 0;
    int    ch  = 0;

    if(buffer == NULL || size == 0)
        return 0;

    while((ch = getc(stdin)) != EOF && cnt < size - 1)
    {
        if(ch == '\n')
        {
            buffer[cnt] = 0;
            return 1;
        }

        buffer[cnt++] = ch;
    }

    buffer[cnt] = 0; 
    return 1;
}

//===================================================================================

const size_t MAX_CFG_SIZE = 1000;
const size_t MAX_CFG_NAME_SIZE = 100;

int load_config(config cfg, double x_offset, double y_offset, double graph_dot)
{
    FILE* cfg_file = fopen("custom_configes.cfg", "ab");
    if (cfg_file == NULL) return -1;

    char new_cfg[MAX_CFG_SIZE] = {0};
    int offset = 0;
    char new_cfg_name[MAX_CFG_NAME_SIZE + 1] = {0};

    printf("Please, print new config name:\n");
    get_line(new_cfg_name, MAX_CFG_NAME_SIZE + 1);
    offset += sprintf(new_cfg, "/=================================================\\\n"
                              "=|%s|=\n", new_cfg_name);
    
    offset += sprintf(new_cfg + offset, "WINDOW_WIDTH:   %i\n"
                                        "WINDOW_HEIGHT:  %i\n"
                                        "COLOR_WIDTH:    %i\n"
                                        "COLOR_OFFSET:   %i\n"
                                        "MAX_ITERATIONS: %i\n"
                                        "MAX_RADIUS^2:   %20.16lg\n"
                                        "OFFSET_X:       %20.16lg\n"
                                        "OFFSET_Y:       %20.16lg\n"
                                        "SCALE:          %20.16lg\n", 
                                        cfg.window_width, 
                                        cfg.window_height, 
                                        cfg.color_width,
                                        cfg.color_offset,
                                        cfg.n_max, 
                                        cfg.r_max2,
                                        x_offset,
                                        y_offset,
                                        graph_dot);

    sprintf(new_cfg + offset, "\\=================================================/\n");

    fprintf(cfg_file, "%s", new_cfg);
    fclose(cfg_file);

    return 0;
}
//===================================================================================

