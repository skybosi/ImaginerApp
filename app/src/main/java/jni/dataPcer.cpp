#include "dataPcer.h"
#include <math.h>

#define PI 3.14159
#define R2D(R) R*180/PI
#define D2R(D) D*PI/180
#define OUTRANGE(P) ((P > 255) || (P < 0))
#define EQUALBackGround(rgb) ( !(rgb.rgbRed ^ backGround.rgbRed) && \
                                   !(rgb.rgbGreen ^ backGround.rgbGreen) && \
                                   !(rgb.rgbBlue ^ backGround.rgbBlue) )

/*
 * init  data
 *
 */
bool dataPcer::initData(ppPIXELS data,int width,int height)
{
    if(!data)
    {
        printf("Data provide is fair!\n");
        return false;
    }
    _Data   = data;
    _width  = width;
    _height = height;
    _beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
    return true;
}

/*
 * dataPcer common function
 *
 */

ppPIXELS dataPcer::density(ppPIXELS& allData,float scale)
{
    if(scale <= 0)
    {
        return NULL;
    }
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            allData[y][x] = allData[y][x]*scale;
        }
    }
    return allData;
}

ppPIXELS dataPcer::shear(ppPIXELS& allData,bool XorY,float angle)
{
    angle = D2R(angle);
    ppPIXELS tmpData = NULL;
    int nx, ny;
    dataDup2(allData, tmpData);
    int H = XorY ? _height : (int)(_height + _width * ABS(tan(angle)));
    int W = XorY ? (int)(_width + _height * ABS(tan(angle))) : _width;
    //printf("Shear after: W:%d H:%d\n",W,H);
    delData(allData, _height);	// free
    if (allData == NULL)	// renew
    {
        newData(allData, W, H);
        // printf("new is ok\n");
    }
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            /*
            //right up
            nx = XorY ? x + tan(angle) * (y - _height): x;
            ny = XorY ? y : y + tan(angle) * (x - _width);
            //left down
            nx = XorY ? x - tan(angle) * (y - _height)- _height: x;
            ny = XorY ? y : y - tan(angle) * (x - _width) - _width;
            */

            nx = (XorY ? ((angle > 0) ? (x + tan(angle) * (y - _height)) : (x + tan(angle) * (y - _height) - _height)) : x);
            ny = (XorY ? y : ((angle > 0) ? (y + tan(angle) * (x - _width)) :(y + tan(angle) * (x - _width) - _width)));
            //printf("Shear after: x:%d nx:%d y:%d ny:%d\n",x,nx,y,ny);
            if (nx < 0 || ny < 0 || nx >= _width || ny >= _height)
                allData[y][x].setRGB(255, 255, 255);
            else
                allData[y][x] = tmpData[ny][nx];
        }
    }	// then set allhead
    //TODO:
    _width = W;
    _height= H;
    //reset _height and _width
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dataPcer::revolution(ppPIXELS& allData,int px,int py,float angle)
{
    angle = D2R(angle);
    ppPIXELS tmpData;
    int nx, ny;
    dataDup2(allData, tmpData);
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {

            nx = (x - px) * cos(angle) - (y - py) * sin(angle) + px;
            ny = (x - px) * sin(angle) + (y - py) * cos(angle) + py;
            //printf("angle: %f cos:%f X:%d nx:%d\t Y:%d ny:%d\n",angle, cos(angle),x,nx,y,ny);
            if (nx < 0 || ny < 0 || nx >= _width || ny >= _height)
                allData[y][x].setRGB(255, 255, 255);
            else
                allData[y][x] = tmpData[ny][nx];
        }
    }
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dataPcer::transpose(ppPIXELS& allData,bool AR)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    int W = _height;
    int H = _width;
    delData(allData, _height);	// free
    if (allData == NULL)		// renew
    {
        newData(allData, W, H);
        // printf("new is ok\n");
    }
    // printf("WWW:%d HHH:%d\n",W,H);
#define AorR(x,y) ((AR) ? (tmpData[x][H-1-y]) : (tmpData[W-1-x][y]))
    for (int y = 0;y < H;y++)
    {
        for (int x = 0;x < W;x++)
        {
            allData[y][x] = AorR(x,y);
        }
    }
    // first free tmpData
    delData(tmpData, _height);
    // then set allhead
    //setHead(allhead,W,H);
    //TODO:
    _width = W;
    _height = H;
    //reset _height and _width
    return allData;
}

ppPIXELS dataPcer::spherize(ppPIXELS& allData,float radius)
{
    ppPIXELS tmpData;
    float w = _width / 2.0;
    float h = _height / 2.0;
    float wh = w / h;
    float mx1, mx2;
    int nx;
    float radius2 = radius * radius;
    float dealt;
    dataDup2(allData, tmpData);
    if (radius <= 0.0)			// oval
    {
        for (int y = 0; y < _height; y++)
        {
            //mx1 = w + (w * sqrt(2 * h * y - y * y)) / h;
            //mx2 = w - (w * sqrt(2 * h * y - y * y)) / h;
            mx1 = w + wh * sqrt(2 * h * y - y * y);
            mx2 = w - wh * sqrt(2 * h * y - y * y);
            dealt = (mx1 - mx2) / _width;
            //printf("Y:%d, %d - %d %d ;%f\n", y, mx1, mx2, mx1 - mx2, dealt);
            for (int x = 0; x < _width; x++)
            {
                if (x < mx2 || x > mx1 || dealt == 0)
                    allData[y][x].setRGB(255, 255, 255);
                else
                {
                    nx = (x - mx2) / dealt;
                    if (nx >= _width)
                    {
                        nx = _width - 1;
                    }
                    allData[y][x] = tmpData[y][nx];
                }
            }
        }
    }

    else// circle
    {
        for (int y = 0; y < _height; y++)
        {
            mx1 = w + sqrt(radius2 - (y - h) * (y - h));
            // mx1 = w + sqrt(2*h*y-y*y);
            mx2 = w - sqrt(radius2 - (y - h) * (y - h));
            // mx2 = w - sqrt(2*h*y-y*y);
            dealt = (mx1 - mx2) / (_width * 1.0);
            //printf("Y:%d, %d - %d ;%f\n", y, mx1, mx2, dealt);
            for (int x = 0; x < _width; x++)
            {
                // if(x < mx2 || x > mx1)
                if (x < mx2 || x > mx1 || dealt == 0)
                    allData[y][x].setRGB(255, 255, 255);
                else
                {
                    nx = (x - mx2) / dealt;
                    if (nx >= _width)
                    {
                        nx = _width - 1;
                    }
                    allData[y][x] = tmpData[y][nx];
                }
            }
        }
    }
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dataPcer::zoom(ppPIXELS& allData,float scalex, float scaley)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    if (scalex <= 0.0 || scaley <= 0.0)
    {
        printf("In %s zoom number is <= 0 ,is wrong",__FUNCTION__);
        return NULL;
    }
    else
    {
        int W = _width * scalex;
        int H = _height * scaley;
        delData(allData, _height);	// free
        if (allData == NULL)	// renew
        {
            newData(allData, W, H);
            // printf("new is ok\n");
        }
        //printf("imageZoom: WWW:%d HHH:%d\n",W,H);
        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                int nx = (int)(x / scalex);
                int ny = (int)(y / scaley);
                allData[y][x] = tmpData[ny][nx];
                allData[y][x].setXY(x, y);
                // allData[y][x].show_PIXELS();
                // tmpData[y/scaley][x/scalex].show_PIXELS();
                // printf("\n");
            }
        }
        // first free tmpData
        delData(tmpData, _height);
        // then set allhead
        //TODO:
        _width = W;
        _height = H;
        //reset _height and _width
    }
    return allData;
}

ppPIXELS dataPcer::getImage3Color(ppPIXELS& allData,colorType color)
{
    switch (color)
    {
        case Red:
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = allData[y][x].get3Color(Red);
                }
            }
            break;
        case Green:
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = allData[y][x].get3Color(Green);
                }
            }
            break;
        case Blue:
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = allData[y][x].get3Color(Blue);
                }
            }
            break;
        default:
            break;
    }
    return allData;
}

ppPIXELS dataPcer::mirror(ppPIXELS& allData,Method method)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    switch (method)
    {
        case UD:					// up down change
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[_height - 1 - y][x];
                }
            }
            break;
        case LR:					// left right change
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[y][_width - 1 - x];
                }
            }
            break;
        case UR:					// up down & left right change
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[_height - 1 - y][_width - 1 - x];
                }
            }
            break;
        default:					// NONE
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[y][x];
                }
            }
            break;
    }
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dataPcer::move(ppPIXELS& allData,int mx, int my)
{
#define MOVEX(Mx,x) ((Mx)>0 ? (x <= Mx) : (x > _width-1+Mx))
#define MOVEY(My,y) ((My)>0 ? (y <= My) : (y > _height-1+My))
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            if ((MOVEY(my, y) || MOVEX(mx, x)))
                allData[y][x].setRGB(255, 255, 255);
            else
                allData[y][x] = tmpData[y - my][x - mx];
        }
    }
    delData(tmpData, _height);
    return allData;
}

