/*
 * Data Provider >> Dper  (D is the most important)
 * get DATA from from diffrent image type to generate Data source :
 * jus like .bmp,.jpeg(.jpg),gif......
 * The Data Provider will provider such Data:
 *     1.ppPIXLES imageData
 *     2.int imageWidth
 *     3.int imageHeight
 *     4.Opearator Command
 * Dper should have image's path and output path
 *
 */

#ifndef _DPER_H_
#define _DPER_H_

#include "ipoint.h"

typedef struct RGBAXY
{
    int x;
    int y;
    int rgba;
}RGBA;
class DperMum
{
protected:
    string    _imagePath;
    string    _omagePath;
    //char*   OpertCmd;
    //all data
    int       _width;
    int       _height;
    ppPIXELS  _Data;
    //part of data
    int       dp_beginX;
    int       dp_beginY;
    int       dp_width;
    int       dp_height;
    ppPIXELS  dp_Data;
public:
    DperMum(const DperMum& dp){
        _imagePath = dp._imagePath;_omagePath = dp._omagePath ;
        _width = dp._width;_height = dp._height;_Data = dp._Data;
        dp_beginX = dp.dp_beginX; dp_beginY =dp.dp_beginY;
        dp_width = dp.dp_width;dp_height = dp.dp_height;dp_Data = dp.dp_Data;
    }
    DperMum(const char *imagepath):_imagePath(imagepath),_omagePath(""),
                                   _width(0),_height(0),_Data(NULL),
                                   dp_beginX(0),dp_beginY(0),
                                   dp_width(0),dp_height(0),dp_Data(NULL){}
    virtual ~DperMum(){}
    //processing request
    virtual bool ProcesReq(const char* dealType = NULL) = 0;
public:
    virtual ppPIXELS&   getData(){return dp_Data;}
    virtual int&        getWidth(){return dp_width;}
    virtual int&        getHeight(){return dp_height;}
    virtual void        setData(ppPIXELS data){dp_Data = data;}
    virtual void        setWidth(int width){dp_width = width;}
    virtual void        setHeight(int height){dp_height = height;}
public:
    //Init when open or read a image
    virtual bool  initData() = 0;
    virtual bool  read(int width = 0,int height = 0,int beginX = 0,int beginY = 0) = 0;
public:
    //Write or Create the new image message into a file,
    //the data is from  dp_Data;
    //Get a new image
    //@ outpath : detail output path
    //@ isOverWrite : true -> will over write the read part of origin After deal with
    //                false-> will crete a new image to save the dealed with [ default isOverWrite = false ]
    virtual bool write(const char* outpath,bool isOverWrite = false) = 0;
    virtual bool write(const char* outpath,ppPIXELS data,int width,int height) = 0;
private://utils
    PIXELS rgba2pixel(int x,int y,int ipixel) {
        PIXELS opixel;
        opixel.setXY(x,y);
        RGBQUAD rgba;
        rgba.rgbReserved = (ipixel & 0xFF000000) >> 24;
        rgba.rgbRed      = (ipixel & 0x00FF0000) >> 16;;
        rgba.rgbGreen    = (ipixel & 0x0000FF00) >> 8;;
        rgba.rgbBlue     = (ipixel & 0x000000FF);;
        return opixel.setRGB(rgba);
    }
    RGBA pixel2rgba(PIXELS& pixel) {
        RGBA tmprgba;
        tmprgba.x = pixel.getX();
        tmprgba.y = pixel.getY();
        int R = pixel.getRed();
        int G = pixel.getGreen();
        int B = pixel.getBlue();
        int A = 255;
        tmprgba.rgba = ((A << 24) | (R << 16) | (G << 8) | B);
        return tmprgba;
    }
};

#endif // Dper.h :[]

