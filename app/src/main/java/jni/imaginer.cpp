//
// Created by dejian.fei on 2016/9/8.
//
#include "ibmp.h"
#include "ipoint.h"
#include "imaginerUtils.h"
#include "CImaginer.h"
#include "android_ImgSdk_Imaginer.h"

int rgba2pixel(int R,int G,int B,int A) 
{
	return ((A << 24) | (R << 16) | (G << 8) | B);
}
void pixel2rgba(int pixel,int x ,int y,PIXELS& cpixel) 
{
	int A = (pixel & 0xFF000000) >> 24;
	int R = (pixel & 0x00FF0000) >> 16;
	int G = (pixel & 0x0000FF00) >> 8;
	int B = (pixel & 0x000000FF);
	cpixel.setRGB(R,G,B,A);
	int tmp = rgba2pixel(R,G,B,A);
	printf("rgba: %d\n",tmp);
}

void CImaginer::showPIXELS(PIXELS& cpixel)
{
	cpixel.show_PIXELS();
}
CImaginer* 	cimageObj = NULL;
ppPIXELS    cimageData = NULL;

JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_init(JNIEnv * env, jobject obj, jintArray imageData, jint width, jint height)
{
	cimageObj = new CImaginer(width,height);
	PIXELS cpixel;
	jint *cbuf = (env)->GetIntArrayElements(imageData,false);
	if (cbuf == NULL) {
		return false; /* exception occurred */
	}
	for (int x = 0; x < width; x++) 
	{
		for (int y = 0; y < height; y++) 
		{
			int tmp = cbuf[ x * height + y];
			pixel2rgba(tmp,x,y,cpixel);
			cimageObj->insert(cpixel);
			cimageObj->showPIXELS(cpixel);
		}
	}
	cimageData = cimageObj->getImageData();
	return true;
}

JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_isStartPoint(JNIEnv *, jobject)
{
	PIXELS tmp;
	int bmpHeight = cimageObj->getWidth();
	int bmpWidth = cimageObj->getHeight();
	for (int y = 0;y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if(cimageObj->getNextStartPoint(x,y))
			{
				tmp = cimageData[y][x];
				if(tmp.getEdge() >= 0)
				{
					if(!cimageObj->getBoundaryLine(x,y))
					{
						//printf("getBoundaryLine flase\n");
					}
				}
				else
				{
					//genSkipTable(cimageData[y][x]);
				}
			}
		}
	}
	return true;
}

JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_getNextPoint (JNIEnv *, jobject)
{
	return true;
}


JNIEXPORT jfloat JNICALL Java_android_ImgSdk_Imaginer_getSimilarity (JNIEnv *, jobject)
{
	return 0.0;
}
JNIEXPORT void JNICALL Java_android_ImgSdk_Imaginer_cfinalize (JNIEnv *, jobject)
{
//will be use free all memory at c	
}

