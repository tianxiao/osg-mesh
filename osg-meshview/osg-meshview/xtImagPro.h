#pragma once
#include <vector>

class xtImagPro
{
public:
	xtImagPro(void);
	~xtImagPro(void);
};

struct xtHexColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct xtBMPPicFormat
{
	char filename[20];
	std::vector<xtHexColor> colors;
	int width, height;
	int channels;
};

void DumImage( xtBMPPicFormat* img, char *filename);



