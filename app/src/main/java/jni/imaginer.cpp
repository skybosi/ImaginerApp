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
	cpixel.setXY(x,y);
	int tmp = rgba2pixel(R,G,B,A);
	printf("rgba: %d\n",tmp);
}

CImaginer* 	cimageObj = NULL;
ppPIXELS    cimageData = NULL;
int startX = 0;
int startY = 0;
Position nextPos = Right;

JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_init(JNIEnv * env, jobject obj, jintArray imageData, jint width, jint height)
{
	LOGD("imageData lenth:%d\tWidth:%d\t Height:%d\n",env->GetArrayLength(imageData), width,height);
	cimageObj = new CImaginer(width,height);
	PIXELS cpixel;
	jint *cbuf = (env)->GetIntArrayElements(imageData,false);
	if (cbuf == NULL) {
		LOGE("GetIntArrayElements in imageData error\n");
		return false; /* exception occurred */
	}
	for(int y = 0;y < height;++y)
	{
		for(int x = 0;x < width;++x)
		{
			int tmp = cbuf[ y * width + x];
			pixel2rgba(tmp,x,y,cpixel);
			cimageObj->insert(cpixel);
			cimageObj->showPIXELS(cpixel);
			//if(x == 15)
			//goto here;
		}
	}
	//here: LOGD("get 1 hang\n");
	cimageData = cimageObj->getImageData();
	return true;
}

JNIEXPORT jintArray JNICALL Java_android_ImgSdk_Imaginer_isStartPoint (JNIEnv *env, jobject obj, jint curx, jint cury)
{
	PIXELS tmp;
	int buffer[2];//save start point x,y 
	jintArray iarray = NULL;
	int bmpHeight = cimageObj->getHeight();
	int bmpWidth = cimageObj->getWidth();
	LOGD("bmpWidth:%d\t bmpHeight:%d\tcurX:%d\tcurY:%d\n",bmpWidth,bmpHeight,curx,cury);
	for (int y = cury;y < bmpHeight; ++y)
	{
		for (int x = curx; x < bmpWidth; ++x)
		{
			if(cimageObj->getNextStartPoint(x,y))
			{
				LOGD("Get startX:%d\t startY:%d will see Edge flags\n",x,y);
				tmp = cimageData[x][y];
				if(tmp.getEdge() >= 0)
				{
					LOGD("oookkk!!! Get startX:%d\t startY:%d\n",x,y);
					buffer[0] = x;
					startX = x;
					buffer[1] = y;
					startY = y;
					iarray = (env)->NewIntArray(2);
					(env)->SetIntArrayRegion(iarray,0,2,buffer);
					goto START;
					//break;
					/*
					if(cimageObj->getBoundaryLine(x,y))
					{
					}
					else
					{
						LOGD("getBoundaryLine flase\n");
						//printf("getBoundaryLine flase\n");
					}
					*/
				}
				else
				{
					LOGD("Edge flags:%d\n",tmp.getEdge());
					//genSkipTable(cimageData[y][x]);
				}
			}
		}
	}
	START: LOGD("get 1 hang\n");
	return iarray;
}

JNIEXPORT jintArray JNICALL Java_android_ImgSdk_Imaginer_getNextPoint(JNIEnv *env, jobject obj, jint curX, jint curY)
{
	int buffer[3];//save next point x,y and flags 
	jintArray iarray = NULL;
	//get next point
	PIXELS pixels = cimageData[curX][curY];
	Position pos = nextPos;
	LOGD("before cutX:%d\t cutY:%d\tPostion:%d\n",curX,curY,pos);
	if(cimageObj->getRpoint(pos,curX,curY))
	{
		LOGD("After cutX:%d\t cutY:%d\tPostion:%d\n",curX,curY,pos);
		buffer[0] = curX;
		buffer[1] = curY;
		buffer[2] = pos;
		nextPos = pos;
		iarray = (env)->NewIntArray(3);
		(env)->SetIntArrayRegion(iarray,0,3,buffer);
	}
	return iarray;
}


JNIEXPORT void JNICALL Java_android_ImgSdk_Imaginer_cfinalize (JNIEnv *, jobject)
{
//will be use free all memory at c	
}

