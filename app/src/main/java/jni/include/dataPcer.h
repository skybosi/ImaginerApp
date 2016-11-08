/*
 * Data Processing Center >> DPC  (P is the most important)
 * deal with Data source from diffrent image type:
 * jus like .bmp,.jpeg(.jpg),gif......
 * The Data Souroce must Provider such Data:
 *     1.ppPIXLES imageData
 *     2.int imageWidth
 *     3.int imageHeight
 *     4.Opearator Command
 * will return the dealed imageData,and imageWidth,imageHeight
 */
 
#ifndef _DPC_H_
#define _DPC_H_
#include <vector>
#include <deque>
#include <map>
#include <stdlib.h>
#include <math.h>
#include "ipoint.h"
#include "DperMum.h"

//frame of axes: Down and Right is positive direction
enum FofA//frame of axes
{
    RU, //第一象限
    LU, //第二象限
    LD, //第三象限
    RD, //第四象限
    R , //x正半轴
    U , //y正半轴
    L , //x负半轴
    D , //y负半轴
    O , //原点
    NO
};

struct mvect //Move Vector
{
private:
    FofA getPosition()
    {
        if(_x & _y)
        {
            if((_x ^_y) >= 0) //同号
            {
                if(_x > 0/*&& _y >0*/)
                    return RD;
                else
                    return LU;
            }
            else//异号
            {
                if(_x  > _y)
                    return RU;
                else
                    return LD;
            }
        }
        else
        {
            if(_x == _y) return O;
            if(_x > 0) return R;
            if(_x < 0) return L;
            if(_y < 0) return U;  //frame of axes: Down and Right is positive direction
            if(_y > 0) return D;

        }
        return NO;
    }
public:
    bool isLineMove(){return !(_x & _y);}
    mvect():_x(0),_y(0){
        _fofa = NO;
    }
    mvect(int x,int y):_x(x),_y(y){
        _fofa = getPosition();
    }
    mvect(const mvect& rh)
    {
        _x = rh._x;
        _y = rh._y;
        _fofa = rh._fofa;
    }
    mvect set(int x,int y){_x = x;_y = y;_fofa = getPosition();}
    mvect reset(int xORy,bool VorH = true){
        mvect ret(*this);
        int meta_x = _x/abs(_x);
        int meta_y = _y/abs(_y);
        xORy = abs(xORy);
        if(VorH)//V(y)
        {
            ret._x = meta_x*abs((xORy*_x)/_y);
            ret._y = meta_y*abs(xORy);
        }
        else//H(x)
        {
            ret._y = meta_x*abs((xORy*_y)/_x);
            ret._x = meta_y*abs(xORy);
        }
        return ret;
    }
    mvect operator+(mvect& rhs){
        _x += rhs._x;
        _y += rhs._y;
        return *this;
    }
    float getDistence()
    {
        return sqrt(_x*_x + _y*_y);
    }
public:
    int _x;
    int _y;
    FofA _fofa;
};

struct limitXXY
{
    limitXXY():sttx(0),endx(0),ally(0){}
    limitXXY(const limitXXY& rhs){
        sttx = rhs.sttx;
        endx = rhs.endx;
        ally = rhs.ally;
    }
    bool change(){return (sttx != endx); }
    bool add(PIXELS begin,PIXELS end,vector<limitXXY>& vtrackLine)
    {
        if(begin.getY() != end.getY())
            return false;
        sttx = begin.getX();
        endx = end.getX();
        ally = begin.getY();
        vtrackLine.push_back(*this);
        return true;
    }
    void show()
    {
        printf("ally:%d\tsttx:%d\tendx:%d\n",ally,sttx,endx);
    }
    bool in(int x)
    {
        return x >= sttx && x < endx;
    }
    bool lout(int x)
    {
        return x < sttx;
    }
    bool rout(int x)
    {
        return x > endx;
    }
    limitXXY operator=(const limitXXY& rhs){
        sttx = rhs.sttx;
        endx = rhs.endx;
        ally = rhs.ally;
        return *this;
    }
    limitXXY operator+(const mvect& mv){
        limitXXY rt(*this);
        rt.sttx += mv._x;
        rt.endx += mv._x;
        rt.ally += mv._y;
        return rt;
    }
    int sttx; //start point x
    int endx; //end point x
    int ally; //communal y
};
typedef vector<limitXXY>   vTrackLine;
typedef vector<vTrackLine> TrackTable;
typedef vector<TrackTable> vTrackTable;

