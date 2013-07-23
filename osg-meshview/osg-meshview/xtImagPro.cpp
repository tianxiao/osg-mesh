#include "StdAfx.h"
#include "xtImagPro.h"
#include <stdlib.h>
#include <stdio.h>


xtImagPro::xtImagPro(void)
{
}


xtImagPro::~xtImagPro(void)
{
}

void DumImage( xtBMPPicFormat* img, char *filename )
{
	FILE *fp = fopen(filename, "w");

	fprintf(fp,"P3\n");
	fprintf(fp,"%d %d\n",img->width,img->height);
	fprintf(fp,"%d\n",15);

	int num=0;

	for ( int h=0; h<img->height; ++h ) {
		for ( int w=0; w<img->width; ++w ) {
			
			fprintf(fp,"%d %d %d\n", 
				img->colors[num].r,
				img->colors[num].g, 
				img->colors[num].b
				);
			num++;

		}
	}

	fclose(fp);
}
