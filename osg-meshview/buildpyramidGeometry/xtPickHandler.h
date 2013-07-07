#pragma once
#include "osgcommon.h"



class xtPickHandler
{
public:
	xtPickHandler(void);
	~xtPickHandler(void);
};

class PickHandler : public osgGA::GUIEventHandler 
{
public:
	PickHandler(osgText::Text *updateText):
	  _updateText(updateText) {};

	  ~PickHandler(){}
	  
	  bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	  virtual void pick(osgViewer::View *view, const osgGA::GUIEventAdapter &ea);

	  void setLabel(const std::string &name) 
	  {
		  if (this->_updateText.get()) this->_updateText->setText(name);
	  }



protected:
	osg::ref_ptr<osgText::Text> _updateText;

};

