#if defined(RPI)
	#include <GLES2/gl2.h>
#elif defined(_WIN32)
	#include "glee.h"
#else
	#include <OpenGL/gl.h>
#endif

#include <stdio.h>
#include <cairo/cairo.h>
#include <cairo/cairo-gl.h>
#include <SDL2/SDL.h>

#ifdef _WIN32
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	double prevt, cpuTime;
	SDL_Window *window;
	SDL_GLContext glcontext;
	SDL_Event event;
	int done = 0, i;
	cairo_device_t *device;
	cairo_t *cr;
	cairo_surface_t *surface;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow("Test SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1300, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	glcontext = SDL_GL_CreateContext(window);
	if(!glcontext)
	{
		printf("got error %s\n", SDL_GetError());
		return 1;
	}

	if(SDL_GL_MakeCurrent(window, glcontext) < 0)
	{
		printf("OpenGL context could not be made current! SDL Error: %s\n", SDL_GetError());
		return 1;
	}

printf("%s\n", glGetString(GL_VERSION));

printf("a\n");
	device = cairo_sdl_device_create(window);
printf("b\n");
	surface = cairo_sdl_surface_create(device, 1300, 800);
printf("c\n");
	cr = cairo_create(surface);
printf("d\n");

	SDL_GL_SetSwapInterval(0);

	prevt = 0;
	while(!done)
	{
		int x = 10, y = 10;
        int winWidth, winHeight;
        int fbWidth, fbHeight;
        float pxRatio;
		int start = SDL_GetTicks(), end;

        while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYDOWN)
				done = 1;
			if(event.type == SDL_MOUSEBUTTONDOWN)
				done = 1;
		}

		SDL_GetWindowSize(window, &winWidth, &winHeight);
		SDL_GetWindowSize(window, &fbWidth, &fbHeight);
        // Calculate pixel ration for hi-dpi devices.
        pxRatio = (float)fbWidth / (float)winWidth;

        // Update and render
        glViewport(0, 0, winWidth, winHeight);
        glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		for(i = 0; i < 15000; i++)
		{
			cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
			cairo_rectangle(cr, x, y, 8, 8);
			cairo_fill(cr);

			x += 10;
			if(x >= winWidth - 10)
			{
				x = 10;
				y += 10;
			}
		}

		cairo_gl_surface_swapbuffers(surface);
		end = SDL_GetTicks();

		printf("%f, %d\n", 1000.0 / (end - start), end - start);
	}

	cairo_surface_destroy(surface);
	cairo_device_destroy(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