struct FramePoint
{
    FramePoint():FramePoint(-1,-1){}
    FramePoint(int H,int W):bindNum(-1)
    {
        framePoint[0] = H;
        framePoint[1] = 0;
        framePoint[2] = W;
        framePoint[3] = 0;
    }
    FramePoint(const FramePoint& rh)
    {
        bindNum = rh.bindNum;
        framePoint[0] = rh.framePoint[0];
        framePoint[1] = rh.framePoint[1];
        framePoint[2] = rh.framePoint[2];
        framePoint[3] = rh.framePoint[3];
    }
    bool setframePoint(Position index,const PIXELS& xORy)
    {
        if(index < Up || index > Right)
            return false;
        switch(index)
        {
            case Up:
                framePoint[Up] = std::min(framePoint[Up],xORy.getY());
                break;
            case Down:
                framePoint[Down] = std::max(framePoint[Down],xORy.getY());
                break;
            case Left:
                framePoint[Left] = std::min(framePoint[Left],xORy.getX());
                break;
            case Right:
                framePoint[Right] = std::max(framePoint[Right],xORy.getX());
                break;
            default:
                break;
        }
        return true;
    }
    int operator[](int index)const
    {
        if(index < Up || index > Right)
            return -1;
        else
            return framePoint[index];
    }
    //NOTE: this bindNum is first set,then readonly
    //It's use to bind a boundarys,even though the boundarys is move
    void setBindNum(int num){if(bindNum == -1)bindNum = num;}
    int getBindNum()const{ return bindNum;}
    FramePoint operator+(const mvect& mv){
        FramePoint rt(*this);
        rt.framePoint[0] += mv._y;
        rt.framePoint[1] += mv._y ;
        rt.framePoint[2] += mv._x;
        rt.framePoint[3] += mv._x;
        return rt;
    }
    FramePoint operator+(const int& mvXorY){
        FramePoint rt(*this);
        rt.framePoint[0] += mvXorY;
        rt.framePoint[1] += mvXorY;
        rt.framePoint[2] += mvXorY;
        rt.framePoint[3] += mvXorY;
        return rt;
    }
    FramePoint& operator+=(const mvect& mv){
        framePoint[0] += mv._y;
        framePoint[1] += mv._y ;
        framePoint[2] += mv._x;
        framePoint[3] += mv._x;
        return *this;
    }
    const FramePoint& operator=(const FramePoint& rhs){
        framePoint[0] = rhs[0];
        framePoint[1] = rhs[1];
        framePoint[2] = rhs[2];
        framePoint[3] = rhs[3];
        bindNum       = rhs.bindNum;
        return *this;
    }
    bool operator>(const FramePoint& rhs){
        if(framePoint[Up] > rhs[Up])
            return true;
        else if(framePoint[Up] == rhs[Up] && framePoint[Left] > rhs[Left])
            return true;
        else return false;
    }
    bool operator<(const FramePoint& rhs){
        if(framePoint[Up] < rhs[Up])
            return true;
        else if(framePoint[Up] == rhs[Up] && framePoint[Left] < rhs[Left])
            return true;
        else return false;
    }
    bool operator==(const FramePoint& rhs){
        return  framePoint[0] == rhs[0] &&
                framePoint[1] == rhs[1] &&
                framePoint[2] == rhs[2] &&
                framePoint[3] == rhs[3];
    }
    bool operator!=(const FramePoint& rhs){
        return  framePoint[0] != rhs[0] ||
                framePoint[1] != rhs[1] ||
                framePoint[2] != rhs[2] ||
                framePoint[3] != rhs[3];
    }
    bool checkPosition(mvect where,const FramePoint& Curr)
    {
        int x = where._x;
        int y = where._y;
        int CurrX = 0;
        int CurrY = 0;
        //由于移动的方向确定了，以被移动的一个角点为原点，来判断被检测碰撞的框子在移动的向量的那一
        //侧与该角点坐标被忽略(其实已经被减去)，及可直接由移动向量的坐标(x,y),具体原理详见（《关于边界移动时碰撞的原理讨论》）
        switch(where._fofa)
        {
        case RU://(Curr[Left]-this[Right],Curr[Down]-this[Up]) * (x,y)
            //return (((Curr[Left]-framePoint[Right])*y-
            //         (Curr[Down]-framePoint[Up])*x) >= 0);
            CurrX = Curr[Left]-framePoint[Right];
            CurrY = Curr[Down]-framePoint[Up];
            break;
        case LU://(Curr[Right]-this[Left],Curr[Down]-this[Up]) * (x,y)
            //return (((Curr[Right]-framePoint[Left])*y+
            //        (Curr[Down]-framePoint[Up])*x) >=0);
            CurrX = Curr[Right]-framePoint[Left];
            CurrY = Curr[Down]-framePoint[Up];
            break;
        case LD://(Curr[Right]-this[Left],Curr[Up]-this[Down]) * (x,y)
            //return (((Curr[Right]-framePoint[Left])*y-
            //         (Curr[Up]-framePoint[Down])*x)>=0);
            CurrX = Curr[Right]-framePoint[Left];
            CurrY = Curr[Up]-framePoint[Down];
            break;
        case RD://(Curr[Left]-this[Right],Curr[Up]-this[Down]) * (x,y)
            //return (((Curr[Left]-framePoint[Right])*y+
            //         (Curr[Up]-framePoint[Down])*x) >=0);
            CurrX = Curr[Left]-framePoint[Right];
            CurrY = Curr[Up]-framePoint[Down];
            break;
        default:
            return false;
            break;
        }
        return ((CurrX*y-CurrY*x)<=0);
    }
    bool in(int x,int y){ return ((x >= framePoint[Left] && x <= framePoint[Right]) &&
                                  (y >= framePoint[Up]   && y <= framePoint[Down])); }
    //a line is cross the FramePoint or not:line in
    bool lin(int xORy,int index){ return ((index > Down) ?
                                  (xORy >= framePoint[Left] && xORy <= framePoint[Right]) :
                                  (xORy >= framePoint[Up]   && xORy <= framePoint[Down])); }
    //check this FramePoint whether out of image after move
    FofA out(const int& width,const int& height){
        if(framePoint[Up] < 0)
        {
            if(framePoint[Left] < 0)
            {
                return LU;
            }
            return U;
        }
        if(framePoint[Left] < 0)
        {
            if(framePoint[Down] > height-1)
            {
                return LD;
            }
            return L;
        }
        if(framePoint[Down] > height-1)
        {
            if(framePoint[Right] > width-1)
            {
                return RD;
            }
            return D;
        }
        if(framePoint[Right] > width-1)
        {
            if(framePoint[Up] < 0)
            {
                return RU;
            }
            return R;
        }
        return NO;
    }
    void checkout(mvect& where,int width,int height)
    {
        FofA fofa = (*this+where).out(width,height);
        switch(fofa)
        {
        case U:
            where._y = -framePoint[Up];
            break;
        case D:
            where._y = height-1-framePoint[Down];
            break;
        case L:
            where._x = -framePoint[Left] ;
            break;
        case R:
            where._x = width-1-framePoint[Right] ;
            break;
        case RU:
            where._x = width-1-framePoint[Right] ;
            where._y = -framePoint[Up] ;
            break;
        case LU:
            where._x = -framePoint[Left] ;
            where._y = -framePoint[Up] ;
            break;
        case LD:
            where._x = -framePoint[Left] ;
            where._y = height-1-framePoint[Down];
            break;
        case RD:
            where._x = width-1-framePoint[Right] ;
            where._y = height-1-framePoint[Down];
            break;
        default:
            break;
        }
    }
    void getCenter(int& x,int& y)
    {
        x = (framePoint[Right] + framePoint[Left]) >> 1;
        y = (framePoint[Down] + framePoint[Up]) >> 1;
    }
    private:
    int framePoint[4];
    int bindNum;
};

