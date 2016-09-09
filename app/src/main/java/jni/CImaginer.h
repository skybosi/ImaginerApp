#include "imaginerUtils.h"
#include "ipoint.h"
class CImaginer
{
	private:
		ppPIXELS cimageData;
		int      cwidth;
		int      cheight;
	private:
		float    baseSmlrty;
    RGBQUAD  backGround;
		vdPIXELS boundarys; //record all boundary line
		int      granularity;
		bool     granOpeartor;//contrl the granularity opeartor method
				//granOpeartor: boundarys will save only largger than granularity value's boundaryline :true
				//              boundarys will save only smaller than granularity value's boundaryline :false
	public:
		CImaginer():cwidth(0),cheight(0){}
		CImaginer(int width,int height):cwidth(width),cheight(height),granularity(10),
		                                baseSmlrty(0),granOpeartor(false)
	{
		cimageData = newImageData(cimageData,cwidth,cheight);
	}
		~CImaginer()
		{
			delImageData(cimageData,cheight);
		}
		void     showPIXELS(PIXELS& cpixel);
		bool     insert(const PIXELS& cpixel);
	public:
		ppPIXELS getImageData(){return cimageData;}
		int      getWidth(){return cwidth;}
		int      getHeight(){return cheight;}
		bool     getNextStartPoint(int& x,int& y){ return isBoundaryPoint(x,y);}
		bool     getBoundaryLine(int& x,int& y);
		//just want to get the right point of one point,if get x,y will be reset
		bool     getRpoint(Position& direction,int& x,int& y);
		//just want to get the left point of one point,if get x,y will be reset
		bool     getLpoint(Position& direction,int& x,int& y);
	private:
		ppPIXELS newImageData(ppPIXELS &imageData, int W, int H);
		bool     delImageData(ppPIXELS& imageData, int H);
	private:
		bool     isBoundaryPoint(int& x,int& y);
		float    getSimilarity(PIXELS backPoint, PIXELS currPoint);
		float    getSimilarity(Position direction,int x,int y,int step = 1);
		PIXELS   get_pix(int x, int y);
		PIXELS   get_pix(PIXELS pixel);
		bool     out_range_error(const PIXELS& pixel);
		//set BackGround with RGBQUAD
		bool     setBackground(RGBQUAD rgb);
		//set BackGround with R,G,B
		bool     setBackground(U8 r = 255,U8 g = 255,U8 b = 255);
		//set BackGround with PIXELS
		bool     setBackground(const PIXELS& pixel);
		int      trackDown(PIXELS& startPoint);
		//alike background or not,Mean and the same color as 
		//the background color 
		//NOTE: But not necessarily background 
		//return code:
		//   -1 is empty pixel, 1 alike background,0 not alike background
		int      alikeBackground(PIXELS pixel);
		//alike background or not,Mean and the same color as 
		//the background color 
		//NOTE: But not necessarily background
		//return code:
		//   -1 is empty pixel, 1 alike background,0 not alike background
		int      alikeBackground(int x,int y);
		//alike background or not,Mean and the same color as 
		//the background color 
		//NOTE: But not necessarily background
		bool     alikeBackground(RGBQUAD rgb);
		//Test whether around the start point has been visited
		//bool testStartP(PIXELS pixel,int range = 2);
		bool     testStartP(PIXELS& pixel);
		bool     isEdge(int x,int y);
};
