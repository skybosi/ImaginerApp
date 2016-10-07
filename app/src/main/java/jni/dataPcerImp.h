#ifndef DATAPCERIMP_H_
#define DATAPCERIMP_H_


#include <map>
#include <vector>
#include <deque>
#include <android/log.h>
#include "ipoint.h"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "IMAGINER", __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "IMAGINER", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "IMAGINER", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "IMAGINER", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "IMAGINER", __VA_ARGS__)

typedef deque<PIXELS>      dPIXELS;
typedef vector<dPIXELS>    vdPIXELS;
typedef deque<PIXELS>      dSkipLine;
typedef vector<dSkipLine>  SkipTable;
typedef vector<SkipTable>  vSkipTable;

class dataPcerImp
{
	private:
		ppPIXELS cimageData;
		int      cwidth;
		int      cheight;
	/*
	private:
		float    baseSmlrty;
		RGBQUAD  backGround;
		vdPIXELS boundarys; //record all boundary line
		int      granularity;
		bool     granOpeartor;//contrl the granularity opeartor method
				//granOpeartor: boundarys will save only largger than granularity value's boundaryline :true
				//              boundarys will save only smaller than granularity value's boundaryline :false
	*/
	public:
		int& getWidth(){return cwidth;}
		int& getHeight(){return cheight;}
		ppPIXELS& getImageData(){return cimageData;}
	public:
		dataPcerImp(int width,int height):cwidth(width),cheight(height)//,baseSmlrty(0),granularity(10),granOpeartor(false)
		{
			newImageData(cimageData,cwidth,cheight);
		}
		~dataPcerImp()
		{
			delImageData(cimageData,cheight);
		}
		void     showPIXELS(const PIXELS& cpixel);
		bool     insert(int x,int y,int pixel);
	private:
		int      rgba2pixel(int R,int G,int B,int A);
		void     pixel2rgba(int pixel,int x ,int y,PIXELS& cpixel);
		ppPIXELS newImageData(ppPIXELS &imageData, int W, int H);
		bool     delImageData(ppPIXELS& imageData, int H);
};




#endif //DATAPCERIMP_H_