#include "ipoint.h"
#include <cmath>
#define LOS(RGBvar) ((ABS((char)RGBvar.rgbRed) <= 25) && \
	                  (ABS((char)RGBvar.rgbGreen) <= 25) &&\
	                  (ABS((char)RGBvar.rgbBlue) <= 25))        //level of similarity
PIXPOT::PIXPOT()
{
	//memset(this,0,sizeof(PIXPOT));
}

void PIXPOT::fix_PIXPOT(PIXPOT& pots8,int W,int H)
{
	int i = 0;
	while(i<4)
	{
		pots8.pot4s[i].fix_PIXELS(W,H);
		pots8.pot4a[i].fix_PIXELS(W,H);
		i++;
	}
}

void PIXPOT::show_PIXPOT8diffRGB(RGBQUAD diffRgb)
{
	printf("diff:(%03d,%03d,%03d)\taverage:%-3.3f\t",
		(char)diffRgb.rgbRed,
		(char)diffRgb.rgbGreen,
		(char)diffRgb.rgbBlue,
		((char)diffRgb.rgbBlue + (char)diffRgb.rgbGreen + (char)diffRgb.rgbRed)/3.0);
	char min = MIN(MIN((char)diffRgb.rgbRed,(char)diffRgb.rgbGreen),(char)diffRgb.rgbBlue);
	char max = MAX(MAX((char)diffRgb.rgbRed,(char)diffRgb.rgbGreen),(char)diffRgb.rgbBlue);
	char diff;
	if((min < 0 && max < 0))
		diff = min - 0;
	else if( (min > 0 && max > 0))
		diff = 0 - max;
	else
	{
		diff = min - max;
	}
	printf("色差:%d\n",ABS(diff));
}

PIXPOT PIXPOT::set_pots8(PIXELS* pos8)
{
	pot  = pos8[0];
	int i = 0;
	while(i< 4)
	{
		pot4s[i] = pos8[i+1];
		diff4s[i] = pot4s[i].get_diff8RGB(pos8[0]);
		pot4a[i] = pos8[i+5];
		diff4a[i] = pot4a[i].get_diff8RGB(pos8[0]);
		i++;
	}
	//show_PIXPOT();
	return *this;
}
void PIXPOT::show_PIXPOT()
{
	pot.show_PIXELS();
	printf("\n");
	int i = 0;
	while(i<4)
	{
		pot4s[i].show_PIXELS();
		show_PIXPOT8diffRGB(diff4s[i]);
		pot4a[i].show_PIXELS();
		show_PIXPOT8diffRGB(diff4a[i]);
		i++;
	}
}
//get the diff value 4 side point
RGBQUAD PIXPOT::get_diff4s(int indexs)
{
	RGBQUAD tmp;
	memset(&tmp,255,sizeof(RGBQUAD));
	if(indexs < 0 || indexs > 3)
		return tmp;
	else
		return diff4s[indexs]; 
}
//get the diff value 4 angle point
RGBQUAD PIXPOT::get_diff4a(int indexa)
{
	RGBQUAD tmp;
	memset(&tmp,255,sizeof(RGBQUAD));
	if(indexa < 0 || indexa > 3)
		return tmp;
	else
		return diff4a[indexa]; 
}
//get 8 point position(x,y)
PIXELS* PIXPOT::get_pos8(PIXELS pixel,PIXELS* pos8,int W,int H)
{
	int i = 0;
	while(i<4)
	{
		//get 4 side point position(x,y)
		pot4s[i] = pixel;
		//get 4 angle point position(x,y)
		pot4a[i] = pixel;
		i++;
	}
	pot4s[0].resetXY(0,-1);
	pot4s[1].resetXY(1,0);
	pot4s[2].resetXY(0,1);
	pot4s[3].resetXY(-1,0);

	pot4a[0].resetXY(-1,-1);
	pot4a[1].resetXY(1,-1);
	pot4a[2].resetXY(1,1);
	pot4a[3].resetXY(-1,1);
	fix_PIXPOT(*this,W,H);
	pos8[0] = pixel;
//	pos8[0].show_PIXELS();
//	printf("\n");
	i = 1;
	while(i < 5)
	{
		pos8[i] = pot4s[i-1];
//		pos8[i].show_PIXELS();
//		printf("\n");
		pos8[i+4] = pot4a[i-1];
//		pos8[i+4].show_PIXELS();
//		printf("\n");
		i++;
	}
//	show_PIXPOT();
	return pos8;
}
bool PIXPOT::pixelSimilar()
{
	int level = 0;
	int i = 0;
	while(i<4)
	{
		if(LOS(diff4a[i]))
			level++;
		if( LOS(diff4s[i]))
			level++;
		i++;
	}
	printf("level:%d\n",level);
	if(level >= 6)
		return true;
	else
		return false;
}

