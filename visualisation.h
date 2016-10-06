#ifndef _VISUALISATION_H
#define _VISUALISATION_H

#include <math.h>
#include <SDL2/SDL.h>

#include "fifo.h"
#include "window.h"

#define SPACING 0.4f
//TODO variable sample size
#define SAMPLE_SIZE 64
#define COLOR


void visualizer_init();
void visualizer_loop();
void render(int32_t *samples);
void render_grid(int32_t *samples);
float distance_from_center(GLfloat* point, float center);
void _render_grid(GLfloat *b, GLfloat *c, GLfloat *d, int32_t *samples);

//TODO implement colors
enum colors
{
	WHITE,
	RED,
	GREEN,
	BLUE
};

//TODO implement modes
enum modes
{
	VIEW_WAVE,
	VIEW_SPECTRUM
};

//Window exit flag
extern int window_sdl_exited;

void visualizer_init()
{
	// Open MPD visualisation file
	fifo_open();

	// Init window
	window_init();
	window_viewport_set(1280, 720);

	add_event_handler(SDL_QUIT, SDLK_UNKNOWN, window_quit);
	add_event_handler(SDL_KEYDOWN, SDLK_q, window_quit);
	add_event_handler(SDL_KEYUP, SDLK_ESCAPE, window_quit);
}

void visualizer_loop()
{
	int32_t *samples;

	while(!window_sdl_exited) {
		handle_events();
		samples = fifo_get_spectrum(SAMPLE_SIZE, 100);
		if(samples)
			render(samples);
		free(samples);
	}
}

void render(int32_t *samples)
{
	// Clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Recolor
	float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor3fv(color);

	render_grid(samples);

	SDL_GL_SwapWindow(displayWindow);
}

//TODO not void
void render_grid(int32_t *samples)
{
	//B---C
	//|   |
	//A---D
	GLfloat a[3], b[3], c[3], d[3];
	a[0] = a[2] = b[0] = d[2] = 0.0f;
	b[2] = c[0] = c[2] = d[0] = 0.0f + SPACING;
	

	glBegin(GL_LINES);
	_render_grid(b, c, d, samples);
	glEnd();
}

float distance_from_center(GLfloat* point, float center)
{
	return sqrt(((center - point[0])*(center - point[0])) + 
				((center - point[2])*(center - point[2])));
}

void _render_grid(GLfloat *b, GLfloat *c, GLfloat *d, int32_t *samples)
{
	//Distance from center = index in the spectrum/wave array
	float center = (SPACING * (SAMPLE_SIZE - 1) ) / 2;

	int pos_b = (int)distance_from_center(b, center);
	int pos_c = (int)distance_from_center(c, center);
	int pos_d = (int)distance_from_center(d, center);

	b[1] = *(samples + pos_b) * (1.0 / pos_b);
	c[1] = *(samples + pos_c) * (1.0 / pos_c);
	d[1] = *(samples + pos_d) * (1.0 / pos_d);

	glVertex3fv(b);
	glVertex3fv(c);
	glVertex3fv(c);
	glVertex3fv(d);

	//Edges
	if(b[0] == 0.0f)
	{
		glVertex3f(b[0], b[1], d[2]);
		glVertex3fv(b);
	}
	if(d[2] == 0.0f)
	{
		glVertex3f(b[0], b[1], d[2]);
		glVertex3fv(d);
	}
	
	//Display upper grid square when x is 0
	if(b[0] == 0.f && b[2] <= SPACING * SAMPLE_SIZE - 1)
	{
		GLfloat b2[3];
		GLfloat c2[3];
		GLfloat d2[3];

		b2[0] = b[0];
		c2[0] = c[0];
		d2[0] = d[0];

		b2[2] = b[2] + SPACING;
		c2[2] = c[2] + SPACING;
		d2[2] = d[2] + SPACING;
		_render_grid(b2, c2, d2, samples);
	}
	//Display the square on the right
	if(c[0] <= SPACING*SAMPLE_SIZE && c[2] <= SPACING * SAMPLE_SIZE)
	{
		b[0] += SPACING;
		d[0] += SPACING;
		c[0] += SPACING;
		_render_grid(b, c, d, samples);
	}
}

#endif