enum Method
{
    UD,
    LR,
    UR,
    NONE
};

//this position is first relation to second,but NOTE IN2
enum PRs// position relation
{
    IN,   //first in the second
    IN2,  //second in the first NOTE!
    CROSS,//include inscribe
    HERE, //Circumscribe
    NEAR, //>0 && <5 //5 is a example,but it's a small number
    OUT,
    EVER
};

typedef deque<FramePoint>  Frames;
typedef deque<PIXELS>      dPIXELS;
typedef vector<dPIXELS>    vdPIXELS;
typedef deque<PIXELS>      dSkipLine;
typedef vector<dSkipLine>  SkipTable;
typedef vector<SkipTable>  vSkipTable;

#define CLOSEOPEN(var)       ((var) ? ("close") : ("open"))
/*
#define DIRECTION(var)       switch(var) {\
                               case Right: direction = Right; x++; break;\
                               case Down:  direction = Down;  y++; break;\
                               case Left:  direction = Left;  x--; break;\
                               case Up:    direction = Up;    y--; break;\
                               default:    break; }
                                                             */
#define DIRECTION_Up       {direction = Up;    y--;}
#define DIRECTION_Down     {direction = Down;  y++;}
#define DIRECTION_Right    {direction = Right; x++;}
#define DIRECTION_Left     {direction = Left;  x--;}
#define ISV(_var) (_var <= Down)   //vertically
#define ISH(_var) (_var >= Left)//  horizontally
#define SETSKIP(_pot)     (ISV(_pot.getpPos().first) ? (_pot.setEdge(-3)) : (_pot.setEdge(-1)))
#define SETPREV(_v,_prev) (_prev.setpPos(_v),(ISV(_v) ? SETSKIP(_prev) : _prev.setEdge(-2)))
#define SETCURR(_v,_curr) (_curr.setpPos(_v),(ISV(_v) ? SETSKIP(_curr) : _curr.setEdge(-2)))