/*
ppPIXELS dataPcer::imageSpatialize(string outpath)
{
    if (initSpatialize(bmppathl))
    {
        printf("In %s init bmp list image is OK!\n",__FUNCTION__);
        //show_6path(rbmp);
    }
    else
    {
        cout << "" << endl;
        printf("In %s init bmp list image is fair!\n",__FUNCTION__);
        return NULL;
    }
    map<Position,string>::iterator it;
    ppPIXELS _Data[6] = {NULL};
    int i;
    int LH = 0,RH = 0, UH = 0,DH = 0, FH = 0,BH = 0, H = 0,W = 0;
    for(i = 0,it = rbmp.begin(); it != rbmp.end() ; ++it,++i)
    {
        cout<<"key: " << it->first <<" (" << Pos2str(it->first)
            << ")" << "\tvalue: " << it->second << endl;
        bmppath = it->second;
        fp = fopen(bmppath.c_str(), "rb");
        if (!init_image())
        {
            printf("init_image %s is failed\n",(it->second).c_str() );
        }
        printf("it->first:%d\n",it->first);
        switch(it->first)
        {
            case Left:
                imageShear(allData,false,45.0);
                H = allhead.infoHead.biHeight;
                LH = H;
                break;
            case Right:
                imageShear(allData,false,-45.0);
                H = allhead.infoHead.biHeight;
                RH = H;
                break;
            case Up:
                imageShear(allData,true,45.0);
                W = allhead.infoHead.biWidth;
                UH = allhead.infoHead.biHeight;
                break;
            case Down:
                imageShear(allData,true,-45.0);
                W = allhead.infoHead.biWidth;
                DH = allhead.infoHead.biHeight;
                break;
            case Front:
                FH = allhead.infoHead.biHeight;
                break;
            case Back:
                BH = allhead.infoHead.biHeight;
                break;
            default:
                printf("why:%d\n",it->first);
                break;
        }
        printf("shear is ok... W:%d H:%d\n",W,H);
        get_image_msg();
        deleteAll();
    }
    //open OutFile
    size_t pos = bmppath.rfind(".bmp");
    if(pos != string::npos)
        outpath = bmppath.substr(0,pos) +"_Spl.bmp";
    else
        outpath = bmppath +"_Spl.bmp";
    fpo = fopen(outpath.c_str(), "wb");
    if (!fpo)
    {
        printf("In %s open out image is FAILED!\n",__FUNCTION__);
    }
    //write image
    setHead(allhead,W,H);
    //write head
    fwrite(&allhead, 54, 1, fpo);
    //write body
    ppPIXELS newImage = NULL;
    newData(newImage,W,H);
    //memset(newImage,255,sizeof(PIXELS)*W*H);
    //printf("imageData:%p,fpo:%p",_Data[Down],fpo);
    //_Data[Down][0][0].show_PIXELS();
    writeAllData(newImage);
    //free a image memory!
    if(LH)
    {
        //printf("Lwwww%d\n",LH);
        delData(_Data[Left],LH);
    }
    if(RH)
    {
        //printf("Rwwww%d\n",RH);
        delData(_Data[Right],RH);
    }
    if(UH)
    {
        //printf("Uwwww%d\n",UH);
        delData(_Data[Up],UH);
    }
    if(DH)
    {
        //printf("Dwwww%d ",DH);
        delData(_Data[Down],DH);
    }
    if(FH)
    {
        //printf("Fwwww%d\n",FH);
        delData(_Data[Front],FH);
    }
    if(BH)
    {
        //printf("Bwwww%d\n",BH);
        delData(_Data[Back],BH);
    }
    return NULL;
}*/


bool     dataPcer::genHistogram(ppPIXELS& allData,colorType color,bool isOverWrite )
{
    if(NULL == allData)
    {
        printf("In %s allData is NULL\n",__FUNCTION__);
        return false;
    }
    float PIXELSCOUNT = _width * _height;
    int RED[256] = {0};
    float FRED[256] = {0};
    int GRE[256] = {0};
    float FGRE[256] = {0};
    int BLU[256] = {0};
    float FBLU[256] = {0};
    int ALL[256] = {0};
    float FALL[256] = {0};
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            RED[allData[y][x].getRed()]++;
            GRE[allData[y][x].getGreen()]++;
            BLU[allData[y][x].getBlue()]++;
        }
    }
    for(int i = 0; i < 256; i++)
    {
        FRED[i] = RED[i] / PIXELSCOUNT;
        FGRE[i] = GRE[i] / PIXELSCOUNT;
        FBLU[i] = BLU[i] / PIXELSCOUNT;
        ALL[i] = RED[i] + GRE[i] + BLU[i];
        FALL[i] = ALL[i] / PIXELSCOUNT;
        //FALL[i] = FRED[i] + FGRE[i] + FBLU[i];
        //printf("i:%d R:%d(%f) G:%d(%f) B:%d(%f)\n",i,RED[i],FRED[i],GRE[i],FGRE[i],BLU[i],FBLU[i]);
        //printf("%d %d %d %d %d\n",i,RED[i],GRE[i],BLU[i],ALL[i]);
    }
    float Rmax = 0,Gmax = 0,Bmax = 0,Amax = 0;
    for(int i = 0; i < 256; i++)
    {
        if(FRED[i] > Rmax)
            Rmax=FRED[i];
        if(FGRE[i] > Gmax)
            Gmax=FGRE[i];
        if(FBLU[i] > Bmax)
            Bmax=FBLU[i];
    }
    Amax  = Rmax + Gmax + Bmax;
    //printf("PIXELSCOUNT: %f\n",PIXELSCOUNT);
    //printf("R:%f G:%f B:%f A:%f\n",Rmax,Gmax,Bmax,Amax);
    //set allhead
    //set Histogram's frame
    int border = 1;
    int Lmargin = 20, Rmargin = 20;
    int Dmargin = 20, Umargin = 20;
    int Lb = Lmargin + border;
    int Rb = Rmargin + border;
    int Ub = Umargin + border;
    int Db = Dmargin + border;
#define Hh 158
    int W = 255 + Lb + Rb;
    int H = Hh + Ub + Db;// 158 = 256*0.618 golden section
    int H_D = H - Dmargin;
    int W_R = W - Rmargin;
    int H_Db = H - Db;
    int W_Rb = W - Rb;
    float Rscale = Hh/Rmax;
    float Gscale = Hh/Gmax;
    float Bscale = Hh/Bmax;
    float Ascale = Hh/Amax;
    //printf("R:%f G:%f B:%f A:%f\n",Rscale,Gscale,Bscale,Ascale);
    if(isOverWrite)
    {
        delData(allData, _height);	// free
        if (allData == NULL)	// renew
        {
            newData(allData, W, H);
            _width = W;
            _height = H;
            // printf("new is ok\n");
        }
    }
    bool flag = true;
    for (int y = H - 1; y >= 0; y--)
    {
        for (int x = 0; x < W; x++)
        {
            if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
            {
                allData[y][x].setRGB(255,255,255);
            }
            else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
                    || (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
            {
                allData[y][x].setRGB(0,0,0);
            }
            else
            {
                switch(color)
                {
                    case Red:
                        if(FRED[x-Rb]*Rscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(0,0,255);

                        }
                        else
                            flag = true;
                        break;
                    case Green:
                        if(FGRE[x-Rb]*Gscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(0,255,0);
                        }
                        else
                            flag = true;
                        break;
                    case Blue:
                        if(FBLU[x-Rb]*Bscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(255,0,0);
                        }
                        else
                            flag = true;
                        break;
                    case Pricolor:
                        if(FALL[x-Rb]*Ascale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(57,77,118);
                        }
                        else
                            flag = true;
                        break;
                    default:
                        printf("NO THIS COLOR!\n");
                        break;
                }
                if(flag)
                {
                    allData[y][x].setRGB(255,255,255);
                }
            }
        }
    }
#undef  Hh
    return true;
}

bool     dataPcer::genBardiagram(ppPIXELS& allData,colorType color,bool isOverWrite )
{
    if(NULL == allData)
    {
        printf("In %s allData is NULL\n",__FUNCTION__);
        return false;
    }
    int RED[256] = {0};
    int GRE[256] = {0};
    int BLU[256] = {0};
    int ALL[256] = {0};
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            RED[allData[y][x].getRed()]++;
            GRE[allData[y][x].getGreen()]++;
            BLU[allData[y][x].getBlue()]++;
        }
    }
    float Rmax = 0,Gmax = 0,Bmax = 0,Amax = 0;
    for(int i = 0; i < 256; i++)
    {
        ALL[i] = RED[i] + GRE[i] + BLU[i];
        if(RED[i] > Rmax)
            Rmax=RED[i];
        if(GRE[i] > Gmax)
            Gmax=GRE[i];
        if(BLU[i] > Bmax)
            Bmax=BLU[i];
        //printf("i:%d R:%d G:%d B:%d\n",i,RED[i],GRE[i],BLU[i]);
    }
    Amax = Rmax + Gmax + Bmax;
    //set allhead
    //set Histogram's frame
    int border = 1;
    int Lmargin = 20, Rmargin = 20;
    int Dmargin = 20, Umargin = 20;
    int Lb = Lmargin + border;
    int Rb = Rmargin + border;
    int Ub = Umargin + border;
    int Db = Dmargin + border;
