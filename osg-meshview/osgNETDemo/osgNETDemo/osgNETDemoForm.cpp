#include "StdAfx.h"
#include "osgNETDemoForm.h"

using namespace System::Threading;

void osgNETDemo::osgNETDemoForm::InitOSG()
{
	RECT rect;
	HWND mHwnd=(HWND)osgRenderTarget->Handle.ToInt32();
	GetWindowRect(mHwnd, &rect);

	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(mHwnd);
	osg::ref_ptr<osg::GraphicsContext::Traits> traits=new osg::GraphicsContext::Traits;

	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->inheritedWindowData = windata;

	// We must set the pixelformat before we can create the OSG Rendering Surface
	//
	PIXELFORMATDESCRIPTOR pixelFormat =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		24,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HDC hdc = ::GetDC(mHwnd);
	if (hdc==0)
	{
		::DestroyWindow(mHwnd);
		return ;
	}

	int pixelFormatIndex = ::ChoosePixelFormat(hdc, &pixelFormat);
	if (pixelFormatIndex==0)
	{
		::ReleaseDC(mHwnd, hdc);
		::DestroyWindow(mHwnd);
		return ;
	}

	if (!::SetPixelFormat(hdc, pixelFormatIndex, &pixelFormat))
	{
		::ReleaseDC(mHwnd, hdc);
		::DestroyWindow(mHwnd);
		return ;
	}

	osg::ref_ptr<osg::GraphicsContext> gc=osg::GraphicsContext::createGraphicsContext(traits.get());
	osg::ref_ptr<osg::Camera> camera=new osg::Camera;
	camera->setGraphicsContext(gc.get());
	camera->setViewport(new osg::Viewport(0,0,traits->width,traits->height));
	camera->setDrawBuffer(GL_BACK);
	camera->setReadBuffer(GL_BACK);
	
	osgViewer=new osgViewer::Viewer;
	osgViewer->setCameraManipulator(new osgGA::TrackballManipulator);
	osgViewer->getCamera()->setClearColor(osg::Vec4(0.8,0.8,0.8,1));
	osgViewer->addSlave(camera.get());
}

void osgNETDemo::osgNETDemoForm::RunOSG()
{
	OSGthread=gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &osgNETDemoForm::OSGThread));
	OSGthread->Priority=Threading::ThreadPriority::BelowNormal;
	OSGthread->Start();
}

void osgNETDemo::osgNETDemoForm::OSGThread()
{
	dummyViewerObject=gcnew Object;
	osgViewer->setDone(false);
	int frameCount=0;
	while(!osgViewer->done())
	{
		Monitor::Enter(dummyViewerObject);		//You must use a monitor to prevent another thread to update scene while viewer is rendering
		osgViewer->frame();
		frameRateToolStripStatusLabel->Text="Frame rendered "+ frameCount;
		frameCount++;
		Monitor::Exit(dummyViewerObject);			//Now after exit, you can do what you want
	}
}