/*
#define SETPREV(_v,_prev) (_prev.setpPos(_v),(ISV(_v) ? _prev.setEdge(-1) : _prev.setEdge(-2)))
#define SETCURR(_v,_curr) (_curr.setpPos(_v),(ISV(_v) ? _curr.setEdge(-1) : _curr.setEdge(-2)))
*/
#define TMAX(_v1,_v2,_v3) (_v1 > _v2) ? ((_v1 > _v3) ? (_v1) : (_v3)) : ((_v2 > _v3) ? (_v2) : (_v3))
#define TMIN(_v1,_v2,_v3) (_v1 < _v2) ? ((_v1 < _v3) ? (_v1) : (_v3)) : ((_v2 < _v3) ? (_v2) : (_v3))

class dataPcer
{
private:
    DperMum*  _dp;
    ppPIXELS  _Data;
    int       _width;
    int       _height;
    char*     _OpertCmd;
    int       _beginX;
    int       _beginY;
private:
    RGBQUAD     backGround;
    vdPIXELS    boundarys; //record all boundary line
    vSkipTable  skipTables;
    vTrackTable trackTables;//用于抠出轨迹内的部分
    //record the trackdown's result,just for cutout the image
    Frames      frames;    //record all boundaryline's frame Point
    U32         granularity;//图像碎片边缘最少像素
    bool        granOpeartor;//contrl the granularity opeartor method
    //granOpeartor: boundarys will save only largger than granularity value's boundaryline :true
    //              boundarys will save only smaller than granularity value's boundaryline :false
    float       baseSmlrty;//base Similarity,use to judge is boundary point or not
    U8          testRange;//for use to set the test Range,that can sure a Edge Point is not trackdown again

public:
    dataPcer(ppPIXELS& data,int& width,int& height):_dp(NULL),
        _Data(data),_width(width),_height(height),_OpertCmd(NULL),
        granularity(10),granOpeartor(true),baseSmlrty(1.0),testRange(2){
        _beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
    }
    dataPcer(DperMum* dp):_dp(dp),granularity(10),granOpeartor(true),baseSmlrty(1.0),testRange(2){
        //_Data = _dp->getData();_width = _dp->getWidth();_height = _dp->getHeight();
        //_beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
        initData(_dp->getData(),_dp->getWidth(),_dp->getHeight());
    }
    ~dataPcer(){printf("out the dataPcer\n");}
    bool initData(ppPIXELS data,int width,int height);
    bool dealManager(const char* dealType);
    vdPIXELS getBoundrysData(){return boundarys;} 
    int autoMove(int x,int y,int mx,int my);

public://return all Data
    int      retnWidth();
    int      retnHeight();
    ppPIXELS retnData();

private://common function
    //Function: generate the image's bar diagram
    //@ imageData: will dealwith data source
    //@ color    : chose a color,default Pricolor
    bool     genBardiagram(ppPIXELS& allData,colorType color = Pricolor,bool isOverWrite = true);
    //Function: generate the image's Histogram
    //@ allData  : will dealwith data source
    //@ color    : chose a color,default Pricolor
    bool     genHistogram(ppPIXELS& allData,colorType color = Pricolor,bool isOverWrite = true);
    //Function: Move the image
    // move the image:x>0,y>0 ->right down ;x<0 y<0 -> left up
    //@ allData   : will dealwith data source
    //@ mx        : The distance at x direction move
    //@ my        : The distance at y direction move
    ppPIXELS move(ppPIXELS& allData,int mx = 0,int my = 0);
    //Function: Mirror the image
    //@ allData   : will dealwith data source
    //@ method    : Mirror method (see enum Method)
    ppPIXELS mirror(ppPIXELS& allData,Method method = NONE);
    //Function: get 3 Color(RGB)
    //@ allData   : will dealwith data source
    //@ color     : The color (see enum colorType)
    ppPIXELS getImage3Color(ppPIXELS& allData,colorType color = Pricolor);
    //Function: Zoom the image
    //@ allData   : will dealwith data source
    //@ scalex    : The zoom rate at x direction move
    //@ scaley    : The zoom rate at y direction move
    ppPIXELS zoom(ppPIXELS& allData,float scalex = 1.0,float scaley = 1.0);
    //Function: Spherize the image
    //@ radius    : Spherize's radius
    //@ allData   : will dealwith data source
    //NOTE: if radius = 0,will Adhered with imageWidth and bmpWidth (oval)
    //else will Adhered with a circle of radius
    ppPIXELS spherize(ppPIXELS& allData,float radius = 0.0);
    //Function: Transpose the image
    //@ allData   : will dealwith data source
    //@ AR        : Antegrade(TRUE) and retrograde(FALSE)
    ppPIXELS transpose(ppPIXELS& allData,bool AR = true);
    //Function: Revolution the image
    //@ allData   : will dealwith data source
    //@ px        : The Revolution point set x
    //@ py        : The Revolution point set y
    //@ angle     : The Revolution angle(+ Antegrade, - retrograde)
    ppPIXELS revolution(ppPIXELS& allData,int px = 0,int py = 0,float angle = 90.0);
    //Function: Shear the image
    //@ allData   : will dealwith data source
    //@ XorY      : Shear the at x or y direction(TRUE :x FALSE: y)
    //@ angle     : The Shear angle(+ Up/Right, - Down/left)
    ppPIXELS shear(ppPIXELS& allData,bool XorY = true,float angle = 45.0);
    //Function: spatialize the image
    //  [0] : up [1] : down [2] : left [3] : right [4] : front [5] : back
    ppPIXELS spatialize(string outpath);//not work
    //Function: Change a image each pixel's Idensity
    //@ allData   : will dealwith data source
    //@ scala     : Thick  or thin scale
    ppPIXELS density(ppPIXELS& allData,float scale);
    bool     boundarysHL(vTrackTable& Tables);

private://core function
    //Gets all of the border(boundary) line
    void   getBoundarys();
    //Gets the border(boundary) line
    bool   getBoundaryLine(int& x, int& y);
    //is a close/open boundary line or not
    bool   isCloseOpen(dPIXELS boundaryline);
    int    trackDown(PIXELS& startPoint);
    //deburr: delete The Burr on the track
    bool   deburrTrack(dPIXELS& boundaryline);
    //Boundary  highlight
    bool   boundarysHL();
    //bool   boundarysHL(vTrackTable& Tables);
    bool   boundarylineHL(TrackTable& table);
    bool   ImageHL();
    //cut out part of Boundary with cut point(skip table)
    bool   cutOut(vTrackTable& Tables);
    bool   cutAOut(ppPIXELS& imageData,TrackTable& table,FramePoint& FP);
    bool   inTrackLine(vTrackLine line,int x);
private://core function
    //set BackGround with RGBQUAD
    bool   setBackground(RGBQUAD rgb);
    //set BackGround with R,G,B
    bool   setBackground(U8 r = 255,U8 g = 255,U8 b = 255);
    //set BackGround with PIXELS
    bool   setBackground(const PIXELS& pixel);
    //set image granularity
    void   setGranularity(U32 gran,bool opeartor=false);
    //alike background or not,Mean and the same color as
    //the background color
    //NOTE: But not necessarily background
    //return code:
    //   -1 is empty pixel, 1 alike background,0 not alike background
    int    alikeBackground(PIXELS pixel);
    //alike background or not,Mean and the same color as
    //the background color
    //NOTE: But not necessarily background
    //return code:
    //   -1 is empty pixel, 1 alike background,0 not alike background
    int    alikeBackground(int x,int y);
    //test two border upon point similarity
    float  getSimilarity(Position direction,int x,int y,int step = 1);
    //test curr point with background point's similarity
    float  getSimilarity(PIXELS backPoint, PIXELS currPoint);
    //just want to get the right point of one point,if get x,y will be reset
    bool   getRpoint(Position& direction,int& x,int& y);
    //just want to get the left point of one point,if get x,y will be reset
    bool   getLpoint(Position& direction,int& x,int& y);
    void   getNext(Position& pos, int &x,int& y,int& nexts,int& step);
    int    Bsearch(dSkipLine& line,int indexX);
    void   genSkipTable(int x,int y,SkipTable& skipTable2);
    void   genSkipTable(const PIXELS& pixel,SkipTable& skipTable2);
    void   genTrackTable(SkipTable& skipTable2,TrackTable& tracktable);
    //Test a line's Boundary
    bool   isBoundaryPoint(PIXELS pot);
    //Test a line's Boundary
    bool   isBoundaryPoint(int& x,int& y);
    //link each Boundary Line
    void   linker(const Frames& frame);
    //Test whether around the start point has been visited
    //bool testStartP(PIXELS& pixel,int range = 2);
    bool   testStartP(PIXELS& pixel);
    //get a point RGB value from the (x,y)
    //if the x,y is out range ,return empty PIXELS
    PIXELS getPix(int x,int y)throw(ImgException);

private: //utils
    //new/malloc some memory to a save the image data will be deal with
    //@ imageData : pointer the image Data
    //@ W         : the row Create two-dimensional array
    //@ H         : the col Create two-dimensional array
    bool    newData(ppPIXELS& imageData,int W,int H);
    //Copy the imageData to tmpimageData
    //@ imageData    : source image pointer
    //@ tmpimageData : destination iamge pointer
    bool    dataDup2(ppPIXELS& imageData,ppPIXELS& tmpimageData);
    //delete/free the memory image Data after deal with
    bool    delData(ppPIXELS& imageData,int H);
    bool    clearData(ppPIXELS& imageData, int H);
    bool    isEdge(int x,int y);
    //just draw a line
    bool    drawLine(int x,int y,double delta,int length,int size = 1,RGBQUAD* rgb = NULL)throw(ImgException);
    //void    drawLine(int startX, int startY, int endX,int endY,int size = 1,RGBQUAD* rgb = NULL)throw(ImgException);
    void    drawLine(int x1, int y1, int x2, int y2,int size, RGBQUAD* rgb /*= NULL*/) throw(ImgException);
    //draw a line with add avg rgb
    void    drawLine(int startX, int startY, int endX,int endY,RGBQUAD avgrgb,int size = 1)throw(ImgException);
    //just draw Horizontal and Vertical Rect.
    bool    drawRect(int x,int y,int width,int height,int size = 1);
    //just draw Horizontal and Vertical Rect.
    bool    drawRect(const FramePoint& FP ,int size = 1,RGBQUAD* rgb = NULL);
    //test two Frame postion relation
    PRs     checkPR(const FramePoint& A,const FramePoint& B);
    //collision algorithm;
    bool    checkCollision(Frames& fms,int pos,mvect& where,int interval = 0);
    bool    updateFrames(Frames& fms,int pos,mvect& where);
    int     getNearFrame(Frames& fms,int x,int y);
    bool    locateFrame();
    bool    locateMove();
    bool    BrainRepair(PIXELS& A,PIXELS& B);
    bool    BrainRepair(PIXELS& A,bool VorH,float range = 1.0);
    bool    moveLine(dPIXELS& line,mvect where,bool isOverWrite = false);
    bool    moveLine(int sx,int xy,int ex,int ey,mvect where,bool isOverWrite = false);
    bool    moveBoundary(int pos,mvect where,bool isOverWrite = false);
    bool    zoomBoundary(dPIXELS& boundry,int step,Method method = UR,bool isOverWrite = false);
};

#endif // dataPcer.h :[]