#define Hh 158
    int W = 255 + Lb + Rb;
    int H = Hh + Ub + Db;// 158 = 256*0.618 golden section
    int H_D = H - Dmargin;
    int W_R = W - Rmargin;
    int H_Db = H - Db;
    int W_Rb = W - Rb;
    float Rscale = Hh/Rmax;
    float Gscale = Hh/Gmax;
    float Bscale = Hh/Bmax;
    float Ascale = Hh/Amax;
    if(isOverWrite)
    {
        delData(allData, _height);	// free
        if (allData == NULL)	// renew
        {
            newData(allData, W, H);
            _width = W;
            _height = H;
            // printf("new is ok\n");
        }
    }
    bool flag = true;
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
            {
                allData[y][x].setRGB(255,255,255);
            }
            else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
                    || (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
            {
                allData[y][x].setRGB(0,0,0);
            }
            else
            {
                switch(color)
                {
                    case Red:
                        if(RED[x-Rb]*Rscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(0,0,255);
                        }
                        else
                            flag = true;
                        break;
                    case Green:
                        if(GRE[x-Rb]*Gscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(0,255,0);
                        }
                        else
                            flag = true;
                        break;
                    case Blue:
                        if(BLU[x-Rb]*Bscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(0,0,255);
                        }
                        else
                            flag = true;
                        break;
                    case Pricolor:
                        if(ALL[x-Rb]*Ascale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGB(57,77,118);
                        }
                        else
                            flag = true;
                        break;
                    default:
                        printf("NO THIS COLOR!\n");
                        break;
                }
                if(flag)
                {
                    allData[y][x].setRGB(255,255,255);
                }
            }
        }
    }
#undef  Hh
    return true;
}

/*
 * dataPcer core function
 *
 */

bool     dataPcer::getBoundaryLine(int& x, int& y)
{
    size_t bsize = boundarys.size();
    x = trackDown(_Data[y][x]);
    if( boundarys.size() != bsize)
    {
        return true;
    }
    return false;
    //printf("next footprint'x value:%d\n",x+1);
}

void     dataPcer::getBoundarys()
{
    _beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
#define debug1
    PIXELS tmp;
    for (int y = 0;y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            if(isBoundaryPoint(x,y))
            {
                tmp = _Data[y][x];
                if(tmp.getEdge() >= 0)
                {
                    if(!getBoundaryLine(x,y))
                    {
                        //printf("getBoundaryLine flase\n");
                    }
                }
                else
                {
                }
            }
        }
    }
#ifdef debug
here:	printf("OOOOOOKKKKK!\n");
#endif
    printf("granularity: %u boundarys size:%ld\n",granularity,boundarys.size());
    printf("skip Table size:%ld\n",trackTables.size());
    /*
    for (size_t i =0; i < boundarys.size(); i++)
    {
        printf("$[%ld]: close or open status: %s ;boundary line len: %ld,(%u)\n",
                i,CLOSEOPEN(isCloseOpen(boundarys[i])),boundarys[i].size(),granularity);
        show_line(boundarys[i]);
    }
    vector<limitXXY>::const_iterator it;
    for(it = skipTable.begin(); it != skipTable.end();++it)
    {
        printf("start x:%d end x:%d communal y:%d\n", it->sttx,it->endx,it->ally);
    }

    printf("Frames point:%ld\n",frames.size());
    vector<FramePoint>::const_iterator itPoint;
    for(itPoint = frames.begin(); itPoint != frames.end();++itPoint)
    {
        printf("UPy:%d Downy:%d Leftx:%d Rightx:%d\n",
                (*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
    }
    */

}

bool     dataPcer::isCloseOpen(dPIXELS boundaryline)
{
    if(boundaryline.empty())
        return false;
    else
        return (boundaryline.front() == boundaryline.back());
}

//start track down by following clues(顺藤摸瓜)
int      dataPcer::trackDown(PIXELS& startPoint)
{
    int sx = startPoint.getX()-_beginX;
    int x = sx;
    int sy = startPoint.getY()-_beginY;
    int y = sy;
    //make sure not trackDown again
    if(!boundarys.empty() && testStartP(startPoint))
        return sx;
    int nextx = 0;
    dPIXELS boundaryline;
    SkipTable skipTable2;
    Position direction = Right;
    //each direction relative to the image
    if(getRpoint(direction,x,y))
    {
        //make sure not only one point
        if (x < sx || y < sy)
        {
            sx--;
            return sx;
        }
    }
    else //when at first line,maybe out-of-range
    {
        return sx++;
    }
    startPoint.setpPosStatus(direction);
    //boundaryline.push_back(startPoint);
    Position prevDiret = direction;
    FramePoint framePoint(_height+_beginY,_width+_beginX);//此处的入参要注意
    x = sx,y = sy;
    int rx = sx,ry = sy;
    int lx = sx,ly = sy;
    Position ldirection = direction;
    Position rdirection = direction;
    bool rEdge = false;
    //bool lEdge = false;

    //anticlockwise(getRpoint)
    x = rx;//rollback lx to x
    y = ry;//rollback ly to y
    direction = rdirection;//rollback rdirection to direction
    while(1)//getRpoint
    {
        PIXELS& prevPoint = _Data[y][x];
        prevDiret = direction;
        //printf("rdirection:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
        if(getRpoint(direction,x,y))
        {
            if(x == rx && y == ry)//back to the start point
            {
                if(ISH(prevDiret))
                {
                    prevPoint.setEdge(-2);
                }
                else
                {
                    prevPoint.setEdge(-1);
                    //genSkipTable(prevPoint,skipTable2);
                }
                //set frame Point
                framePoint.setframePoint(direction,_Data[y][x]);
                //set boundary line point
                boundaryline.push_back(prevPoint);
                break;
            }
            if(prevDiret == direction)
            {
                if(ISV(prevDiret))
                {
                    prevPoint.setEdge(-1);
                    genSkipTable(prevPoint,skipTable2);
                }
                else
                {
                    prevPoint.setEdge(-2);
                }
            }
            else//direction is change
            {
                prevPoint.setpPosStatus(prevDiret);//set Corner point
                int direSum = prevDiret + direction;
                switch(direSum)
                {
                    case 3://reset direction
                        prevDiret = direction;
                        if(ISV(direction))
                        {
                            prevPoint.setEdge(-1);
                            genSkipTable(prevPoint,skipTable2);
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
                            genSkipTable(prevPoint,skipTable2);
                        }
                        break;
                    case 5://LR or RL
                        prevPoint.setEdge(-1);
                        genSkipTable(prevPoint,skipTable2);
                        if(prevDiret > direction)
                            prevPoint.setpPosStatus(Up);
                        else
                            prevPoint.setpPosStatus(Down);
                        break;
                    default://case 1:
                        prevPoint.setEdge(-1);
                        genSkipTable(prevPoint,skipTable2);
                        genSkipTable(prevPoint,skipTable2);
                        break;
                }
            }
            //set frame Point
            framePoint.setframePoint(direction,_Data[y][x]);
            //set boundary line point
            boundaryline.push_back(prevPoint);
            /*
               printf("push a: ");
               getPix(x,y).show_PIXELS();
               printf("\n");
               */
        }
        else //at Edge,Over loop
        {
            rEdge = true;
            if(ISH(direction))
                genSkipTable(prevPoint,skipTable2);
            break;
        }
    }

    //clockwise(getLpoint),if anticlockwise at Edge
    x = lx;//rollback lx to x
    y = ly;//rollback ly to y
    direction = ldirection;//rollback ldirection to direction
    while(rEdge)//getLpoint
    {
        PIXELS& prevPoint = _Data[y][x];
        prevDiret = direction;
        //printf("rdirection:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
        if(getLpoint(direction,x,y))
        {
            if(x == lx && y == ly)//back to the start point
            {
                if(ISH(prevDiret))
                {
                    prevPoint.setEdge(-2);
                }
                else
                {
                    prevPoint.setEdge(-1);
                    genSkipTable(prevPoint,skipTable2);
                }
                //set frame Point
                framePoint.setframePoint(direction,_Data[y][x]);
                //set boundary line point
                boundaryline.push_back(prevPoint);
                /*
                   printf("push a: ");
                   getPix(x,y).show_PIXELS();
                   printf("\n");
                   */
                break;
            }
            if(prevDiret == direction)
            {
                if(ISV(prevDiret))
                {
                    prevPoint.setEdge(-1);
                    genSkipTable(prevPoint,skipTable2);
                }
                else
                {
                    prevPoint.setEdge(-2);
                }
            }
            else//direction is change
            {
                prevPoint.setpPosStatus(prevDiret);//save Corner point
                int direSum = prevDiret + direction;
                switch(direSum)
                {
                    //reset direction
                    case 2:
                    case 4:
                        prevDiret = direction;
                        if(ISV(direction))//?
                        {
                            prevPoint.setEdge(-1);
                            genSkipTable(prevPoint,skipTable2);
                        }
                        else
                        {
                            prevPoint.setEdge(-2);
                        }
                        break;
                    case 3:
                        if(ISV(direction))
                        {
                            prevPoint.setEdge(-2);
                        }
                        else
                        {
                            prevPoint.setEdge(-1);
                            genSkipTable(prevPoint,skipTable2);
                        }
                        break;
                    case 5://LR or RL
                        prevPoint.setEdge(-1);
                        genSkipTable(prevPoint,skipTable2);
                        if(prevDiret < direction)
                            prevPoint.setpPosStatus(Up);
                        else
                            prevPoint.setpPosStatus(Down);
                        break;
                    default://case 1:
                        prevPoint.setEdge(-1);
                        genSkipTable(prevPoint,skipTable2);
                        genSkipTable(prevPoint,skipTable2);
                        break;
                }
            }
            //set frame Point
            framePoint.setframePoint(direction,_Data[y][x]);
            //set boundary line point
            boundaryline.push_back(prevPoint);
            /*
               printf("push a: ");
               getPix(x,y).show_PIXELS();
               printf("\n");
               */
        }
        else //at Edge,Over loop
        {
            //lEdge = true;
            if(ISH(direction))
                genSkipTable(prevPoint,skipTable2);
            break;
        }
    }
    //startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
#define GRANOPERATION(size) (granOpeartor)?(size > granularity):(size <= granularity)
    if(GRANOPERATION(boundaryline.size()))
    {
        deburrTrack(boundaryline);
        TrackTable tracktable;
        skipTables.push_back(skipTable2);
        genTrackTable(skipTable2,tracktable);
        trackTables.push_back(tracktable);
        boundarys.push_back(boundaryline);
        framePoint.setBindNum(boundarys.size());
        frames.push_back(framePoint);
        nextx = boundaryline.size();
    }
    else
    {
        nextx = 0;
        int xx = 0,yy = 0;
        while(!boundaryline.empty())//clean the smaller than the granularity data
        {
            xx = boundaryline.front().getX()-_beginX;
            yy = boundaryline.front().getY()-_beginY;
            _Data[yy][xx].setEdge(0);
            _Data[yy][xx].initpPos();
            boundaryline.pop_front();
        }
    }
    //printf("$[%d]> close or open status: %s Track down by following clues(顺藤摸瓜) OK... len:%ld(%u)\n",
    //globalI,CLOSEOPEN(isCloseOpen(boundaryline)),boundaryline.size(),granularity);
    //get next point's x value
    int NextX = sx;
    while(--nextx >= 0 && boundaryline[nextx].getY()-_beginY == sy
            && boundaryline[nextx].getX()-_beginX >= NextX)
    {
        NextX = boundaryline[nextx].getX()-_beginX;
        //printf("%d mx %d\n",nextx,maxX);
    }
    //printf("%d mx %d\n",nextx,maxX);
    //printf("The max x %d will be nextpoint\n",maxX);
    return NextX;
}