PIXELS::PIXELS():pix_X(0),pix_Y(0),rgb_threshold(128),bEdge(0),bEmpty(false),pix_P(None,0)
{
	memset(&prgb,0,sizeof(RGBQUAD));
	//rgb_threshold  = 0;
}
PIXELS::PIXELS(const RGBQUAD& rgb):pix_X(0),pix_Y(0),rgb_threshold(128),bEdge(0),bEmpty(false),pix_P(None,0)
{
	prgb.rgbBlue = rgb.rgbBlue;
	prgb.rgbGreen = rgb.rgbGreen;
	prgb.rgbRed = rgb.rgbRed;
	prgb.rgbReserved = rgb.rgbReserved;
}

//mix two color
PIXELS PIXELS::mix(PIXELS& ppot1,PIXELS& ppot2,U8 weight)
{
	PIXELS ppot3;
	ppot3.prgb.rgbRed = (((ppot1.prgb.rgbRed * weight) + (ppot1.prgb.rgbRed *(1 - weight))) >> 2)/100;
	ppot3.prgb.rgbGreen = (((ppot1.prgb.rgbGreen * weight) + (ppot1.prgb.rgbGreen *(1 - weight))) >> 2)/100;
	ppot3.prgb.rgbBlue = (((ppot1.prgb.rgbBlue * weight) + (ppot1.prgb.rgbBlue *(1 - weight))) >> 2)/100;
	ppot3.prgb.rgbReserved = (((ppot1.prgb.rgbReserved * weight) + (ppot1.prgb.rgbReserved *(1 - weight))) >> 2)/100;
	return ppot3;
}
//opposition the point color
PIXELS PIXELS::opposition(PIXELS& ppot)
{
	ppot.prgb.rgbRed = 255 - ppot.prgb.rgbRed;
	ppot.prgb.rgbGreen = 255 - ppot.prgb.rgbGreen;
	ppot.prgb.rgbBlue = 255 - ppot.prgb.rgbBlue;
	return ppot;
}
PIXELS PIXELS::opposition()
{
	prgb.rgbRed = 255 - prgb.rgbRed;
	prgb.rgbGreen = 255 - prgb.rgbGreen;
	prgb.rgbBlue = 255 - prgb.rgbBlue;
	return *this;
}
//get a threshold from a point ppot
U8 PIXELS::set_threshold(PIXELS ppot)
{
	U8 r = ppot.prgb.rgbRed * 0.3;
	U8 g = ppot.prgb.rgbGreen * 0.59;
	U8 b = ppot.prgb.rgbBlue * 0.11;
	rgb_threshold = (r + g + b)/3;
	return rgb_threshold;
}
//binaryzation image with a threshold
void PIXELS::toBin()
{
	U8 r = prgb.rgbRed * 0.3;
	U8 g = prgb.rgbGreen * 0.59;
	U8 b = prgb.rgbBlue * 0.11;
	U8 rgb = (r + g + b)/3;
	if(rgb >= rgb_threshold)
	{
		rgb = 255;
	}
	else
		rgb = 0;
	setRGB(rgb,rgb,rgb);
}
PIXELS PIXELS::toBin(PIXELS& ppot)
{
	ppot.toBin();
	return ppot;
}
PIXELS PIXELS::get3Color(colorType color)
{
	switch(color)
	{
		case Red:
			prgb.rgbGreen = 0;
			prgb.rgbBlue = 0;
			break;
		case Green:
			prgb.rgbRed = 0;
			prgb.rgbBlue = 0;
			break;
		case Blue:
			prgb.rgbRed = 0;
			prgb.rgbGreen = 0;
			break;
		default:
			break;
	}
	return *this;
}

