#if defined(RPI)
	#include <GLES2/gl2.h>
#elif defined(_WIN32)
	#include "glee.h"
#elif defined(__APPLE__)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include <stdio.h>
#include <cairo/cairo.h>
#include <cairo/cairo-gl.h>
#include <SDL2/SDL.h>
#include "cairosdl.h"

SDL_Window *window;
SDL_GLContext glcontext;
SDL_Surface* screen;

#ifdef _WIN32
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	double prevt, cpuTime;
	SDL_Event event;
	int done = 0, i;
	SDL_Surface* screen;
	cairo_device_t *device;
	cairo_t *cr;
	cairo_surface_t *surface;
	int opengl = 0;

	SDL_Init(SDL_INIT_VIDEO);

	if(argc > 1 && strcmp(argv[1], "opengl") == 0)
	{
		printf("OPENGL mode\n");
		opengl = 1;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		window = SDL_CreateWindow("Test SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			1600, 900, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

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

		device = cairo_sdl_device_create(window);
		surface = cairo_sdl_surface_create(device, 1600, 900);
		cr = cairo_create(surface);
		SDL_GL_SetSwapInterval(0);
	}
	else
	{
		printf("SOFTWARE mode\n");
		opengl = 0;

		window = SDL_CreateWindow("Test SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			1600, 900, SDL_WINDOW_SHOWN);

		screen = SDL_GetWindowSurface(window);
	}

	prevt = 0;
	while(!done)
	{
		int x = 10, y = 10;
        int winWidth, winHeight;
		int start = SDL_GetTicks(), end;

        while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYDOWN)
				done = 1;
			if(event.type == SDL_MOUSEBUTTONDOWN)
				done = 1;
		}

		SDL_GetWindowSize(window, &winWidth, &winHeight);

		if(opengl)
		{
        	glViewport(0, 0, winWidth, winHeight);
        	glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
        	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		}
		else
		{
			SDL_LockSurface(screen);
			cr = cairosdl_create(screen);
		}

		cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
		for(i = 0; i < 15000; i++)
		{
			cairo_rectangle(cr, x, y, 6, 6);
			cairo_fill(cr);

			x += 8;
			if(x >= winWidth - 10)
			{
				x = 10;
				y += 10;
			}
		}

		if(opengl)
		{
			cairo_gl_surface_swapbuffers(surface);
			SDL_UpdateWindowSurface(window);
		}
		else
		{
			cairosdl_destroy(cr);
			SDL_UnlockSurface(screen);
			SDL_UpdateWindowSurface(window);
		}

		end = SDL_GetTicks();
		printf("%f, %d\n", 1000.0 / (end - start), end - start);
	}

	if(opengl)
	{
		cairo_surface_destroy(surface);
		cairo_device_destroy(device);
	}

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