bool     dataPcer::setBackground(RGBQUAD rgb)
{
    /*
    if(OUTRANGE(rgb.rgbRed) || OUTRANGE(rgb.rgbGreen) || OUTRANGE(rgb.rgbBlue))
        return false;
        */
    backGround = rgb;
    return true;
}

bool     dataPcer::setBackground(U8 r,U8 g,U8 b)
{
    /*
    if(OUTRANGE(r) || OUTRANGE(g) || OUTRANGE(b))
        return false;
        */
    backGround.setRGBA(r,g,b);
    return true;
}

bool     dataPcer::setBackground(const PIXELS& pixel)
{
    return setBackground(pixel.getRGB());
}

void     dataPcer::setGranularity(U32 gran,bool opeartor)
{
    granularity = gran;
    granOpeartor = opeartor;
}

int      dataPcer::alikeBackground(PIXELS pixel)
{
    PIXELS backGroundpixel(backGround);
    if(getSimilarity(backGroundpixel,pixel) > baseSmlrty)
        return 1;
    return 0;
}

int      dataPcer::alikeBackground(int x,int y)
{
    PIXELS pixle;
    try{
        pixle = getPix(x,y);
    }catch(ImgException& e)
    {
        //printf("%s\n",e.msg.c_str());
        return -1;
    }catch(...)
    {
        return -1;
    }
    return alikeBackground(pixle);
}

float    dataPcer::getSimilarity(PIXELS backPoint, PIXELS currPoint)
{
    float Similarity = 0;
    PIXELS diff = backPoint - currPoint;
    /*
       float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
       SmlrtyR = 1 - diff.getRed()/255.0;
       SmlrtyG = 1 - diff.getGreen()/255.0;
       SmlrtyB = 1 - diff.getBlue()/255.0;
       */
    //Similarity = 1- abs(diff.getRed() + diff.getGreen() + diff.getBlue())/765.0;
    diff = ~diff;
    Similarity = (diff.getRed() + diff.getGreen() + diff.getBlue())/765.0;
    //printf("curr x: %2d y: %2d\tSimilarity: %.3f\tSmlrtyR: %.3f\tSmlrtyG: %.3f\tSmlrtyB: %.3f\n",
    //       currPoint.getX()-_beginX,currPoint.getY()-_beginY,Similarity,SmlrtyR,SmlrtyG,SmlrtyB);
    //return TMAX(SmlrtyR,SmlrtyB,SmlrtyG);
    return Similarity;
}

float    dataPcer::getSimilarity(Position direction,int x,int y,int step)
{
    float Similarity = 0;
    //float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
    try
    {
        getPix(x,y);
    }
    catch(ImgException& e)
    {
        return -1;
    }
    PIXELS potCurnt;
    PIXELS potRight;
    switch(direction)
    {
        case Down:
            if(y+step >= _height)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y+step][x];
            break;
        case Right:
            if(x+step >= _width)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y][x+step];
            break;
        case Up:
            if(y-step < 0)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y-step][x];
            break;
        case Left:
            if(x-step < 0)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y][x-step];
            break;
        default:
            break;
    }
    PIXELS diff = potCurnt - potRight;
    //SmlrtyR = 1 - diff.getRed()/255.0;
    //SmlrtyG = 1 - diff.getGreen()/255.0;
    //SmlrtyB = 1 - diff.getBlue()/255.0;
    /*
       printf("curr:\n");
       potCurnt.show_PIXELS();
       printf("\n");
       printf("Right:\n");
       potRight.show_PIXELS();
       printf("\n");
       printf("diff:\n");
       diff.show_PIXELS();
       printf("\n");
       printf("~diff:\n");
       */
    diff = ~diff;
    /*
       diff.show_PIXELS();
       printf("\n");
       */
    Similarity = (diff.getRed() + diff.getGreen() + diff.getBlue())/765.0;
    /*
       printf("x: %2d y: %2d Similarity: %.3f\n",x,y,Similarity);
       if(Similarity != 1 && Similarity > baseSmlrty)
       baseSmlrty = Similarity;
       printf("curr x: %2d y: %2d\tSimilarity: %.3f\tSmlrtyR: %.3f\tSmlrtyG: %.3f\tSmlrtyB: %.3f\n",
       x,y,Similarity,SmlrtyR,SmlrtyG,SmlrtyB);
       */
    //return TMAX(SmlrtyR,SmlrtyB,SmlrtyG);
    return Similarity;
}

