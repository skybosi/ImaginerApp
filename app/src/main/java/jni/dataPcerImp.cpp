#include "dataPcerImp.h"

ppPIXELS dataPcerImp::newImageData(ppPIXELS &imageData, int W, int H)
{
	// malloc some memroy
	// imageData = new pPIXELS[W];
	imageData = (ppPIXELS)malloc(H*sizeof(pPIXELS));
	if(!imageData)
	{
		LOGE("In %s new imageData wrong!\n",__FUNCTION__);
	}
	LOGD("new:(W,H):%d,%d\n",W,H);
	int y;
	for (y = 0; y < H; y++)
	{
		// imageData[y] = new PIXELS[W];
		imageData[y] = (pPIXELS)malloc(W*sizeof(PIXELS));
		if (!imageData[y])
			LOGE("In %s new imageData[y] wrong!\n",__FUNCTION__);
	}
	return imageData;
}

bool dataPcerImp::delImageData(ppPIXELS& imageData, int H)
{
	//printf("H:%d %p\n",H,imageData);
	if (imageData)
	{
		for (int i = 0; i < H; i++)
		{
			// delete []imageData[i];
			free(imageData[i]);
		}
		// delete []imageData;
		free(imageData);
		LOGD("In %s free imageData memory\n",__FUNCTION__);
		imageData = NULL;
	}
	else
	{
		printf("In delImageData imageData = NULL\n");
	}
	return true;
}

bool dataPcerImp::insert(int x,int y,int pixel)
{
	PIXELS cpixel;
	pixel2rgba(pixel,x,y,cpixel);
	//LOGD("insert: \t");
	//showPIXELS(cpixel);
	cimageData[y][x] = cpixel;
	return true;
}

int dataPcerImp::rgba2pixel(int R,int G,int B,int A) 
{
	return ((A << 24) | (R << 16) | (G << 8) | B);
}

void dataPcerImp::pixel2rgba(int pixel,int x ,int y,PIXELS& cpixel)
{
	int A = (pixel & 0xFF000000) >> 24;
	int R = (pixel & 0x00FF0000) >> 16;
	int G = (pixel & 0x0000FF00) >> 8;
	int B = (pixel & 0x000000FF);
	cpixel.setRGB(R,G,B,A);
	cpixel.setXY(x,y);
	int tmp = rgba2pixel(R,G,B,A);
	printf("rgba: %d\n",tmp);
}
