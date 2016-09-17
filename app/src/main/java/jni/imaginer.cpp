//
// Created by dejian.fei on 2016/9/8.
//
#include<exception> 
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
Position prevDiret = Right;

JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_init(JNIEnv * env, jobject obj, jintArray imageData, jint width, jint height)
{
	LOGD("imageData lenth:%d\tWidth:%d\t Height:%d\n",env->GetArrayLength(imageData), width,height);
	cimageObj = new CImaginer(width,height);
	PIXELS cpixel;
	jint *cbuf = env->GetIntArrayElements(imageData,NULL);
	if (cbuf == NULL) {
		LOGE("GetIntArrayElements in imageData error\n");
		return false; /* exception occurred */
	}
	try
	{
		for(int y = 0;y < height;++y)
		{
			for(int x = 0;x < width;++x)
			{
				int tmp = cbuf[ y * width + x];
				pixel2rgba(tmp,x,y,cpixel);
				cimageObj->insert(cpixel);
			}
		}
	}catch (exception& e)
	{
		LOGE("Exception occur:%s\n",e.what());
	}
	cimageData = cimageObj->getImageData();
	env->ReleaseIntArrayElements(imageData,cbuf,0);
	LOGD("init image is OOOOKKKKK!\n");
	return true;
}

JNIEXPORT jintArray JNICALL Java_android_ImgSdk_Imaginer_isStartPoint (JNIEnv *env, jobject obj, jint curx, jint cury)
{
	PIXELS tmp;
	int buffer[2];//save start point x,y 
	jintArray iarray = NULL;
	if(cimageObj == NULL || cimageData == NULL)
	{
		LOGE("image init not OK!!!\n");
		return NULL;
	}
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
				tmp = cimageData[y][x];
				if(tmp.getEdge() >= 0)
				{
					LOGD("oookkk!!! Get startX:%d\t startY:%d\n",x,y);
					buffer[0] = x;
					buffer[1] = y;
					iarray = (env)->NewIntArray(2);
					(env)->SetIntArrayRegion(iarray,0,2,buffer);
					goto START;
				}
				else
				{
					LOGD("X:%d\tY:%d\tEdge flags:%d\n",x,y,tmp.getEdge());
				}
			}
		}
	}
	START: LOGD("get a start point\n");
	return iarray;
}

JNIEXPORT jintArray JNICALL Java_android_ImgSdk_Imaginer_getNextPoint(JNIEnv *env, jobject obj, jint curX, jint curY)
{
	int buffer[3];//save next point x,y and flags 
	jintArray iarray = NULL;
	//get next point
	PIXELS& prevPoint = cimageData[curY][curX];
	Position direction = prevDiret;
	LOGD("before cutX:%d\t cutY:%d\tPostion:%d\n",curX,curY,direction);
	if(cimageObj->getRpoint(direction,curX,curY))
	{
		LOGD("After cutX:%d\t cutY:%d\tdirection:%d\tprevDiret:%d\n",curX,curY,direction,prevDiret);
        if(prevDiret == direction)
        {
            if(ISV(prevDiret))
            {
                prevPoint.setEdge(-1);
            }
            else
            {
                prevPoint.setEdge(-2);
            }
            LOGD("After == direction:%d\tprevDiret:%d\tEdge:%d\n",direction,prevDiret,prevPoint.getEdge());
        }
        else//direction is change
        {
            int direSum = prevDiret + direction;
            switch(direSum)
            {
                case 3://reset direction
                    prevDiret = direction;
                    if(ISV(direction))
                    {
                        prevPoint.setEdge(-1);
                    }
                    else
                    {
                        prevPoint.setEdge(-2);
                    }
                    break;
                case 2:
                case 4:
                    if(ISV(direction))
                    {
                        prevPoint.setEdge(-2);
                    }
                    else
                    {
                        prevPoint.setEdge(-1);
                    }
                    break;
                case 5://LR or RL
                    prevPoint.setEdge(-1);
                    if(prevDiret > direction)
                        prevPoint.setpPos(Up);
                    else
                        prevPoint.setpPos(Down);
                    break;
                default://case 1:
                    prevPoint.setEdge(-1);
                    break;
            }
            LOGD("After != direction:%d\tprevDiret:%d\tEdge:%d\n",direction,prevDiret,prevPoint.getEdge());
        }
        buffer[0] = curX;
        buffer[1] = curY;
        buffer[2] = prevPoint.getEdge();
		LOGD("will return buffer:%d %d %d\n", buffer[0], buffer[1], buffer[2]);
		prevDiret = direction;
		iarray = (env)->NewIntArray(3);
		(env)->SetIntArrayRegion(iarray,0,3,buffer);
		LOGD("????????????????????????????\n");
	}
	return iarray;
}


JNIEXPORT void JNICALL Java_android_ImgSdk_Imaginer_cfinalize (JNIEnv *env, jobject obj)
{
//will be use free all memory at c
/*
	if(cimageObj != NULL)
	{
		delete cimageObj;
		cimageObj = NULL;
		cimageData = NULL;
		prevDiret = Right;
	}
	*/
}