//get the diff RGB between the focus point with other 8 point
RGBQUAD PIXELS::get_diff8RGB(PIXELS pixel)
{
	RGBQUAD diffRgb;
	diffRgb.rgbRed = prgb.rgbRed - pixel.prgb.rgbRed;
	diffRgb.rgbGreen = prgb.rgbGreen - pixel.prgb.rgbGreen;
	diffRgb.rgbBlue = prgb.rgbBlue - pixel.prgb.rgbBlue;
	diffRgb.rgbReserved = prgb.rgbReserved - pixel.prgb.rgbReserved;
	return diffRgb;
}

bool PIXELS::setData(BYTE8& b,BYTE8& g,BYTE8& r)
{
	b = prgb.rgbBlue;
	g = prgb.rgbGreen;
	r = prgb.rgbRed;
	return true;
}

//set rgb with r g b
PIXELS PIXELS::setRGB(U8 b,U8 g,U8 r)
//PIXELS PIXELS::setRGB(U8 r,U8 g,U8 b)
{
	prgb.rgbRed = r;
	prgb.rgbGreen = g;
	prgb.rgbBlue = b;
	return *this;
}
//set rgb with RGBQUAD
PIXELS PIXELS::setRGB(RGBQUAD rgb)
{
	prgb.rgbRed = rgb.rgbRed;
	prgb.rgbGreen = rgb.rgbGreen;
	prgb.rgbBlue = rgb.rgbBlue;
	prgb.rgbReserved = rgb.rgbReserved;
	return *this;
}
//set rgb with PIXELS
PIXELS PIXELS::setRGB(PIXELS ppot)
{
	prgb.rgbRed = ppot.prgb.rgbRed;
	prgb.rgbGreen = ppot.prgb.rgbGreen;
	prgb.rgbBlue = ppot.prgb.rgbBlue;
	prgb.rgbReserved = ppot.prgb.rgbReserved;
	return *this;
}
void PIXELS::setempty(bool state)
{
	bEmpty = state;
}
void PIXELS::setEdge(int bedge)
{
	bEdge = bedge;
}
void PIXELS::initpPos()
{
	pix_P.first = None;
	pix_P.second = 0;
}
void PIXELS::setpPos(Position pos)
{
	pix_P.first = pos;
	pix_P.second += 1;
}
void PIXELS::setpPosStatus(bool status)
{
	if(status)
		pix_P.second |= 0x80;
	else
		pix_P.second &= 0x7F;
}
bool PIXELS::empty()
{
	return bEmpty;
}
bool PIXELS::isEdge(PIXELS& pixel, int W,int H)
{
	if((pixel.pix_X <= 0) | (pixel.pix_X >= W-1) |
			(pixel.pix_Y <= 0) | (pixel.pix_Y >= H-1))
	{
		pixel.bEdge = 1;
		return true;
	}
	else
	{
		pixel.bEdge = 0;
		return false;
	}
}
bool PIXELS::isEdge(int W,int H)
{
	if((pix_X <= 0) | (pix_X >= W-1) |
			(pix_Y <= 0) | (pix_Y >= H-1))
	{
		bEdge = 1;
		return true;
	}
	else
	{
		bEdge = 0;
		return false;
	}
}
void PIXELS::show_PIXELS()
{
	printf("X: %-3d Y: %-3d edge:%d\t",
			pix_X,
			pix_Y,
			bEdge);
	printf("[R,G,B]:(%03d,%03d,%03d)\t",
		prgb.rgbRed,
		prgb.rgbGreen,
		prgb.rgbBlue);
    printf("[Position,rtime]:(%s(%d),%d)\t",
        Pos2str(pix_P.first).c_str(),pix_P.first,pix_P.second & 0x7F);
}
//set (x,y) PIXELS
PIXELS PIXELS::setXY(PIXELS pixel)
{
	pix_X = pixel.pix_X;
	pix_Y = pixel.pix_Y;
	bEdge = pixel.bEdge;
	return *this;
}
//set (x,y) with x y
PIXELS PIXELS::setXY(int x,int y)
{
	pix_X = x;
	pix_Y = y;
	return *this;
}
PIXELS PIXELS::resetXY(int x,int y)
{
	pix_X += x;
	pix_Y += y;
	return *this;
}
int PIXELS::getX()const
{
	return pix_X;
}
int PIXELS::getY()const
{
	return pix_Y;
}
int PIXELS::getEdge()const
{
	return bEdge;
}
RGBQUAD PIXELS::getRGB()const
{
	return prgb;
}
PIXELS::pix_p PIXELS::getpPos()
{
	return pix_P;
}
bool PIXELS::getpPosStatus()
{
	return pix_P.second & 0x80;
}
int PIXELS::getpPosValues()
{
	return pix_P.second & 0x7F;
}
U8 PIXELS::getRed()const
{
	return prgb.rgbRed;
}
U8 PIXELS::getGreen()const
{
	return prgb.rgbGreen;
}
U8 PIXELS::getBlue()const
{
	return prgb.rgbBlue;
}
//just copy the position
PIXELS& PIXELS::operator=(const PIXELS& pixel)
{
	if(this == NULL)
		printf("kaokao\n");
	if(&pixel == NULL)
		printf("TMDTMD\n");
	if(this == &pixel)
		return *this;
	this->pix_X = pixel.pix_X;
	this->pix_Y = pixel.pix_Y;
	this->bEdge = pixel.bEdge;
	memcpy(&prgb,&pixel.prgb,sizeof(RGBQUAD));
	rgb_threshold = pixel.rgb_threshold;
	return *this;
}
//opposition the point color
PIXELS& PIXELS::operator~()
{
	prgb.rgbRed = 255 - prgb.rgbRed;
	prgb.rgbGreen = 255 - prgb.rgbGreen;
	prgb.rgbBlue = 255 - prgb.rgbBlue;
	prgb.rgbReserved = 255 - prgb.rgbReserved;
	return *this;
}
//just diff of two pixel
PIXELS PIXELS::operator-(const PIXELS& pixel)
{
	PIXELS diff;
	diff.pix_X = pix_X;
	diff.pix_Y = pix_Y;
	diff.prgb.rgbRed = abs(prgb.rgbRed - pixel.prgb.rgbRed);
	diff.prgb.rgbGreen = abs(prgb.rgbGreen - pixel.prgb.rgbGreen);
	diff.prgb.rgbBlue = abs(prgb.rgbBlue - pixel.prgb.rgbBlue);
	diff.prgb.rgbReserved = abs(prgb.rgbReserved - pixel.prgb.rgbReserved);
	return diff;
}
//just diff of two pixel
const PIXELS operator-(const PIXELS& pixel1,const PIXELS& pixel2)
{
	PIXELS diff;
	diff.pix_X = pixel1.pix_X;
	diff.pix_Y = pixel1.pix_Y;
	diff.prgb.rgbRed = abs(pixel1.prgb.rgbRed - pixel2.prgb.rgbRed);
	//printf("diff Red:%d\n",diff.prgb.rgbRed);
	diff.prgb.rgbGreen = abs(pixel1.prgb.rgbGreen - pixel2.prgb.rgbGreen);
	//printf("diff Green:%d\n",diff.prgb.rgbGreen);
	diff.prgb.rgbBlue = abs(pixel1.prgb.rgbBlue - pixel2.prgb.rgbBlue);
	//printf("diff Blue:%d\n",diff.prgb.rgbBlue);
	diff.prgb.rgbReserved = abs(pixel1.prgb.rgbReserved - pixel2.prgb.rgbReserved);
	return diff;
}

