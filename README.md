# Mandelbrot
### This project made for visualization of mandelbrot set.

## Installing
Project supported on Linux only.  
Mandelbrot requires SFML graphics library, AVX2 technology and OpenMP.  
To start clone repository and make project: ``` > make main```  

## Features
* 2 modes: video-mode for viewing set in high FPS and screen-mode for making images of set in high quality.
* To speed up used parallel programming(OpenMP) and AVX.
* The ability to create configs to save the patterns of the set.
* The ability to control FPS.

## Usage
### Config
You can make ```config.cfg``` file and add coordinates, scale, screen resolution and quality of set.  
Here is example of standard setup:
```
WINDOW_WIDTH:   800
WINDOW_HEIGHT:  450
COLOR_WIDTH:    27
COLOR_OFFSET:   81
MAX_ITERATIONS: 256
MAX_RADIUS^2:                     40
OFFSET_X:                          0
OFFSET_Y:                          0
SCALE:                          0.01
```
Program finds first occurrence of keyword (OFFSET_X, COLOR_OFFSET, WINDOW_WIDTH, etc.) and makes setup with number after keyword.
If in file there's no such keyword, program use standard setup.  
Config settings:  
* COLOR_WIDTH and COLOR_OFFSET are used for colorization.  
* MAX_ITERATIONS and MAX_RADIUS^2 are used for quality of set.  
* OFFSET_X and OFFSET_Y show center of set.  
### Program launch
* ```> ./mandelbrot``` launch video-mode with parameters:
* ```> ./mandelbrot screen``` launch screen-mode which makes one image with parameters from ```config.cfg``` and saves it.  


### Video-mode Controlling

* **E**     - exit the program
* **LAlt**  - zoom out
* **X**     - zoom in
* **Left**  - move left
* **Right** - move right
* **Up**    - move up
* **Down**  - move down
* **F**     - enable/disable FPS
* **L**     - load this position and make new config (save to ```custom_configes.cfg```)

### Example

This is how I used this program in total: 
1. Find nice pattern with standard config in video-mode
2. Load this config with name of pattern 
3. Change pattern config's settings: Iterations 20000, radius 100, width 1920, height 1080.
4. Launch screen-mode
5. Done 

![Example of pattern](https://github.com/amanakin/Mandelbrot/blob/main/images/Egg.jpg)

In file ```custom_configes.cfg``` you can find some configes for nice patterns.
In folder images you can find images of this patterns.