bool     dataPcer::getRpoint(Position& direction,int& x,int& y)
{
    int flagxy = 0;
    //printf("direction:%s x:%d y:%d\t",Pos2str(direction).c_str(),x,y);
    switch (direction)
    {
        case Right:
            flagxy = alikeBackground(x,y + 1);
            if (flagxy == 1)
            {
                //direction = Down;
                //y++;
                DIRECTION_Down;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x + 1,y);
                if (flagxy == 1)
                {
                    x++;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x,y - 1);
                    if (flagxy == 1)
                    {
                        //direction = Up;
                        //y--;
                        DIRECTION_Up;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Left;
                        //x--;
                        DIRECTION_Left;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case Down:
            flagxy = alikeBackground(x - 1,y);
            if (flagxy == 1)
            {
                //direction = Left;
                //x--;
                DIRECTION_Left;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x,y + 1);
                if (flagxy == 1)
                {
                    y++;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x + 1,y);
                    if (flagxy == 1)
                    {
                        //direction = Right;
                        //x++;
                        DIRECTION_Right;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Up;
                        //y--;
                        DIRECTION_Up;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case Up:
            flagxy = alikeBackground(x + 1,y);
            if (flagxy == 1)
            {
                //direction = Right;
                //x++;
                DIRECTION_Right;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x,y - 1);
                if (flagxy == 1)
                {
                    y--;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x - 1,y);
                    if (flagxy == 1)
                    {
                        //direction = Left;
                        //x--;
                        DIRECTION_Left;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Down;
                        //y++;
                        DIRECTION_Down;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case Left:
            flagxy = alikeBackground(x ,y - 1);
            if (flagxy == 1)
            {
                //direction = Up;
                //y--;
                DIRECTION_Up;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x - 1,y);
                if (flagxy == 1)
                {
                    x--;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x,y + 1);
                    if (flagxy == 1)
                    {
                        //direction = Down;
                        //y++;
                        DIRECTION_Down;
                    }
                    else if(flagxy == 0)
                    {
                        //direction = Right;
                        //x++;
                        DIRECTION_Right;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        default:
            break;
    }
    return true;
}

bool     dataPcer::getLpoint(Position& direction,int& x,int& y)
{
    int flagxy = 0;
    PIXELS pot;
    //printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
    switch (direction)
    {
        case Right:
            flagxy = alikeBackground(x ,y - 1);
            if (flagxy == 1)
            {
                // direction = Up;
                // y--;
                DIRECTION_Up;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x + 1,y);
                if (flagxy == 1)
                {
                    x++;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x,y + 1);
                    if (flagxy == 1)
                    {
                        // direction = Down;
                        // y++;
                        DIRECTION_Down;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Left;
                        //x--;
                        DIRECTION_Left;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case Down:
            flagxy = alikeBackground(x + 1,y);
            if (flagxy == 1)
            {
                // direction = Right;
                // x++;
                DIRECTION_Right;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x,y + 1);
                if (flagxy == 1)
                {
                    y++;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x - 1,y);
                    if (flagxy == 1)
                    {
                        // direction = Left;
                        // x--;
                        DIRECTION_Left;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Up;
                        //y--;
                        DIRECTION_Up;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case Up:
            flagxy = alikeBackground(x - 1,y);
            if (flagxy == 1)
            {
                // direction = Left;
                // x--;
                DIRECTION_Left;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x,y - 1);
                if (flagxy == 1)
                {
                    y--;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x + 1,y);
                    if (flagxy == 1)
                    {
                        // direction = Right;
                        // x++;
                        DIRECTION_Right;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Down;
                        //y++;
                        DIRECTION_Down;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case Left:
            flagxy = alikeBackground(x ,y + 1);
            if (flagxy == 1)
            {
                // direction = Down;
                // y++;
                DIRECTION_Down;
            }
            else if (flagxy == 0)
            {
                flagxy = alikeBackground(x - 1,y);
                if (flagxy == 1)
                {
                    x--;
                }
                else if(flagxy == 0)
                {
                    flagxy = alikeBackground(x,y - 1);
                    if (flagxy == 1)
                    {
                        //direction = Up;
                        //y--;
                        DIRECTION_Up;
                    }
                    else if (flagxy == 0)
                    {
                        //direction = Right;
                        //x++;
                        DIRECTION_Right;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        default:
            break;
    }
    return true;
}

void     dataPcer::getNext(Position& pos, int &x,int& y,int& nexts,int& step)
{
    nexts++;
    switch(pos)
    {
        case Left:
            x--;
            if(nexts==step)
            {
                pos = Down;
                nexts = 0;
            }
            break;
        case Down:
            y++;
            if(nexts==step)
            {
                step++;
                pos = Right;
                nexts = 0;
            }
            break;
        case Right:
            x++;
            if(nexts==step)
            {
                pos = Up;
                nexts = 0;
            }
            break;
        case Up:
            y--;
            if(nexts==step)
            {
                step++;
                pos = Left;
                nexts = 0;
            }
            break;
        default:
            break;
    }
}

int      dataPcer::Bsearch(dSkipLine& line,int indexX)
{
    int len = line.size();
    int high = len -1,low = 0;//假设数组是从小到大排列的
    int midle = len >> 1;
    int curX = 0;
    while(high >= low)
    {
        midle = (high + low) >> 1;
        curX = line[midle].getX()-_beginX;
        if(curX  == indexX)
            return midle + 1;
        if(curX > indexX)
        {
            high = midle - 1;
        }
        else if(curX < indexX )
        {
            low = midle + 1;
        }
    }
    return low;
}

void     dataPcer::genSkipTable(int x,int y,SkipTable& skipTable2)
{
    //allData[y][x].setRGB(0,0,255);
    dSkipLine line;
    dSkipLine::iterator it;
    int deltaY = -1;
    int pos = -1;
    if(!skipTable2.empty())
    {
        deltaY = (y +_beginY) - skipTable2[0][0].getY();
    }
    if(deltaY >= 0 && deltaY < (int)skipTable2.size())
    {
        it = skipTable2[deltaY].begin();
        pos = Bsearch(skipTable2[deltaY],x);
        skipTable2[deltaY].insert(it+pos,_Data[y][x]);
    }
    else // a new watch line
    {
        line.push_back(_Data[y][x]);
        skipTable2.push_back(line);
    }
}

void     dataPcer::genSkipTable(const PIXELS& pixel,SkipTable& skipTable2)
{
    int x = pixel.getX()-_beginX;
    int y = pixel.getY()-_beginY;
    genSkipTable(x,y,skipTable2);
}

void     dataPcer::genTrackTable(SkipTable& skipTable2,TrackTable& tracktable)
{
    size_t skptlen = skipTable2.size();
    dSkipLine line;
    vTrackLine lline;
    limitXXY lxxy;
    size_t j = 0;
    for (size_t i = 0; i < skptlen; i ++)
    {
        line = skipTable2[i];
        size_t len = line.size();
        j = 0;
        if(len == 1)
        {
            lxxy.add(line[0],line[0],lline);
        }
        else
        {
            while(j < len)
            {
                lxxy.add(line[j],line[j+1],lline);
                j+=2;
            }
        }
        tracktable.push_back(lline);
        lline.clear();
    }
}

bool     dataPcer::isBoundaryPoint(PIXELS pot)
{
    if(alikeBackground(pot) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool     dataPcer::isBoundaryPoint(int& x,int& y)
{
    int i = 0;
    float similarity = 1;
    //float checkSmlrty = 0;
    float avgSimi = 0;
    float diffSim = 0;
    for (i = 0; x < _width-1; ++i,++x)
    {
        diffSim  = similarity - avgSimi;
        similarity = getSimilarity(Right,x,y);
        //printf("%3d: num:%lf\tavg:%lf\tdiff:%lf\n", i + 1, similarity, avgSimi, fabs(diffSim));
        avgSimi += diffSim / (i + 1);
        if (fabs(similarity - avgSimi) > 0.1)
        {
            ++x;
            setBackground(_Data[y][x]);
            //printf("finded :%lf\n", similarity);
            //make sure the edge point is not a shade
            //work is not stable, need TODO
            /*
            if(allData[y][x].getEdge() >= 0)
            {
                checkSmlrty = getSimilarity(Right,x,y);
                if(checkSmlrty != 1 && checkSmlrty != similarity)
                {
                    ++x;
                    setBackground(allData[y][x]);
                    similarity = checkSmlrty;
                }
            }
            */
            baseSmlrty = similarity;
            // printf("baseSmlrty:%lf\n",baseSmlrty);
            break;
        }
    }
    if(x < _width-1)
    {
        return true;
    }
    else
        return false;
}

void     dataPcer::linker(const Frames& frame)
{
    printf("Frames point:%ld\n",frame.size());
    vector<FramePoint>::const_iterator itPoint;
    for(itPoint = frame.begin(); itPoint != frame.end();++itPoint)
    {
        printf("UPy:%d Downy:%d Leftx:%d Rightx:%d\n",
                (*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
    }
}

/*
//return: true trackDown again
bool dataPcer::testStartP(PIXELS& pixel,int range)
{
    range = testRange;
    int x = pixel.getX()-_beginX;
    int y = pixel.getY()-_beginY;
    if(x < range || x > _width - 1 - range)
        return true;
    int i = 1;
    while(i <= range)
    {
        if(allData[y][x-i].getEdge() < 0 || allData[y][x+i].getEdge() < 0)
        {
            testRange = std::max(i,(int)testRange);
            //printf("testRange:%d\n",testRange);
            return true;
        }
        i++;
    }
    return false;

//	if(allData[y][x-3].getEdge() < 0 || allData[y][x-2].getEdge() < 0 ||
//			allData[y][x-1].getEdge() < 0 || allData[y][x].getEdge() < 0 ||
//			allData[y][x+1].getEdge() < 0 || allData[y][x-1].getEdge() < 0)
//		return true;
//	else
//		return false;

}*/

bool     dataPcer::testStartP(PIXELS& pixel)
{
    int step = 1;
    int x = pixel.getX()-_beginX;
    int y = pixel.getY()-_beginY;
    Position nextPos = Left;
    int numPoint = 1;
    int nexts = 0;
    //    pixel.show_PIXELS();
    //    printf("\n");
    getNext(nextPos,x,y,nexts,step);
    //    allData[y][x].show_PIXELS();
    //    printf("\n");
    while( !isEdge(x,y) && numPoint < 25 && _Data[y][x].getEdge() >= 0)
    {
        getNext(nextPos,x,y,nexts,step);
        //        allData[y][x].show_PIXELS();
        //        printf("\n");
        numPoint++;
    }
    //printf("%d\t",numPoint);
    if(numPoint >=25)
        return false;
    return true;
}

PIXELS   dataPcer::getPix(int x, int y) throw(ImgException)
{
#define OUTIMAGE(_X,_Y,_W,_H) ((_X >= _W) || (_Y >= _H) || (_X < 0) || (_Y < 0))
    if(! OUTIMAGE(x,y,_width,_height))
        return _Data[y][x];
    else
    {
        throw ImgException("out of image RANGE!");
    }
#undef  OUTIMAGE
}

bool     dataPcer::deburrTrack(dPIXELS& boundaryline)
{
    printf("in deburrTrack...\n");
    /*
    if(boundaryline.empty())
        return false;
    int j = 1;
    int x ,y;
    dPIXELS::iterator itPoint = boundaryline.begin();
    for (;itPoint != boundaryline.end(); ++itPoint)
    {
        if(itPoint->getEdge() == -1)
        {
            while((itPoint-j != boundaryline.begin())
                    && (*(itPoint-j) == *(itPoint+j)))
            {
                y = (itPoint-j)->getY()-_beginY;
                x = (itPoint-j)->getX()-_beginX;
                printf("find back point! j:%d\n",j);
                //(itPoint-j)->setEdge(-3);
                boundaryline.erase(itPoint-j);
                _Data[y][x].setEdge(-3);
                (itPoint-j)->show_PIXELS();
                printf("\n===============\n");
                y = (itPoint+j)->getY()-_beginY;
                x = (itPoint+j)->getX()-_beginX;
                (itPoint+j)->setEdge(-3);
                _Data[y][x].setEdge(-3);
                (itPoint+j)->show_PIXELS();
                printf("\n");
                j++;
            }
            j = 1;
        }
    }*/

    if (boundaryline.empty())
        return false;
    //cornerSize++;//size must add 1,
    int cornerSize = 2;
    int corner = 0;
    dPIXELS::iterator iter = boundaryline.begin();
    dPIXELS::iterator fast, low;
    for (;iter != boundaryline.end();)
    {
        fast = iter + cornerSize;
        low =  iter;
        if(*fast == *low)
        {
            do
            {
                ++corner,++iter;
                fast = iter + cornerSize;
                low = iter -(corner<<1);
            }while (*fast == *low);
            (corner<<=1)+=(cornerSize-1);

            //insert value
            //boundaryline.insert(low+1,pointer(low->getX(),low->getY()+1));
            iter = boundaryline.erase(low+1,fast);
            corner = 0;
        }
        else
             ++iter;
    }
    return true;
}

/*
bool dataPcer::deburrTrack(dPIXELS& boundaryline)
{
    printf("in deburrTrack...\n");
    if(boundaryline.empty())
        return false;
    int j = 1;
    int x ,y;
    for (size_t i = 1; i < boundaryline.size(); i ++)
    {
        if(boundaryline[i].getEdge() == -1)
        {
            while((i-j > 0) && (boundaryline[i-j] == boundaryline[i+j]))
            {
                y = boundaryline[i-j].getY()-_beginY;
                x = boundaryline[i-j].getX()-_beginX;
                printf("find back point! j:%d\n",j);
                boundaryline[i-j].setEdge(-3);
                allData[y][x].setEdge(-3);
                boundaryline[i-j].show_PIXELS();
                printf("\n===============\n");
                y = boundaryline[i+j].getY()-_beginY;
                x = boundaryline[i+j].getX()-_beginX;
                boundaryline[i+j].setEdge(-3);
                allData[y][x].setEdge(-3);
                boundaryline[i+j].show_PIXELS();
                printf("\n");
                j++;
            }
            j = 1;
        }
    }
    return true;
}
*/

bool     dataPcer::boundarysHL()
{
        getBoundarys();
#define AVG
#ifdef EDGE
    ImageHL();
#else

    if(boundarys.empty())
        return false;
    int x = 0,y = 0;
    int avg  = 0;
    size_t boundarysLen  = boundarys.size();
    if(boundarysLen)
        avg = 255/ boundarysLen;
    for (size_t i = 0; i < boundarysLen; i++)
    {
        for (size_t j = 0; j < boundarys[i].size(); j ++)
        {
            y = boundarys[i][j].getY()-_beginY;
            x = boundarys[i][j].getX()-_beginX;
#ifdef FINGERPRINT
            U8 tmpnum = _Data[y][x].getpPosStatus();
            if(tmpnum)
                _Data[y][x].setRGB(0,255,0);
#endif
#ifdef  ONE
            //if(allData[y][x].getEdge() == -2)
            //allData[y][x].setRGB(255,0,0);
            if(_Data[y][x].getEdge() == -1)
            {
                /*
                PIXELS::pix_p tmp = allData[y][x].getpPos();
                U8 tmpnum = allData[y][x].getpPosStatus();
                if(tmp.first == Down)//入点
                    allData[y][x].setRGB(0,255,0);
                else if(tmp.first == Up)//出点
                    allData[y][x].setRGB(255,0,0);
                else
                */
                _Data[y][x].setRGB(0,0,255);
            }
            if(_Data[y][x].getEdge() == -2)
                _Data[y][x].setRGB(0,255,0);
#endif
            /*
            int color = 128;
            allData[y][x].setRGB(color,color,color);
            int v = allData[y][x].getpPosValues();
            bool s = allData[y][x].getpPosStatus();
            if(s)
                allData[y][x].setRGB(0,255,0);
            */
#ifdef  AVG
            _Data[y][x].setRGB(0,avg*(i+2),0);
#endif
        }
#ifdef  START
        //get first point
        y = boundarys[i][0].getY()-_beginY;
        x = boundarys[i][0].getX()-_beginX;
        _Data[y][x].setRGB(255,255,255);
#endif
    }
#ifdef SKIP
    vector<limitXXY>::const_iterator it;
    for(it = skipTable.begin(); it != skipTable.end();++it)
    {
        allData[it->ally][it->sttx].setRGB(0,255,0);
        allData[it->ally][it->endx].setRGB(0,255,0);
    }
#endif
#endif
    return true;
}

bool     dataPcer::cutOut(vTrackTable& Tables)
{
    getBoundarys();
    ppPIXELS tmpData = NULL;
    if(dataDup2(_Data,tmpData))
    {
        clearData(_Data,_height);
    }
    size_t skipTablesLen  = frames.size();//boundary line's number
    for (size_t i = 0; i < skipTablesLen ; i++)
    {
        cutAOut(tmpData,Tables[i],frames[i]);
    }
    delData(tmpData,_height);
    return true;
}

bool     dataPcer::locateFrame()
{
    RGBQUAD red;
    red.setRGBA(255,0,0);
    RGBQUAD green;
    green.setRGBA(0,255,0);
    //drawRect(20,20,100,100,1);
    //drawLine(45,189,33,279,1);
    //drawLine(0,0,D2R(45),100);
    //drawLine(1,2,100,2,2);
    //drawLine(0,0,D2R(45),100);
    getBoundarys();
    clearData(_Data,_height);
    if(frames.empty())
        return false;
    size_t skipTablesLen  = frames.size();//boundary line's number
    PRs prs = EVER;
    for (size_t i = 0; i < skipTablesLen ; i++)
    {
        prs = checkPR(frames[i],frames[i+1]);
        printf("checkPR:%d\n",prs);
        if(i&1)
        {
            drawRect(frames[i],1,&red);
        }else
        {
            drawRect(frames[i],1,&green);
        }
    }
    return true;
}

bool     dataPcer::locateMove()
{
    getBoundarys();
    mvect mv(15,15);
    moveLine(boundarys[1],mv);
    //moveBoundry(trackTables[1],mv);
}

bool     dataPcer::inTrackLine(vTrackLine line,int x)
{
    size_t len  = line.size();
    if(line.empty())
        return false;
    if(x < line.front().sttx || x > line.back().endx)
    {
        return false;
    }
    int high = len -1,low = 0;//假设数组是从小到大排列的
    int midle = len >> 1;
    while(high >= low)
    {
        midle = (high + low)>>1;
        if(line[midle].in(x))
            return true;
        else if(line[midle].lout(x))
            high = midle - 1;
        else
            low = midle + 1;
    }
    return false;
}

bool     dataPcer::cutAOut(ppPIXELS& imageData, TrackTable& table,FramePoint& FP)
{
    int len = table.size();
    vTrackLine line;
    for (int y = FP[0],i = 0; y <= FP[1] && i < len; y++)
    {
        line = table[i++];
        for (int x = FP[2]; x <= FP[3]; x++)
        {
            if(inTrackLine(line,x))//此处不需要传入y值得原因是：FramePoint的特性
            {
                _Data[y-_beginY][x-_beginX] = imageData[y-_beginY][x-_beginX];
            }
        }
    }
    return true;
}

bool     dataPcer::boundarysHL(vTrackTable& Tables)
{
    getBoundarys();
    size_t skipTablesLen  = Tables.size();//boundary line's number
    for (size_t i = 0; i < skipTablesLen; i++)
    {
        boundarylineHL(Tables[i]);
    }
    return true;
}

bool     dataPcer::boundarylineHL(TrackTable& table)
{
    size_t tablelen = table.size();
    vTrackLine line;
    size_t linelen = 0;
    int sx = 0,ex = 0,y = 0;
    for (size_t i = 0; i < tablelen; ++i)
    {
        line = table[i];
        linelen = line.size();
        for (size_t j = 0; j < linelen; ++j)
        {
            //line[k].show();
            sx = line[j].sttx;
            ex = line[j].endx;
            y = line[j].ally;
            if(sx == ex)
                _Data[y-_beginY][sx-_beginX].setRGB(0,255,0);
            else
            {
                _Data[y-_beginY][sx-_beginX].setRGB(255,255,0);
                _Data[y-_beginY][ex-_beginX].setRGB(0,255,255);
            }
        }
    }
    return true;
}

bool     dataPcer::ImageHL()
{
    for (int i = 0; i < _width; i++)
    {
        for (int j = 0; j < _height; j ++)
        {
            if(_Data[j][i].getEdge() < 0)
                _Data[j][i].setRGB(0,255,0);
        }
    }
    return true;
}

/*
 * dataPcer utils for deal with function
 *
 */

bool     dataPcer::newData(ppPIXELS& imageData, int W, int H)
{
    // malloc some memroy
    // imageData = new pPIXELS[W];
    imageData = (ppPIXELS)malloc(H*sizeof(pPIXELS));
    if(!imageData)
    {
        printf("In %s new imageData wrong!\n",__FUNCTION__);
        return false;
    }
    // printf("new:(W,H):%d,%d\n",W,H);
    int y;
    for (y = 0; y < H; y++)
    {
        // _Data[y] = new PIXELS[W];
        imageData[y] = (pPIXELS)malloc(W*sizeof(PIXELS));
        if (!imageData[y])
        {
            printf("In %s new _Data[y] wrong!\n",__FUNCTION__);
            return false;
        }
    }
    return true;
}

bool     dataPcer::dataDup2(ppPIXELS& imageData, ppPIXELS &tmpData)
{
    // tmpData = new pPIXELS[_width];
    tmpData = (ppPIXELS)calloc(_height, sizeof(PIXELS));
    if(!tmpData)
    {
        printf("In %s calloc memory is failed\n",__FUNCTION__);
        return false;
    }
    for (int y = 0; y < _height; y++)
    {
        // tmpData[y] = new PIXELS[_width];
        tmpData[y] = (pPIXELS)calloc(_width, sizeof(PIXELS));
        if(!tmpData[y])
        {
            printf("In %s[y] calloc memory is failedp\n",__FUNCTION__);
            return false;
        }
        memcpy(tmpData[y], imageData[y], sizeof(PIXELS) * _width);
    }
    return true;
}

bool     dataPcer::delData(ppPIXELS& imageData, int H)
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
        imageData = NULL;
    }
    else
    {
        //printf("In delData imageData = NULL\n");
    }
    return true;
}

bool     dataPcer::clearData(ppPIXELS& imageData, int H)
{
    //printf("H:%d %p\n",H,imageData);
    if (!imageData)
        return false;
    for (int i = 0; i < H; i++)
    {
        memset(imageData[i],255,sizeof(PIXELS)*_width);
    }
    return true;
}

bool     dataPcer::isEdge(int x,int y)
{
    if(x <= 0 || x >= _width ||
            y <= 0 || y >= _height)
        return true;
    else
        return false;
}

bool     dataPcer::drawLine(int x,int y,double delta,int length,int size/* = 1*/,RGBQUAD* rgb /*= NULL*/)throw(ImgException)
{
    try
    {
        int ey = y + length*sin(delta);
        int ex = x + length*cos(delta);
        drawLine(x,y,ex,ey,size,rgb);
    }
    catch(ImgException& e)
    {
        return false;
    }
    return true;
}

bool     dataPcer::drawRect(int x,int y,int width,int height,int size/* = 1*/)
{
#define RESET(_T,_FLAG) (((_T) < 0) ? 0 : (((_T) > (_FLAG)) ? (_FLAG) : (_T)))
    int start = 0;
    try
    {
        drawLine(x,y,D2R(0),width,size);
        drawLine(x,y,D2R(90),height,size);
        start = y + height;
        start = RESET(start,_height-1);
        drawLine(x,start,D2R(0),width,size);
        start = x + width;
        start = RESET(start,_width-1);
        drawLine(start,y,D2R(90),height,size);
    }
    catch(ImgException& e)
    {
        return false;
    }
    return true;
#undef RESET
}

bool     dataPcer::drawRect(const FramePoint& FP,int size/* = 1*/,RGBQUAD* rgb /*= NULL*/)
{
    try
    {
        drawLine(FP[2],FP[0],FP[3],FP[0],size,rgb);
        drawLine(FP[2],FP[1],FP[3],FP[1],size,rgb);
        drawLine(FP[2],FP[0],FP[2],FP[1],size,rgb);
        drawLine(FP[3],FP[0],FP[3],FP[1],size,rgb);
    }
    catch(ImgException& e)
    {
        return false;
    }
    return true;
}

//void     dataPcer::drawLine(int startX, int startY, int endX,int endY,int size,RGBQUAD* rgb /*= NULL*/)throw(ImgException)
/*{
    if(size < 0)
    {
        throw("error paint size\n");
    }
#define SWAP(I,J) (I=I+J,J=I-J,I=I-J)
    RGBQUAD tmp;
    if(rgb)
    {
        tmp = *rgb;
    }else
    {
        tmp.setRGBA(0,255,0);
    }
    if(endX < startX)
    {
        SWAP(endX,startX);
        SWAP(endY,startY);
    }
    int x = startX,y = startY;
    if(startY == endY)
    {
        //平行x轴
        while(x <= endX)
        {
            _Data[y][x].setRGB(tmp);
            x++;
        }
        return ;
    }
    if(startX == endX)
    {
        //平行y轴
        while (y <= endY)
        {
            _Data[y][x].setRGB(tmp);
            y++;
        }
        return ;
    }
    int deltaX = endX - startX,deltaY = endY - startY;
    int p = 0;
    int twoDy = deltaY << 1,twoMinusDx = (deltaY-deltaX) << 1,
        twoDx = deltaX << 1,twoMinusDy = (deltaX-deltaY) << 1;
    int twoSum= (deltaY + deltaX) << 1;
    double k = (double)deltaY / (double)deltaX;

    //0 < k < 1的情况
    if(k < 1.0 && k > 0.0)
    {
        p = (deltaY << 1) - deltaX;
        while(x < endX)
        {
            _Data[y][x].setRGB(tmp);
            x++;
            if(p < 0)
                p += twoDy;
            else
            {
                y++;
                p += twoMinusDx;
            }
        }
    }
    //k>=1的情况
    if(k >= 1.0)
    {
        p = deltaY;
        while(y < endY)
        {
            _Data[y][x].setRGB(tmp);
            y++;
            if(p < 0)
                p += twoDx;
            else
            {
                x++;
                p += twoMinusDy;
            }
        }
    }
    //0>k>-1的情况
    if(k > -1 && k < 0)
    {
        p = (deltaY << 1) + deltaX;
        while(x < endX)
        {
            _Data[y][x].setRGB(tmp);
            x++;
            if(p>=0)
                p += twoDy;
            else
            {
                y--;
                p += twoSum;
            }
        }
    }
    //k < -1的情况
    if(k <= -1)
    {
        p = (deltaX << 1) - deltaY;
        while(y > endY)
        {
            _Data[y][x].setRGB(tmp);
            y--;
            if(p >= 0)
                p -= twoDx;
            else
            {
                x++;
                p -= twoSum;
            }
        }
    }
#undef SWAP
}*/

void     dataPcer::drawLine(int x1, int y1, int x2, int y2,int size, RGBQUAD* rgb /*= NULL*/) throw(ImgException)
{
    if(size < 0)
    {
        throw("error paint size\n");
    }
    RGBQUAD tmp;
    if(rgb)
    {
        tmp = *rgb;
    }else
    {
     tmp.setRGBA(0,255,0);
    }
    //(!rgb) ? (tmp = *rgb) : (tmp.setRGBA(0,255,0));
    int x = x1,y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int s1 = (x2 > x1) ? 1 : -1;
    int s2 = (y2 > y1) ? 1 : -1;
    bool interchange = false;	// 默认不互换 dx、dy
    if (dy > dx)				// 当斜率大于 1 时，dx、dy 互换
    {
        int temp = dx;
        dx = dy;
        dy = temp;
        interchange = true;
    }
    int p = (dy << 1) - dx;
    for(int i = 0;i < size ;++i)
    {
        if(interchange)
        {
            (dx)?(++x1):(++y1);//参考上一版本的设计思路
        }else
        {
            (dx)?(++y1):(++x1);//参考上一版本的设计思路
        }
        for(int j = 0; j < dx; ++j)
        {
            _Data[y][x].setRGB(tmp);
            if (p >= 0)
            {
                if (!interchange)		// 当斜率 < 1 时，选取上下象素点
                    y += s2;
                else					// 当斜率 > 1 时，选取左右象素点
                    x += s1;
                p -= dx << 1;
            }
            if (!interchange)
                x += s1;				// 当斜率 < 1 时，选取 x 为步长
            else
                y += s2;				// 当斜率 > 1 时，选取 y 为步长
            p += dy << 1;
        }
        x = x1,y = y1;
    }
}

void     dataPcer::drawLine(int startX, int startY, int endX,int endY,RGBQUAD avgrgb,int size/* = 1*/)throw(ImgException)
{
    if(size < 0)
    {
        throw("error paint size\n");
    }
#define SWAP(I,J) ((I>J)?(I=I+J,J=I-J,I=I-J):(I=I,J=J))
    SWAP(startX,endX);
    SWAP(startY,endY);
    int deltaX = endX - startX;
    int deltaY = endY - startY;
    int e = 0;
    for(int i = 0;i < size ;++i)
    {
        if(!deltaX)//Vertical ,x not change
        {
            for (int y = startY; y <= endY; ++y)
            {
                _Data[y][startX] = _Data[y][startX] + avgrgb;
            }
            startX++;
        }
        else if(!deltaY)//Horizontal ,y not change
        {
            for (int x = startX; x <= endX; ++x)
            {
                _Data[startY][x] =  _Data[startY][x] + avgrgb;
            }
            startY++;
        }
        else
        {
            for (int x = startX, y = startY; x <= endX; ++x)
            {
                //e2 =  e1 + 2 * deltaY – 2 * deltaX * (y2 – y1)
                //当e1 + 2 * deltaY > deltaX时y2 = y1 + 1, 否则y2 = y1
                e += (deltaY << 1);
                if (e > deltaX)
                {
                    e -= (deltaX << 1);
                    ++y;
                }
                _Data[y][x] =  _Data[y][x] + avgrgb;
            }
            startY++;
        }
    }
}

PRs      dataPcer::checkPR(const FramePoint& A,const FramePoint& B)
{
      int a_cx,a_cy; /* 第一个中心点*/
      int a_cw,a_ch; /* 第一个矩形宽高的一半*/
      int b_cx,b_cy; /* 第二个中心点*/
      int b_cw,b_ch; /* 第二个矩形宽高的一半*/
      a_cy = (A[0] + A[1]) >> 1;
      a_ch = (A[1] - A[0]) >> 1;
      a_cx = (A[2] + A[3]) >> 1;
      a_cw = (A[3] - A[2]) >> 1;

      b_cy = (B[0] + B[1]) >> 1;
      b_ch = (B[1] - B[0]) >> 1;
      b_cx = (B[2] + B[3]) >> 1;
      b_cw = (B[3] - B[2]) >> 1;

      int DisX = abs(a_cx - b_cx);
      int DisY = abs(a_cy - b_cy);
      if(DisX <= (a_cw + b_cw) && DisY <= (a_ch + b_ch)) //cross
      {
          if(A[0] <= B[0])
          {
              if(A[2] <= B[2] && A[1] >= B[1] && A[3] >= B[3])
                  return IN;
              else
                  return CROSS;
          }else
          {
              if(A[2] >= B[2] && A[1] <= B[1] && A[3] <= B[3])
                  return IN;
              else
                  return CROSS;
          }
      }else
      {
          if((DisX - a_cw - b_cw) < 5 || (DisY - a_ch - b_ch) < 5)
              return NEAR;
          return OUT;
      }
      return EVER;
}

bool     dataPcer::BrainRepair(PIXELS& A,PIXELS& B)
{
    int range = min(abs(A.getX()-B.getX()),abs(A.getY()-B.getY()));
    RGBQUAD avg = ((A - B)/(float)range).getRGB();
    drawLine(A.getX(),A.getY(),B.getX(),B.getY(),avg,3);
    return true;
}

bool     dataPcer::BrainRepair(PIXELS& A,bool VorH,float range/* = 1*/)
{
    if(!range)
        return false;
    int x = A.getX();
    int y = A.getY();
    RGBQUAD avg;
    int range2 = (int)range;
    if(VorH)
    {
        avg = ((A - _Data[y+range2][x])/range).getRGB();
        drawLine(x,y,x,y+range2,avg,1);
    }
    else
    {
        avg = ((A - _Data[y][x+range2])/range).getRGB();
        drawLine(x,y,x+range2,y,avg,1);
    }
    return true;
}

bool     dataPcer::moveLine(dPIXELS& line,mvect where,bool isOverWrite/* = false*/)
{
#define REPAIR(_x,_X) ((_x < 0) ? (0) : ((_x >= _X) ? (_X-1) : (_x)))
    if(line.empty())
        return false;
    int len = line.size();
    if(!isOverWrite)
    {
        int xx = 0,yy = 0;
        for(int i = 0;i < len;++i)
        {
            xx = REPAIR(line[i].getX()-_beginX + where._x, _width);
            yy = REPAIR(line[i].getY()-_beginY + where._y, _height);
            _Data[yy][xx] = line[i];
            line[i].setRGB(0,255,0);
        }
        return true;
    }
    else
    {
        int xx = 0,yy = 0;
        int x = 0, y = 0;
        RGBQUAD color;
        //int i = 0;
        for(int i = 0;i < len;++i)
        {
            color = line[i].getRGB();
            x = line[i].getX();
            y = line[i].getY();
            xx = REPAIR(x -_beginX + where._x, _width);
            yy = REPAIR(y -_beginY + where._y, _height);
            drawLine(x,y,xx,yy,1,&color);
            //_Data[yy][xx] = line[i];
            //line[i].setRGB(0,255,0);
        }
        return true;
    }
#undef REPAIR
}

bool     dataPcer::moveBoundry(TrackTable& table,mvect where,bool isOverWrite/* = false*/)
{
#define REPAIR(_x,_X) ((_x < 0) ? (0) : ((_x >= _X) ? (_X-1) : (_x)))
    if(table.empty())
        return false;
    int len = table.size();
    int linelen = 0;
    vTrackLine line;
    limitXXY   range;
    int x = 0,y = 0;
    if(!isOverWrite)
    {
        for(int i = 0;i < len; ++i)
        {
            line = table[i];
            linelen = line.size();
            if(where._y < 0)
            {
                for(int j = 0;j < linelen;++j)
                {
                    range = line[j];
                    for(x = range.sttx,y = range.ally;x <= range.endx;++x)
                    {
                        _Data[y+where._y][x+where._x] = _Data[y][x];
                        _Data[y][x].setRGB(_Data[range.ally][range.sttx-1]);
                    }
                }
            }
            else
            {
                for(int j = linelen-1;j >= 0;--j)
                {
                    range = line[j];
                    for(x = range.sttx,y = range.ally;x <= range.endx;++x)
                    {
                        _Data[y+where._y][x+where._x] = _Data[y][x];
                        _Data[y][x].setRGB(255,255,255);
                    }
                }

            }
        }
        return true;
    }
    else
    {
        return true;
    }
#undef REPAIR
}

/*
 * dataPcer deal Manager and return Data
 *
 */

bool     dataPcer::dealManager(const char* dealType)
{
    if(!_Data)
    {
        printf("cannot deal with,there are not Data!\n");
        return false;
    }
    if(!dealType)
    {
        printf("Not deal with!,will output itself\n");
        return true;
    }
    while(*dealType)
    {
        switch(*dealType)
        {
        case 'T':
            cout << "  -T     imageTranspose  : Transpose a iamge\n";
           transpose(_Data);
            break;
        case 'R':
            cout << "  -R     imageRevolution : Revolution a image\n";
            revolution(_Data,_width/2,_height/2,-45);
            break;
        case 's':
            cout << "  -s     imageSpherize   : Spherize a image\n";
            spherize(_Data);
            break;
        case 'Z':
            cout << "  -Z     imageZoom       : zoom a image\n";
            zoom(_Data,0.5,0.5);
            break;
        case 'M':
            cout << "  -M     imageMirror     : Mirror a image\n";
            mirror(_Data,UD);
            break;
        case 'S':
            cout << "  -S     imageShear      : Shear a image\n";
            shear(_Data,true,-45.0);
            break;
        case 'm':
            cout << "  -m     imageMove       : move a image\n";
            move(_Data,100,100);
            break;
        case 'C':
            cout << "  -C     getImage3Color  : get a image's 3(R,G,B) color image\n";
            getImage3Color(_Data,Green);
            break;
        case 'H':
            cout << "  -H     genHistogram    : get a image's 3(R,G,B) color Histogram\n";
            genBardiagram(_Data,Red);
            //genBardiagram(_Data,Green);
            //genBardiagram(_Data,Blue);
            //genBardiagram(_Data);
            break;
        case 'B':
            cout << "  -B     genBardiagram   : get a image's 3(R,G,B) color Bar diagram\n";
            //genHistogram(_Data,Red);
            genHistogram(_Data,Green);
            //genHistogram(_Data,Blue);
            //genHistogram(_Data);
            break;
        case 'b':
            cout << "  -b     backGround_ize   : get a image's part of backGround\n";
            //backGround_ize();
            break;
		case 'g':
            cout << "  -g     boundarysHL      : change boundarys line to HightLight\n";
            boundarysHL(trackTables);
            //boundarysHL();
            break;
        case 'h':
            cout << "  -h     boundarysHL      : change boundarys line to HightLight\n";
            getBoundarys();
            break;
        case 'c':
            cout << "  -c     imageCutOut      : cutout the effective part of the image\n";
            cutOut(trackTables);
            break;
        case 'l':
            cout << "  -l     locateFrame      : HightLight the every frames\n";
            locateFrame();
            break;
        case 'L':
            cout << "  -L     locateMove       : Move the part of image:like boundarysline or boundary\n";
            locateMove();
            //BrainRepair(_Data[48][66],_Data[88][96]);
            break;
        case 'd':
            cout << "  -d     imageDensity     : Change a image each pixel's Idensity\n";
            density(_Data,200);
            break;
        default:
            printf("Not deal with!\n");
            break;
        }
        dealType++;
    }
    //set data
    if(_dp)
    {
        _dp->setData(_Data);
        _dp->setWidth(_width);
        _dp->setHeight(_height);
    }
    return true;
}

int      dataPcer::retnWidth()
{
    return _width;
}

int      dataPcer::retnHeight()
{
    return _height;
}

ppPIXELS dataPcer::retnData()
{
    return _Data;
}
