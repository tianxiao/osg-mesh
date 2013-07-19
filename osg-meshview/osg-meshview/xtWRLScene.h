#pragma once
namespace osg
{
	class Geode;
};

class xtWRLScene
{
public:
	xtWRLScene(void);
	~xtWRLScene(void);
};

struct xtWRLDataS;
osg::Geode *CreateGeo(xtWRLDataS *data) ;

	


