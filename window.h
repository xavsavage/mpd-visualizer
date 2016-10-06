#ifndef _WINDOW_H
#define _WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_events.h>
#include <sys/queue.h>
 

// Event queue for event handling
struct eventq_entry {
	TAILQ_ENTRY(eventq_entry) tailq;
	uint32_t sdl_event_type;
	uint32_t sdl_event_key;
	void (*callback)();
};
TAILQ_HEAD(event_queue, eventq_entry) eventq;

SDL_Window *displayWindow;
SDL_Renderer *displayRenderer;
SDL_RendererInfo displayRendererInfo;

int window_sdl_exited = 0;

void add_event_handler(uint32_t event_type, uint32_t event_key, void (*callback)())
{
	struct eventq_entry *entry;
	entry = malloc(sizeof(*entry));
	entry->sdl_event_type = event_type;
	entry->sdl_event_key = event_key;
	entry->callback = callback;

	TAILQ_INSERT_HEAD(&eventq, entry, tailq);
}

int handle_events()
{
	SDL_Event event;
	struct eventq_entry *eventq_p;

	while(SDL_PollEvent(&event)) {
		TAILQ_FOREACH(eventq_p, &eventq, tailq) {
			if(event.type == eventq_p->sdl_event_type){
				if((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.keysym.sym == eventq_p->sdl_event_key)
						eventq_p->callback();
			}
		}
	}
}

/*
 * In earlier versions, gluPerspective was used instead of glFrustum.
 * Credit goes to nehe.gamedev.net for this function and its implementation.
 */
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;

	fH = tan( (fovY / 2) / 180 * pi ) * zNear;
	fH = tan( fovY / 360 * pi ) * zNear;
	fW = fH * aspect;

	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

void window_init()
{
	// Set background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//
	// Enable depth buffer
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Disable lighting
	glDisable(GL_LIGHTING);

}

void window_viewport_set(int width, int height)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
	SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);

	GLdouble ratio;

	if(height == 0) {
		height = 1;
	}

	ratio = (GLdouble) width / (GLdouble) height;

	// Configure the viewport (the same size as the window)
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set perspective
	perspectiveGL(60.0, ratio, 0.01, 200.0);

	glMatrixMode(GL_MODELVIEW);

	// Reset view
	glLoadIdentity();

	// Center window
	glTranslatef(-13.f, 0.f, -50.f);
	glRotatef(20, 1.f, 0.f, 0.f);

	TAILQ_INIT(&eventq);
}

void window_quit()
{
	window_sdl_exited = 1;
	SDL_Quit();
}

#endif
