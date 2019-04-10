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
	Fl_Slider* mTerminationThresholdSlider{};
	Fl_Slider* mSuperSampleSlider{};
	Fl_Slider* mBackgroundSlider{};

	Fl_Button* mRenderButton;
	Fl_Button* mStopButton;
	Fl_Check_Button* mSoftShadowButton;
	Fl_Check_Button* mGlossyReflectionButton;
	Fl_Check_Button* mMotionBlurButton;

	TraceGLWindow* mTraceGlWindow;

	// member functions
	void show() const;

	void setRayTracer(RayTracer* tracer);

	int getSize() const;
	int getDepth() const;
	double distAtteA = 0, distAtteB = 0, distAtteC = 0;
	double terminationThreshold = 0;
	int superSample = 1;
	bool isUsingBackground = false;

	unsigned char* backgroundPtr = nullptr;
	int backgroundWidth = 0;
	int backgroundHeight = 0;

	static char* scenePath;
	bool softShadow = false, glossyReflection = false, motionBlur = false;

private:
	RayTracer* rayTracer = nullptr;

	int mNSize;
	int mNDepth;

	// static class members
	static Fl_Menu_Item menuItems[];

	static TraceUi* whoami(Fl_Menu_* o);

	static Fl_Callback cbLoadScene;
	static Fl_Callback cbLoadBackground;
	static Fl_Callback cbSaveImage;
	static Fl_Callback cbExit;
	static Fl_Callback cbAbout;

	static Fl_Callback cbExit2;

	static Fl_Callback cbSizeSlides;
	static Fl_Callback cbDepthSlides;
	static Fl_Callback cbDistAtteASlides;
	static Fl_Callback cbDistAtteBSlides;
	static Fl_Callback cbDistAtteCSlides;
	static Fl_Callback cbTerminationThresholdSlides;
	static Fl_Callback cbSuperSampleSlides;
	static Fl_Callback cbBackgroundSlides;

	static Fl_Callback cbRender;
	static Fl_Callback cbStop;

	static Fl_Callback cbSoftShadow;
	static Fl_Callback cbGlossyReflection;
	static Fl_Callback cbMotionBlur;
};

#endif
