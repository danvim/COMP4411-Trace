//
// rayUI.h
//
// The header file for the UI part
//

#ifndef RAY_UI_H_
#define RAY_UI_H_

#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>

#include <FL/Fl_File_Chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUi
{
public:
	TraceUi();

	// The FLTK widgets
	Fl_Window* mMainWindow;
	Fl_Menu_Bar* mMenuBar;

	Fl_Slider* mSizeSlider{};
	Fl_Slider* mDepthSlider{};
	Fl_Slider* mDistAtteASlider{};
	Fl_Slider* mDistAtteBSlider{};
	Fl_Slider* mDistAtteCSlider{};

	Fl_Button* mRenderButton;
	Fl_Button* mStopButton;

	TraceGLWindow* mTraceGlWindow;

	// member functions
	void show() const;

	void setRayTracer(RayTracer* tracer);

	int getSize() const;
	int getDepth() const;

private:
	RayTracer* rayTracer{};

	int mNSize;
	int mNDepth;

	// static class members
	static Fl_Menu_Item menuItems[];

	static TraceUi* whoami(Fl_Menu_* o);

	static Fl_Callback cbLoadScene;
	static Fl_Callback cbSaveImage;
	static Fl_Callback cbExit;
	static Fl_Callback cbAbout;

	static Fl_Callback cbExit2;

	static Fl_Callback cbSizeSlides;
	static Fl_Callback cbDepthSlides;
	static Fl_Callback cbDistAtteASlides;
	static Fl_Callback cbDistAtteBSlides;
	static Fl_Callback cbDistAtteCSlides;

	static Fl_Callback cbRender;
	static Fl_Callback cbStop;
};

#endif
