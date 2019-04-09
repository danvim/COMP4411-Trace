//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <ctime>
#include <cmath>

#include <FL/fl_ask.H>
#include <FL/Fl_Value_Slider.H>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUi* TraceUi::whoami(Fl_Menu_* o) // from menu item back to UI itself
{
	return static_cast<TraceUi*>(o->parent()->user_data());
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUi::cbLoadScene(Fl_Widget* o, void* v)
{
	auto pUi = whoami(dynamic_cast<Fl_Menu_*>(o));

	auto* newFile = fl_file_chooser("Open Scene?", "*.ray", nullptr);

	if (newFile != nullptr)
	{
		char buf[256];

		if (pUi->rayTracer->loadScene(newFile))
		{
			sprintf_s(buf, "Ray <%s>", newFile);
			done = true; // terminate the previous rendering
		}
		else
		{
			sprintf_s(buf, "Ray <Not Loaded>");
		}

		pUi->mMainWindow->label(buf);
	}
}

void TraceUi::cbSaveImage(Fl_Widget* o, void* v)
{
	auto pUi = whoami(dynamic_cast<Fl_Menu_*>(o));

	auto* saveFile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp");
	if (saveFile != nullptr)
	{
		pUi->mTraceGlWindow->saveImage(saveFile);
	}
}

void TraceUi::cbExit(Fl_Widget* o, void* v)
{
	auto pUi = whoami(dynamic_cast<Fl_Menu_*>(o));

	// terminate the rendering
	done = true;

	pUi->mTraceGlWindow->hide();
	pUi->mMainWindow->hide();
}

void TraceUi::cbExit2(Fl_Widget* o, void* v)
{
	auto* pUi = static_cast<TraceUi *>(o->user_data());

	// terminate the rendering
	done = true;

	pUi->mTraceGlWindow->hide();
	pUi->mMainWindow->hide();
}

void TraceUi::cbAbout(Fl_Widget* o, void* v)
{
	fl_message(
		"RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUi::cbSizeSlides(Fl_Widget* o, void* v)
{
	auto* pUi = static_cast<TraceUi*>(o->user_data());

	pUi->mNSize = int(dynamic_cast<Fl_Slider *>(o)->value());
	const auto height = lround(pUi->mNSize / pUi->rayTracer->aspectRatio());
	pUi->mTraceGlWindow->resizeWindow(pUi->mNSize, height);
}

void TraceUi::cbDepthSlides(Fl_Widget* o, void* v)
{
	static_cast<TraceUi*>(o->user_data())->mNDepth = int(dynamic_cast<Fl_Slider *>(o)->value());
}

void TraceUi::cbDistAtteASlides(Fl_Widget* o, void* v)
{
	static_cast<TraceUi*>(o->user_data())->distAtteA = double(dynamic_cast<Fl_Slider *>(o)->value());
}

void TraceUi::cbDistAtteBSlides(Fl_Widget* o, void* v)
{
	static_cast<TraceUi*>(o->user_data())->distAtteB = double(dynamic_cast<Fl_Slider *>(o)->value());
}

void TraceUi::cbDistAtteCSlides(Fl_Widget* o, void* v)
{
	static_cast<TraceUi*>(o->user_data())->distAtteC = double(dynamic_cast<Fl_Slider *>(o)->value());
}

void TraceUi::cbTerminationThresholdSlides(Fl_Widget* o, void* v)
{
	static_cast<TraceUi*>(o->user_data())->terminationThreshold = dynamic_cast<Fl_Slider *>(o)->value();
}

void TraceUi::cbSuperSampleSlides(Fl_Widget* o, void*)
{
	static_cast<TraceUi*>(o->user_data())->superSample = int(dynamic_cast<Fl_Slider *>(o)->value());
}


void TraceUi::cbRender(Fl_Widget* o, void* v)
{
	char buffer[256];

	auto* pUi = static_cast<TraceUi*>(o->user_data());

	if (pUi->rayTracer->sceneLoaded())
	{
		const auto width = pUi->getSize();
		const auto height = lround(width / pUi->rayTracer->aspectRatio());
		pUi->mTraceGlWindow->resizeWindow(width, height);

		pUi->mTraceGlWindow->show();

		pUi->rayTracer->traceSetup(width, height, pUi->superSample);
		pUi->rayTracer->maxDepth = pUi->mNDepth;
		pUi->rayTracer->getScene()->distAtteA = pUi->distAtteA;
		pUi->rayTracer->getScene()->distAtteB = pUi->distAtteB;
		pUi->rayTracer->getScene()->distAtteC = pUi->distAtteC;
		pUi->rayTracer->getScene()->terminationThreshold = pUi->terminationThreshold;

		// Save the window label
		const auto* oldLabel = pUi->mTraceGlWindow->label();

		// start to render here	
		done = false;
		auto prev = clock();

		pUi->mTraceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (auto y = 0; y < height; y++)
		{
			for (auto x = 0; x < width; x++)
			{
				if (done) break;

				// current time
				const auto now = clock();

				// check event every 1/2 second
				if (double(now - prev) / CLOCKS_PER_SEC > 0.5)
				{
					prev = now;

					if (Fl::ready())
					{
						// refresh
						pUi->mTraceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage())
						{
							Fl::flush();
						}
					}
				}

				pUi->rayTracer->tracePixel(x, y);
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready())
			{
				// refresh
				pUi->mTraceGlWindow->refresh();

				if (Fl::damage())
				{
					Fl::flush();
				}
			}
			// update the window label
			sprintf_s(buffer, "(%d%%) %s", int(double(y) / double(height) * 100.0), oldLabel);
			pUi->mTraceGlWindow->label(buffer);
		}
		done = true;
		pUi->mTraceGlWindow->refresh();

		// Restore the window label
		pUi->mTraceGlWindow->label(oldLabel);
	}
}

void TraceUi::cbStop(Fl_Widget* o, void* v)
{
	done = true;
}

void TraceUi::show() const
{
	mMainWindow->show();
}

void TraceUi::setRayTracer(RayTracer* tracer)
{
	rayTracer = tracer;
	mTraceGlWindow->setRayTracer(tracer);
}

int TraceUi::getSize() const
{
	return mNSize;
}

int TraceUi::getDepth() const
{
	return mNDepth;
}

// menu definition
Fl_Menu_Item TraceUi::menuItems[] = {
	{"&File", 0, nullptr, nullptr, FL_SUBMENU},
	{"&Load Scene...", FL_ALT + 'l', cbLoadScene},
	{"&Save Image...", FL_ALT + 's', cbSaveImage},
	{"&Exit", FL_ALT + 'e', cbExit},
	{nullptr},

	{"&Help", 0, nullptr, nullptr, FL_SUBMENU},
	{"&About", FL_ALT + 'a', cbAbout},
	{nullptr},

	{nullptr}
};

TraceUi::TraceUi()
{
	// init.
	mNDepth = 0;
	mNSize = 150;
	mMainWindow = new Fl_Window(100, 40, 320, 200, "Ray <Not Loaded>");
	mMainWindow->user_data(static_cast<void*>(this)); // record self to be used by static callback functions
	// install menu bar
	mMenuBar = new Fl_Menu_Bar(0, 0, 320, 25);
	mMenuBar->menu(menuItems);

	// install slider depth
	mDepthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
	mDepthSlider->user_data(static_cast<void*>(this)); // record self to be used by static callback functions
	mDepthSlider->type(FL_HOR_NICE_SLIDER);
	mDepthSlider->labelfont(FL_COURIER);
	mDepthSlider->labelsize(12);
	mDepthSlider->minimum(0);
	mDepthSlider->maximum(10);
	mDepthSlider->step(1);
	mDepthSlider->value(mNDepth);
	mDepthSlider->align(FL_ALIGN_RIGHT);
	mDepthSlider->callback(cbDepthSlides);

	// install slider size
	mSizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
	mSizeSlider->user_data(static_cast<void*>(this)); // record self to be used by static callback functions
	mSizeSlider->type(FL_HOR_NICE_SLIDER);
	mSizeSlider->labelfont(FL_COURIER);
	mSizeSlider->labelsize(12);
	mSizeSlider->minimum(64);
	mSizeSlider->maximum(512);
	mSizeSlider->step(1);
	mSizeSlider->value(mNSize);
	mSizeSlider->align(FL_ALIGN_RIGHT);
	mSizeSlider->callback(cbSizeSlides);
	
	mDistAtteASlider = new Fl_Value_Slider(10, 75, 180, 20, "Dist Attenuation a");
	mDistAtteASlider->user_data(static_cast<void*>(this)); // record self to be used by static callback functions
	mDistAtteASlider->type(FL_HOR_NICE_SLIDER);
	mDistAtteASlider->labelfont(FL_COURIER);
	mDistAtteASlider->labelsize(12);
	mDistAtteASlider->minimum(0);
	mDistAtteASlider->maximum(1);
	mDistAtteASlider->step(0.01);
	mDistAtteASlider->value(0);
	mDistAtteASlider->align(FL_ALIGN_RIGHT);
	mDistAtteASlider->callback(cbDistAtteASlides);
	
	mDistAtteBSlider = new Fl_Value_Slider(10, 95, 180, 20, "Dist Attenuation b");
	mDistAtteBSlider->user_data(static_cast<void*>(this)); // record self to be used by static callback functions
	mDistAtteBSlider->type(FL_HOR_NICE_SLIDER);
	mDistAtteBSlider->labelfont(FL_COURIER);
	mDistAtteBSlider->labelsize(12);
	mDistAtteBSlider->minimum(0);
	mDistAtteBSlider->maximum(1 * 0.1);
	mDistAtteBSlider->step(0.01* 0.1);
	mDistAtteBSlider->value(0);
	mDistAtteBSlider->align(FL_ALIGN_RIGHT);
	mDistAtteBSlider->callback(cbDistAtteBSlides);
	
	mDistAtteCSlider = new Fl_Value_Slider(10, 115, 180, 20, "Dist Attenuation c");
	mDistAtteCSlider->user_data(static_cast<void*>(this)); // record self to be used by static callback functions
	mDistAtteCSlider->type(FL_HOR_NICE_SLIDER);
	mDistAtteCSlider->labelfont(FL_COURIER);
	mDistAtteCSlider->labelsize(12);
	mDistAtteCSlider->minimum(0);
	mDistAtteCSlider->maximum(1 * 0.05);
	mDistAtteCSlider->step(0.01* 0.05);
	mDistAtteCSlider->value(0);
	mDistAtteCSlider->align(FL_ALIGN_RIGHT);
	mDistAtteCSlider->callback(cbDistAtteCSlides);

	mTerminationThresholdSlider = new Fl_Value_Slider(10, 135, 180, 20, "Term. Threshold");
	mTerminationThresholdSlider->user_data(static_cast<void*>(this));
	mTerminationThresholdSlider->type(FL_HOR_NICE_SLIDER);
	mTerminationThresholdSlider->labelfont(FL_COURIER);
	mTerminationThresholdSlider->labelsize(12);
	mTerminationThresholdSlider->minimum(0.0);
	mTerminationThresholdSlider->maximum(1.0);
	mTerminationThresholdSlider->step(0.01);
	mTerminationThresholdSlider->value(0);
	mTerminationThresholdSlider->align(FL_ALIGN_RIGHT);
	mTerminationThresholdSlider->callback(cbTerminationThresholdSlides);

	mSuperSampleSlider = new Fl_Value_Slider(10, 155, 180, 20, "Super Sampling Factor");
	mSuperSampleSlider->user_data(static_cast<void*>(this));
	mSuperSampleSlider->type(FL_HOR_NICE_SLIDER);
	mSuperSampleSlider->labelfont(FL_COURIER);
	mSuperSampleSlider->labelsize(12);
	mSuperSampleSlider->minimum(1.0);
	mSuperSampleSlider->maximum(10.0);
	mSuperSampleSlider->step(1.0);
	mSuperSampleSlider->value(1.0);
	mSuperSampleSlider->align(FL_ALIGN_RIGHT);
	mSuperSampleSlider->callback(cbSuperSampleSlides);

	mRenderButton = new Fl_Button(240, 27, 70, 25, "&Render");
	mRenderButton->user_data(static_cast<void*>(this));
	mRenderButton->callback(cbRender);

	mStopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
	mStopButton->user_data(static_cast<void*>(this));
	mStopButton->callback(cbStop);

	mMainWindow->callback(cbExit2);
	mMainWindow->when(FL_HIDE);
	mMainWindow->end();

	// image view
	mTraceGlWindow = new TraceGLWindow(100, 150, mNSize, mNSize, "Rendered Image");
	mTraceGlWindow->end();
	mTraceGlWindow->resizable(mTraceGlWindow);
}
