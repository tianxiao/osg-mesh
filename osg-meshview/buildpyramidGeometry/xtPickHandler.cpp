#include "StdAfx.h"
#include "xtPickHandler.h"


xtPickHandler::xtPickHandler(void)
{
}


xtPickHandler::~xtPickHandler(void)
{
}

bool PickHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	switch(ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::PUSH):
		{
			osgViewer::View *view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea);
			return false;
		}
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey()=='c'){
				osgViewer::View *view = dynamic_cast<osgViewer::View*>(&aa);
				osg::ref_ptr<osgGA::GUIEventAdapter> osgevent = new osgGA::GUIEventAdapter(ea);
				osgevent->setX((ea.getXmin()+ea.getXmax())*0.5);
				osgevent->setY((ea.getYmin()+ea.getYmax())*0.5);
				if(view) pick(view,*osgevent);
			}
			return false;
		}
	default:
		return false;
	}
	//return false;
}

void PickHandler::pick(osgViewer::View *view, const osgGA::GUIEventAdapter &ea)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	//osgUtil::LineSegmentIntersector::Intersection;
	osgUtil::PolytopeIntersector::Intersections polyIntersections;

	std::string gdlist="";
	float x = ea.getX();
	float y = ea.getY();

	if (view->computeIntersections(x,y,intersections)) {
		for ( osgUtil::LineSegmentIntersector::Intersections::iterator hitr = 
			intersections.begin(); hitr!=intersections.end(); ++hitr) {
			
				std::ostringstream os;
				if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
				{
					os<<"Object \""<<hitr->nodePath.back()->getName()<<"\""<<std::endl;
				} else if ( hitr->drawable.valid()) {
					os<<"Object \""<<hitr->drawable->className()<<"\""<<std::endl;
				}

				os<<"    local coords vertex("<<hitr->getLocalIntersectPoint()<<")"<<"  normal("<<hitr->getLocalIntersectNormal()<<")"<<std::endl;
				os<<"    world coords vertex("<<hitr->getWorldIntersectPoint()<<")"<<"  normal("<<hitr->getWorldIntersectNormal()<<")"<<std::endl;

				const osgUtil::LineSegmentIntersector::Intersection::IndexList &vil = hitr->indexList;
				for ( unsigned int i=0; i<vil.size(); ++i ) {
					os<<"   vertex indices ["<<i<<"] = "<<vil[i]<<std::endl;
				}

				gdlist += os.str();
		}
		setLabel(gdlist);
	}
}
