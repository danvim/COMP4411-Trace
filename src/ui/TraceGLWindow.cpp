// TraceGLWindow
// A subclass of FL_GL_Window that handles drawing the traced image to the screen
// 

#include "TraceGLWindow.h"
#include "../RayTracer.h"

#include "../fileio/bitmap.h"

TraceGLWindow::TraceGLWindow(int x, int y, int w, int h, const char *l)
	: Fl_Gl_Window(x, y, w, h, l), rayTracer(nullptr), mNDrawWidth(0), mNDrawHeight(0)
{
	mNWindowWidth = w;
	mNWindowHeight = h;
}

int TraceGLWindow::handle(int event)
{
	// disable all mouse and keyboard events
	return 1;
}

void TraceGLWindow::draw()
{
	if(!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		ortho();

		mNWindowWidth=w();
		mNWindowHeight=h();
	}

	glClear( GL_COLOR_BUFFER_BIT );

	unsigned char* buf;
	rayTracer->getBuffer(buf, mNDrawWidth, mNDrawHeight);

	if ( buf ) {
		// just copy image to GL window conceptually
		glRasterPos2i( 0, 0 );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, mNDrawWidth );
		glDrawBuffer( GL_BACK );
		glDrawPixels( mNDrawWidth, mNDrawHeight, GL_RGB, GL_UNSIGNED_BYTE, buf );
	}
		
	glFlush();
}

void TraceGLWindow::refresh()
{
	redraw();
}

void TraceGLWindow::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
	mNWindowWidth=w();
	mNWindowHeight=h();
}

void TraceGLWindow::saveImage(char *iName)
{
	unsigned char* buf;

	rayTracer->getBuffer(buf, mNDrawWidth, mNDrawHeight);
	if (buf)
		writeBmp(iName, mNDrawWidth, mNDrawHeight, buf); 
}

void TraceGLWindow::setRayTracer(RayTracer *tracer)
{
	rayTracer = tracer;
}