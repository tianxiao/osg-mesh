#pragma once
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/EventVisitor>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>

#include <Awesomium/STLHelpers.h>
#include <Awesomium/WebKeyboardCodes.h>
#include <Awesomium/WebKeyboardEvent.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/WebCore.h>
#include <Awesomium/JSObject.h>

using namespace Awesomium;
//extern Awesomium::WebCore *g_webCore = NULL;

class xtAwesomGui
{
public:
	xtAwesomGui(void);
	~xtAwesomGui(void);
};

class xtAwesomiumImage :public osg::Image
{
public:
	static Awesomium::WebCore *g_webCore;// = 0;

public:
	xtAwesomiumImage()
		:mWebView(NULL), mLastButtonMask(0) {};

	xtAwesomiumImage( const xtAwesomiumImage &copy, const osg::CopyOp &op=osg::CopyOp::SHALLOW_COPY)
		:osg::Image(copy, op), mWebView(copy.mWebView), mLastButtonMask(copy.mLastButtonMask) {};

	META_Object( osg, xtAwesomiumImage );

	void LoadURL( const std::string &url, int w=0, int h=0 ) 
	{
		if ( !mWebView ) {
			mWebView = g_webCore->CreateWebView( w>0?w:512, h>0?h:512 );
			mWebView->SetTransparent( true );
		} else if ( w>0 && h>0 ) {
			mWebView->Resize( w, h );
		}
		WebURL urlData(WSLit(url.c_str()));
		mWebView->LoadURL( urlData );
		mWebView->Focus();
	}

	virtual bool requiresUpdateCall() const { return true; };

	virtual void update( osg::NodeVisitor *nv ) 
	{
		if ( !mWebView ) return;
		BitmapSurface *surface = static_cast<BitmapSurface*>( mWebView->surface() );
		if ( surface && surface->is_dirty() ) {
			setImage( surface->width(), surface->height(), 1, 4, GL_BGRA, GL_UNSIGNED_BYTE,
				const_cast<unsigned char*>(surface->buffer()), osg::Image::NO_DELETE );
		}
	}

	virtual bool sendPointerEvent( int x, int y, int buttonMask ) 
	{
		if ( !mWebView) return false;
		switch (buttonMask ) 
		{
		case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
			if ( !mLastButtonMask ) mWebView->InjectMouseDown( kMouseButton_Left );
			else mWebView->InjectMouseMove( x, y );
			break;
		case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
			if ( !mLastButtonMask ) mWebView->InjectMouseDown( kMouseButton_Middle );
			else mWebView->InjectMouseMove( x, y );
			break;
		case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
			if ( !mLastButtonMask ) mWebView->InjectMouseDown( kMouseButton_Right );
			else mWebView->InjectMouseMove( x, y );
			break;
		default:
			mWebView->InjectMouseMove( x, y );
			break;
		}

		if ( mLastButtonMask!=0 && buttonMask==0 ) {
			switch (mLastButtonMask)
			{
			case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
				mWebView->InjectMouseUp( kMouseButton_Left );
				break;
			case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
				mWebView->InjectMouseUp( kMouseButton_Right );
				break;
			}
		}

		mLastButtonMask = buttonMask;
		return true;
	}

	virtual bool sendKeyEvent( int key, bool keydown )
	{
		if ( !mWebView ) return false;
		WebKeyboardEvent::Type type = keydown?WebKeyboardEvent::kTypeKeyDown:WebKeyboardEvent::kTypeKeyUp;

		switch ( key ) 
		{
		case osgGA::GUIEventAdapter::KEY_Tab:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_TAB );
			break;
		case osgGA::GUIEventAdapter::KEY_BackSpace:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_BACK );
			break;
		case osgGA::GUIEventAdapter::KEY_Return:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_RETURN);
			break;
		case osgGA::GUIEventAdapter::KEY_Left:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_LEFT);
			break;
		case osgGA::GUIEventAdapter::KEY_Right:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_RIGHT);
			break;
		case osgGA::GUIEventAdapter::KEY_Up:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_UP);
			break;
		case osgGA::GUIEventAdapter::KEY_Down:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_DOWN);
			break;
		case osgGA::GUIEventAdapter::KEY_Page_Down:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_NEXT);
			break;
		case osgGA::GUIEventAdapter::KEY_Page_Up:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_PRIOR);
			break;
		case osgGA::GUIEventAdapter::KEY_Home:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_HOME);
			break;
		case osgGA::GUIEventAdapter::KEY_End:
			injectAwesomiumKey( type, Awesomium::KeyCodes::AK_END);
			break;
		default:
			if ( keydown ) injectAwesomiumKey( WebKeyboardEvent::kTypeChar, key );
			break;
		}

		return true;
	}

protected:
	virtual ~xtAwesomiumImage()
	{
		if ( mWebView ) {
			mWebView->Destroy();
		}
	}

	void injectAwesomiumKey( WebKeyboardEvent::Type type, int key ) 
	{
		WebKeyboardEvent keyEvent;
		keyEvent.type = type;
		keyEvent.virtual_key_code = key;
		keyEvent.native_key_code = key;
		keyEvent.text[0] = (char) key; keyEvent.text[1]='\0';
		keyEvent.unmodified_text[0] = (char)key; keyEvent.unmodified_text[1]='\0';
		keyEvent.modifiers = 0;
		keyEvent.is_system_key = false;
		mWebView->InjectKeyboardEvent( keyEvent );
	}


	WebView * mWebView;
	int mLastButtonMask;
};

//xtAwesomiumImage::g_webCore=NULL;



