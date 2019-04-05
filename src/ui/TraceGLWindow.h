//
// TraceGLWindow.h
//
// The header file of the view for rendered image
//

#ifndef TRACE_GL_WINDOW_H_
#define TRACE_GL_WINDOW_H_

// ReSharper disable once CppUnusedIncludeDirective
#include <FL/Fl.H>

#include <FL/Fl_Gl_Window.H>
// ReSharper disable once CppUnusedIncludeDirective
#include <FL/gl.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <GL/gl.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <GL/glu.h>

#include "../RayTracer.h"

class TraceGLWindow final : public Fl_Gl_Window
{
public:
	TraceGLWindow(int x, int y, int w, int h, const char *l);
	void draw() override;
	int handle(int event) override;

	RayTracer *rayTracer;

	void refresh();

	void resizeWindow(int width, int height);

	void saveImage(char *iName);

	void setRayTracer(RayTracer *tracer);

private:
	int mNWindowWidth, mNWindowHeight;
	int mNDrawWidth, mNDrawHeight;
};

#endif // TRACE_GL_WINDOW_H_