PIXELS PIXELS::operator*(const float& scale)
{
	prgb.rgbGreen *= scale;
	prgb.rgbRed *= scale;
	prgb.rgbBlue *= scale;
	return *this;
}

const PIXELS operator*(const float& scale,PIXELS& pixel)
{
	pixel.prgb.rgbRed *= scale;
	pixel.prgb.rgbGreen *= scale;
	pixel.prgb.rgbBlue *= scale;
	return pixel;
}
bool PIXELS::operator ==(const PIXELS& pixel)
{
	if(this == &pixel)
		return true;
	if(this->pix_Y == pixel.pix_Y &&
			this->pix_X == pixel.pix_X)
	{
		return true;
	}
	return false;
}
bool PIXELS::operator !=(const PIXELS& pixel)
{
	if(this == &pixel)
		return false;
	if(this->pix_Y != pixel.pix_Y ||
			this->pix_X != pixel.pix_X)
	{
		return true;
	}
	return false;
}

//fix up the point position,if the point is edge point 
void PIXELS::fix_PIXELS(int W,int H)
{
	if(isEdge(*this,W,H))
	{
		if(pix_X < 0)
			pix_X += W;
		if(pix_Y < 0)
			pix_Y += H;
		if(pix_X >= W)
			pix_X -= W;
		if(pix_Y >= H)
			pix_Y -= H;
	}
}