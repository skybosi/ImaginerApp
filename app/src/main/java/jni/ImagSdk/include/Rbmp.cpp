#include <unistd.h>
#include <math.h>
#include <stack>
#include "Rbmp.h"
#define PI 3.14159
#define R2D(R) R*180/PI
#define D2R(D) D*PI/180
#define OUTRANGE(P) ((P > 255) || (P < 0))
#define EQUALBackGround(rgb) ( !(rgb.rgbRed ^ backGround.rgbRed) && \
                                   !(rgb.rgbGreen ^ backGround.rgbGreen) && \
                                   !(rgb.rgbBlue ^ backGround.rgbBlue) )
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
#define SETPREV(_v,_prev) (ISV(_v) ? _prev.setEdge(-1): _prev.setEdge(-2))
#define SETCURR(_v,_curr) (_curr.setpPos(_v),(ISV(_v) ? _curr.setEdge(-1) : _curr.setEdge(-2)))
#define TMAX(_v1,_v2,_v3) (_v1 > _v2) ? ((_v1 > _v3) ? (_v1) : (_v3)) : ((_v2 > _v3) ? (_v2) : (_v3))
#define TMIN(_v1,_v2,_v3) (_v1 < _v2) ? ((_v1 < _v3) ? (_v1) : (_v3)) : ((_v2 < _v3) ? (_v2) : (_v3))
static int globalI = 0;
Rbmp::Rbmp(const char *bmpname):fp(NULL), fpo(NULL), bmppath(bmpname), allData(NULL), pColorTable(NULL),granularity(10),granOpeartor(true),baseSmlrty(1.0),testRange(2)
{
	// 二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(), "rb");
	if(!fp)
	{
		printf("image file cannot find!\n");
		exit(-1);
	}
	if (init_image())
	{
		printf("In %s init bmp image is OK!\n",__FUNCTION__);
	}
	else
	{
		printf("In %s init bmp image is failed!\n",__FUNCTION__);
		return;
	}
	cout << "create a Rbmp ....\n" << endl;
}
Rbmp::Rbmp(const char **bmpnamel):fp(NULL), fpo(NULL),bmppathl(bmpnamel),allData(NULL),pColorTable(NULL)
{
	cout << "create a Rbmp list ....\n" << endl;
}
bool Rbmp::init_image()
{
	if (NULL == fp)
	{
		printf("In %s open image is FAILED!\n",__FUNCTION__);
		return false;
	}
	U16 BM;
	fread(&BM, sizeof(U16), 1, fp);
	// printf("BBBBBBB:%0X\n",BM);
	if (BM != 0x4d42)
	{
		printf("In %s is not bmp image!\n",__FUNCTION__);
		return false;
	}
	rewind(fp);
	// 跳过位图文件头结构BITMAPFILEHEADER
	fread(&head, sizeof(BITMAPFILEHEADER), 1, fp);
	// printf("BITMAPFILEHEADER:%ld\n",sizeof(BITMAPFILEHEADER));
	bfOffBits = head.bfOffBits;

	// 位图信息头结构变量,存放在变量head中
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);
	// printf("BITMAPINFOHEADER:%ld\n",sizeof(BITMAPINFOHEADER));
	// 获取图像宽、高、每像素所占位数等信息
	bmpWidth = infohead.biWidth;
	bmpHeight = infohead.biHeight;
	biBitCount = infohead.biBitCount;
	allhead.bmpHead = head;
	allhead.infoHead = infohead;
	// 定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	// int lineByte = (bmpWidth * biBitCount/8+3)/4*4;
	// int lineByte = (bmpWidth * biBitCount + 31)/32*4;
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31) / 32 * 4;

	// 灰度图像有颜色表，且颜色表表项为256
	if (biBitCount == 8)
	{
		// 申请颜色表所需要的空间，读颜色表进内存
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
		/*
			 int i = 0; printf("颜色表:\n"); while(i < 256) { printf("$%d %3d
			 ",i,pColorTable[i]); printf("r:%3d ",pColorTable[i].rgbRed);
			 printf("g:%3d ",pColorTable[i].rgbGreen);
			 printf("b:%3d\n",pColorTable[i].rgbBlue); i++; } */
	}
	// printf("here1111:%ld\n",ftell(fp));
	// fseek(fp,bfOffBits,0); //左上原点
	BYTE8 *linedata = new BYTE8[lineByte];
	// allData = new pPIXELS[bmpWidth];
	allData = (ppPIXELS)calloc(bmpWidth, sizeof(PIXELS));
	int k = 0, x = 0, y = bmpHeight - 1;
	int tablesite;
	for (; y >= 0; y--)			// y is row number
	{
		// allData[y] = new PIXELS[bmpWidth];
		allData[y] = (pPIXELS)calloc(bmpWidth, sizeof(PIXELS));
		fread(linedata, 1, lineByte, fp);
		for (; x < bmpWidth /* k < lineByte */ ; k++, x++)
		{
			// printf("%03d ",linedata[k]);
			switch (biBitCount)
			{
				case 24:
					allData[y][x].setRGB(linedata[k], linedata[k + 1], linedata[k + 2]);
					// allData[y][x].setRGB(linedata[k+2],linedata[k+1],linedata[k]);
					allData[y][x].setXY(x, y);
					allData[y][x].isEdge(bmpWidth, bmpHeight);
					allData[y][x].setempty();
					allData[y][x].initpPos();
					k += 2;
					break;
				case 8:
					tablesite = linedata[k];
					// printf("%03d\n",linedata[k]);
					// printf("颜色表位置:%d\n",tablesite);
					allData[y][x].setRGB(pColorTable[tablesite]);
					allData[y][x].setXY(x, y);
					allData[y][x].isEdge(bmpWidth, bmpHeight);
					allData[y][x].setempty();
					allData[y][x].initpPos();
					break;
				default:
					break;
			}
			// allData[y][x].show_PIXELS();
			// printf("\n");
		}
		k = x = 0;
	}
	delete[]linedata;
	rewind(fp);
	if(!setBackground())//use default value
		printf("In %s set Background error!\n",__FUNCTION__);
	return true;
}
int Rbmp::alikeBackground(PIXELS pixel)
{
	if(pixel.empty())
		return -1;
	/*
	RGBQUAD rgb = pixel.getRGB();
	if(EQUALBackGround(rgb))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	PIXELS backGroundpixel(backGround);
	if(getSimilarity(backGroundpixel,pixel) > baseSmlrty)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int Rbmp::alikeBackground(int x,int y)
{
	PIXELS tmp = get_pix(x,y);
	if(tmp.empty())
		return -1;
	/*
	RGBQUAD rgb = tmp.getRGB();
	if(EQUALBackGround(rgb))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	PIXELS backGroundpixel(backGround);
	if(getSimilarity(backGroundpixel,tmp) > baseSmlrty)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool Rbmp::alikeBackground(RGBQUAD rgb)
{
	if(EQUALBackGround(rgb))
		return true;
	else
		return false;
}

bool Rbmp::initSpatialize(const char** imagePath)
{
	if(NULL == imagePath)
	{
		printf("In %s imagePath is NULL!\n",__FUNCTION__);
		return  false;
	}
	for(int i = 0; i < 6; ++i)
	{
		//int to enum Position
		if(strcmp(imagePath[i],""))
		{
			rbmp.insert(pair<Position, string>((Position)i, imagePath[i]));
		}
	}
	return true;
}

bool Rbmp::boundarysHL()
{
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
			y = boundarys[i][j].getY();
			x = boundarys[i][j].getX();
#ifdef  ONE
			//if(allData[y][x].getEdge() == -2)
			//allData[y][x].setRGB(255,0,0);
            if(allData[y][x].getEdge() == -1)
            {
                PIXELS::pix_p tmp = allData[y][x].getpPos();
                U8 tmpnum = allData[y][x].getpPosStatus();
                if(tmp.first == Down)//入点
                    allData[y][x].setRGB(0,255,0);
                else if(tmp.first == Up)//出点
                    allData[y][x].setRGB(255,0,0);
                else
                    allData[y][x].setRGB(0,0,255);
            }
#endif
#ifdef  TRI
            if(allData[y][x].getEdge() == -3)
            {
                allData[y][x].setRGB(0,0,255);
            }
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
            allData[y][x].setRGB(0,255,i*avg);
#endif
		}
#ifdef  START
        //get first point
		y = boundarys[i][0].getY();
		x = boundarys[i][0].getX();
		allData[y][x].setRGB(255,255,255);
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
bool Rbmp::imageCutOut()
{
#define BETWEEN(P1,P2) (((P1.sttx > P2->sttx) && (P1.endx < P2->endx)))
	setBackground(255,255,255);
	if(skipTable.empty())
		return false;
	int currAlly;
	stack<limitXXY> currTable;
	stack<limitXXY> mainTable;
	limitXXY topPoint;
	vector<limitXXY>::const_iterator it = skipTable.begin();
	for (int y = 0; y < bmpHeight; y++)
	{
		if(it->ally != y)
		{
			for (int x = 0; x < bmpWidth; x++)
			{
				allData[y][x].setRGB(backGround);
			}
		}
		else
		{
			currAlly = y;
			while(it->ally == currAlly)
			{
				if(!currTable.empty())
				{
					topPoint = currTable.top();
					while(BETWEEN(topPoint,it)) //test the top is between current point
					{
						currTable.pop();
						if(currTable.empty())
							break;
						topPoint = currTable.top();
					}
					currTable.push(*it);
				}
				else
					currTable.push(*it);
				it++;
			}
			while(!currTable.empty())
			{
				mainTable.push(currTable.top());
				currTable.pop();
			}
			topPoint = mainTable.top();
			mainTable.pop();
			for (int x = 0; x < bmpWidth; x++)
			{
				//set other part to backGround
				if(x < topPoint.sttx || x > topPoint.endx)
					allData[y][x].setRGB(backGround);
				//set the cutOut part to you color that you want
				//if(x >= it->sttx && x <= it->endx)
				//	allData[y][x].setRGB(0,0,255);
				//use and contrl the skipTable
				if(!mainTable.empty() && x == topPoint.endx )
				{
					topPoint = mainTable.top();
					mainTable.pop();
				}
			}
		}
	}
	return true;
}
Rbmp::~Rbmp()
{
	if (pColorTable && biBitCount == 8)
	{
		delete[]pColorTable;
		pColorTable = NULL;
	}
	/*
	if (allData)
	{
		for (int i = 0; i < bmpHeight; i++)
		{
			delete []imageData[i];
			free(allData[i]);
		}
		delete []imageData;
		free(allData);
		allData = NULL;
	}
	*/
	delImageData(allData,bmpHeight);
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (fpo)
	{
		fclose(fpo);
		fpo = NULL;
	}
	cout << "delete a Rbmp ...." <<  endl;
}
bool Rbmp::deleteAll()
{
	if (pColorTable && biBitCount == 8)
	{
		delete[]pColorTable;
		pColorTable = NULL;
	}
	if (allData)
	{
		for (int i = 0; i < bmpHeight; i++)
		{
			//delete []imageData[i];
			free(allData[i]);
		}
		//delete []imageData;
		free(allData);
		allData = NULL;
	}
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (fpo)
	{
		fclose(fpo);
		fpo = NULL;
	}
	cout << "delete all data...." <<  endl;
	return true;
}
int Rbmp::getW()
{
	return bmpWidth;
}

int Rbmp::getH()
{
	return bmpHeight;
}

PIXELS Rbmp::get_pix(int x, int y)
{
	//printf("In get_pix(x,y)\n");
	PIXELS ppot;
	ppot.setXY(x, y);
	if (out_range_error(ppot))
	{
		//printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", ppot.getX(), ppot.getY());
		ppot.setempty(true);
		return ppot;
	}
	ppot.setRGB(allData[y][x]);
	/*
	if(alikeBackground(ppot))
	{
		ppot.show_PIXELS();
		printf("\n");
	}
 */
	return ppot;
}

PIXELS Rbmp::get_pix(PIXELS pixel)
{
	PIXELS ppot;
	if (out_range_error(pixel))
	{
		//printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(), pixel.getY());
		ppot.setempty(true);
		return ppot;
	}
	ppot.setXY(pixel.getX(), pixel.getY());
	ppot.setRGB(allData[pixel.getY()][pixel.getX()]);
	ppot.show_PIXELS();
	printf("\n");
	return ppot;
}

// get the 8 point rgb value
PIXPOT Rbmp::get_pot(PIXELS pixel)
{
	PIXPOT pots8;
	pPIXELS pos8;
	try
	{
		if (out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pot ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(), pixel.getY());
		//return pots8;
	}
	pos8 = new PIXELS[9];
	//put the point and the 8 point arond into a array;
	//get the 8 point right position(x,y),if isEdge will be fixup
	pos8 = pots8.get_pos8(pixel, pos8, bmpWidth, bmpHeight);
	pos8[0] = get_pix(pos8[0]);
	//get the 8 point rgb value
	int i = 0;
	while (i < 4)
	{
		pos8[i + 1] = get_pix(pos8[i + 1]);
		pos8[i + 5] = get_pix(pos8[i + 5]);
		i++;
	}
	//set the 8 point message
	pots8.set_pots8(pos8);
	//pots8.show_PIXPOT();
	if (pos8)
	{
		delete[]pos8;
	}
	return pots8;
}

void Rbmp::show_allData()
{
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			allData[y][x].show_PIXELS();
			printf("\n");
		}
	}
}
bool Rbmp::getBoundaryLine(int& x, int& y)
{
	limitXXY footprint;
	int beforeX;
	beforeX = x;
	size_t bsize = boundarys.size();
	x = trackDown(allData[y][x]);
	if( boundarys.size() != bsize || x != beforeX)
	{
		footprint.add(allData[y][beforeX],allData[y][x],skipTable);
		return true;
	}
	return false;
	//printf("next footprint'x value:%d\n",x+1);
	//printf("trackDown.... insert\n");
}
void Rbmp::getBoundarys()
{
#define debug1
    PIXELS tmp;
	for (int y = 0;y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if(isBoundaryPoint(x,y))
            {
                tmp = allData[y][x];
                if(tmp.getEdge() >= 0)
                {
                    if(!getBoundaryLine(x,y))
                    {
                        //printf("getBoundaryLine flase\n");
                    }
                }
                else
                {

                    genSkipTable(allData[y][x]);
                }
                /*

//                PState pstate = getPointState(x,y);
//                switch (pstate)
//                {
//                case NORMAL:
//                    if(!getBoundaryLine(x,y) && !inX.empty())
//                    {
//                        beforeX = inX.top();
//                        footprint.add(allData[y][beforeX],allData[y][--x],skipTable);
//                        inX.pop();
//                    }
//                    break;
//                case INPOT:
//                    inX.push(x);
//                    break;
//                case OUTPOT:
//                    if(!inX.empty())
//                    {
//                        beforeX = inX.top();
//                        footprint.add(allData[y][beforeX],allData[y][--x],skipTable);
//                        inX.pop();
//                    }
//                    break;
//                case ONLYPOT:
//                    footprint.add(allData[y][x],allData[y][x],skipTable);
//                    break;
//                default:
//                    break;
//                }

				if(allData[y][x].getEdge() >= 0)
				{
					if(allData[y][x-1].getEdge() >= 0)
					{
						if(!getBoundaryLine(x,y) && !inX.empty())
						{
							beforeX = inX.top();
							footprint.add(allData[y][beforeX],allData[y][--x],skipTable);
							inX.pop();
						}
						else
						{
							inX.push(x);
						}
					}
					else
					{
						if(!inX.empty())
						{
							beforeX = inX.top();
							footprint.add(allData[y][beforeX],allData[y][--x],skipTable);
							inX.pop();
						}
					}
#ifdef debug
					goto here;
#endif
				}
				else
				{
					//PState pstate = getPointState(x,y);
					//get all cut point(not only point)
					if(alikeBackground(x+1,y))
						inX.push(x);
					else if(!alikeBackground(x-1,y) && !inX.empty())
					{
						beforeX = inX.top();
						footprint.add(allData[y][beforeX],allData[y][--x],skipTable);
						inX.pop();
					}
					else
					{
						footprint.add(allData[y][x],allData[y][x],skipTable);
					}
					//printf("skip table.... insert\n");
				}
				*/
			}
		}
	}
#ifdef debug
here:	printf("OOOOOOKKKKK!\n");
#endif
			printf("granularity: %u boundarys size:%ld\n",granularity,boundarys.size());
			printf("granularity: %u boundarys size:%ld\n",granularity,boundarys.size());
			printf("skip Table size:%ld\n",skipTable.size());
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
void Rbmp::show_line(dPIXELS boundaryline)
{
	for (size_t i =0; i < boundaryline.size(); i ++)
	{
		boundaryline[i].show_PIXELS();
		printf("\n");
	}
}
bool Rbmp::isCloseOpen(dPIXELS boundaryline)
{
	if(boundaryline.empty())
		return false;
	else
		return (boundaryline.front() == boundaryline.back());
}
//start track down by following clues(顺藤摸瓜)
int Rbmp::trackDown(PIXELS& startPoint)
{
	globalI++;
	bool openstatus = true;
	if(startPoint.getEdge() != 1)
		openstatus = !openstatus;
	int sx = startPoint.getX();
	int x = sx;
	int sy = startPoint.getY();
	int y = sy;
	//make sure not trackDown again
	if(!boundarys.empty() && testStartP(startPoint))
		return sx;
	int nextx = 0;
	dPIXELS boundaryline;
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
		return sx++;
	//startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
	SETCURR(Down,startPoint);
	startPoint.setpPosStatus();
	boundaryline.push_back(startPoint);
	/*
	printf("push s: ");
	startPoint.show_PIXELS();
	printf("\n");
 */
	SETCURR(direction,allData[y][x]);
	/*
	if(y != sy)
	{
		allData[y][x].setEdge(-1);
	}
	else
	{
		allData[y][x].setEdge(-2);
	}
	*/
	boundaryline.push_back(allData[y][x]);
	Position prevDiret = direction;
	FramePoint framePoint(bmpHeight,bmpWidth);
	bool clean = false;
	while (x != sx || y != sy)
	{
		//if(getRpoint(direction,x,y)&& !isEdge(x,y))
		PIXELS& prevPoint = allData[y][x];
		prevDiret = direction;
		//printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
		if(getRpoint(direction,x,y))
		{
			/*
			if (x < sx && y < sy)
			{
				clean = true;
				break;
			}
			*/
			if(prevDiret != direction)
				prevPoint.setpPosStatus();
			if(prevDiret + direction == 3)
				prevDiret = direction;
			SETPREV(prevDiret,prevPoint);
			SETPREV(prevDiret,boundaryline.back());
			/*
			if(ISV(prevDiret))
				prevPoint.setEdge(-1);
			if(ISH(prevDiret))
				prevPoint.setEdge(-2);
			*/
			SETCURR(direction,allData[y][x]);
			/*
			if(ISV(direction))
			{
				if(allData[y][x].getEdge() == -1)
					allData[y][x].setEdge(-3);
				else
					allData[y][x].setEdge(-1);
			}
			if(ISH(direction))
				allData[y][x].setEdge(-2);
			*/
			if(prevDiret + direction == 5)
				prevPoint.setEdge(-1);
			// framepoint[direction] = allData[y][x];
			framePoint.setframePoint(direction,allData[y][x]);
			boundaryline.push_back(allData[y][x]);
			/*
			printf("push a: ");
			get_pix(x,y).show_PIXELS();
			printf("\n");
			*/
		}
		else
		{
			openstatus = !openstatus;
			break;
		}
	}
	if(openstatus)
	{
		direction = Right;
		x = sx;
		y = sy;
		while (1)
		{
			PIXELS& prevPoint = allData[y][x];
			prevDiret = direction;
			//printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
			if(getLpoint(direction,x,y))
			{
				/*
				if (x < sx && y < sy)
				{
					clean = true;
					break;
				}
				*/
				if(prevDiret != direction)
					prevPoint.setpPosStatus();
				if((prevDiret != direction) &&
						(prevDiret + direction == 2 ||
						 prevDiret + direction == 4))
					prevDiret = direction;
				SETPREV(prevDiret,prevPoint);
				/*
				if(ISV(prevDiret))
					prevPoint.setEdge(-1);
				if(ISH(prevDiret))
					prevPoint.setEdge(-2);
				*/
				SETCURR(direction,allData[y][x]);
				/*
				if(ISV(direction))
					allData[y][x].setEdge(-1);
				if(ISH(direction))
					allData[y][x].setEdge(-2);
				if(prevDiret + direction == 5)
					prevPoint.setEdge(-1);
				*/
				framePoint.setframePoint(direction,allData[y][x]);
				boundaryline.push_front(allData[y][x]);
				/*
				printf("push a: ");
				get_pix(x,y).show_PIXELS();
				printf("\n");
				*/
				nextx++;
			}
			else
				break;//jump out while loop
		}
	}
	else
	{
		nextx =  boundaryline.size() - 1;
	}
	if(clean)
	{
		int xx = 0,yy = 0;
		while(!boundaryline.empty())//clean the smaller than the granularity data
		{
			xx = boundaryline.front().getX();
			yy = boundaryline.front().getY();
			allData[yy][xx].setEdge(0);
			allData[yy][xx].initpPos();
			boundaryline.pop_front();
		}
	}
    else  //deal with startPoint
    {
        startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
        startPoint.setpPos(Down);
    }
#define GRANOPERATION(size) (granOpeartor)?(size > granularity):(size <= granularity)
	if(GRANOPERATION(boundaryline.size()))
	{
		//show_line(boundaryline);
		//deburrTrack(boundaryline);
		boundarys.push_back(boundaryline);
		framePoint.setBindNum(boundarys.size());
		frames.push_back(framePoint);
	}
	else
	{
		nextx = 0;
		int xx = 0,yy = 0;
		while(!boundaryline.empty())//clean the smaller than the granularity data
		{
			xx = boundaryline.front().getX();
			yy = boundaryline.front().getY();
			allData[yy][xx].setEdge(0);
			allData[yy][xx].initpPos();
			boundaryline.pop_front();
		}
	}

	//printf("$[%d]> close or open status: %s Track down by following clues(顺藤摸瓜) OK... len:%ld(%u)\n",
	//globalI,CLOSEOPEN(isCloseOpen(boundaryline)),boundaryline.size(),granularity);
	//get next point's x value
	int NextX = sx;
	while(--nextx >= 0 && boundaryline[nextx].getY() == sy
			&& boundaryline[nextx].getX() >= NextX)
	{
		NextX = boundaryline[nextx].getX();
		//printf("%d mx %d\n",nextx,maxX);
	}
	//printf("%d mx %d\n",nextx,maxX);
	//printf("The max x %d will be nextpoint\n",maxX);
	return NextX;
}
bool Rbmp::deburrTrack(dPIXELS& boundaryline)
{
	printf("in deburrTrack...\n");
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
				y = (itPoint-j)->getY();
				x = (itPoint-j)->getX();
				printf("find back point! j:%d\n",j);
				//(itPoint-j)->setEdge(-3);
				boundaryline.erase(itPoint-j);
				allData[y][x].setEdge(-3);
				(itPoint-j)->show_PIXELS();
				printf("\n===============\n");
				y = (itPoint+j)->getY();
				x = (itPoint+j)->getX();
				(itPoint+j)->setEdge(-3);
				allData[y][x].setEdge(-3);
				(itPoint+j)->show_PIXELS();
				printf("\n");
				j++;
			}
			j = 1;
		}
	}
	return true;
}
/*
bool Rbmp::deburrTrack(dPIXELS& boundaryline)
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
				y = boundaryline[i-j].getY();
				x = boundaryline[i-j].getX();
				printf("find back point! j:%d\n",j);
				boundaryline[i-j].setEdge(-3);
				allData[y][x].setEdge(-3);
				boundaryline[i-j].show_PIXELS();
				printf("\n===============\n");
				y = boundaryline[i+j].getY();
				x = boundaryline[i+j].getX();
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
// is Boundary
bool Rbmp::isBoundaryPoint(PIXELS pot)
{
	/*
	PIXPOT tmp;
	if(alikeBackground(pot) == 1)
	{
		tmp = get_pot(pot);
		if(tmp.pixelSimilar())
		{
			tmp.show_PIXPOT();
			printf("相似度极高\n");
			printf("\n");
			return false;
		}
		else
		{
			tmp.show_PIXPOT();
			printf("相似度一般\n");
			printf("\n");
			return true;
		}
	}
	*/
	if(alikeBackground(pot) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
// is Boundary
bool Rbmp::isBoundaryPoint(int& x,int& y)
{
	int i = 0;
	float similarity = 1;
	//float checkSmlrty = 0;
	float avgSimi = 0;
	float diffSim = 0;
	for (i = 0; x < bmpWidth-1; ++i,++x)
	{
		diffSim  = similarity - avgSimi;
		similarity = getSimilarity(Right,x,y);
        if(allData[y][x].getEdge() == -1)
        {
            genSkipTable(allData[y][x]);
        }
		//printf("%3d: num:%lf\tavg:%lf\tdiff:%lf\n", i + 1, similarity, avgSimi, fabs(diffSim));
		avgSimi += diffSim / (i + 1);
		if (fabs(similarity - avgSimi) > 0.12)
		{
			++x;
			setBackground(allData[y][x]);
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
	if(x < bmpWidth-1)
	{
		return true;
	}
	else
		return false;
}
float Rbmp::getSimilarity(PIXELS backPoint, PIXELS currPoint)
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
	//       currPoint.getX(),currPoint.getY(),Similarity,SmlrtyR,SmlrtyG,SmlrtyB);
	//return TMAX(SmlrtyR,SmlrtyB,SmlrtyG);
	return Similarity;
}
float Rbmp::getSimilarity(Position direction,int x,int y,int step)
{
	float Similarity = 0;
	float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
	if(get_pix(x,y).empty())
		return -1;
	PIXELS potCurnt;
	PIXELS potRight;
	switch(direction)
	{
		case Down:
			if(y+step >= bmpHeight)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y+step][x];
			break;
		case Right:
			if(x+step >= bmpWidth)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y][x+step];
			break;
		case Up:
			if(y-step < 0)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y-step][x];
			break;
		case Left:
			if(x-step < 0)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y][x-step];
			break;
		default:
			break;
	}
	PIXELS diff = potCurnt - potRight;
	SmlrtyR = 1 - diff.getRed()/255.0;
	SmlrtyG = 1 - diff.getGreen()/255.0;
	SmlrtyB = 1 - diff.getBlue()/255.0;
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
// 函数名称readNline()
ppPIXELS Rbmp::readNline(int beginY, int rows)
{
#define DEAL(ROWS,Y) ((ROWS)>0?(Y++):(Y--))
	try
	{
		if ((beginY < 0) | (beginY >= bmpHeight))
			throw 0;
	}
	catch(...)
	{
		printf("In readNline ,You set beginY:%d is out Range!\n", beginY);
		//return NULL;
	}
	if (rows == 0)
	{
		return allData;
	}
	else
	{
		int rows2 = ABS(rows);
		if (beginY + rows <= 0)
		{
			rows2 = beginY + 1;	// dealwith rows2
			printf("You set rows2 change %d\n", rows2);
		}
		if (beginY + rows >= bmpHeight)
		{
			rows2 = bmpHeight - beginY;	// dealwith rows2
			printf("You set rows2 change %d\n", rows2);
		}
		int x, y = beginY;
		ppPIXELS lineppot = new pPIXELS[rows2];
		for (; rows2--; DEAL(rows, y))	// y is row number
		{
			lineppot[y] = new PIXELS[bmpWidth];
			for (x = 0; x < bmpWidth; x++)
			{
				lineppot[y][x] = allData[y][x];
				lineppot[y][x].show_PIXELS();
				printf("\n");
			}
			x = 0;
		}
		return lineppot;
	}
}

void Rbmp::delreadNline(ppPIXELS  lineppot, int rows)
{
	if (lineppot)
	{
		for (int i = 0; i < ABS(rows); i++)
			delete[]lineppot[i];
		delete[]lineppot;
		lineppot = NULL;
	}
}

bool Rbmp::write_image(const char *outpath,const char* dealType)
{
	// FILE* fpo = fopen(outpath,"wb");
	fpo = fopen(outpath, "wb");
	fseek(fpo, 54, 0);		// 跳过allhead
	//ppPIXELS imageData = NULL;
	//imageData = newImageData(imageData, bmpWidth, bmpHeight);
	if (deal_image(dealType))
	{
		writeAllData(allData);
	}
	rewind(fpo);
	switch (biBitCount)
	{
		case 24:
			fwrite(&allhead, bfOffBits, 1, fpo);
			break;
		case 8:
			// 申请颜色表所需要的空间，读颜色表进内存
			// fread(pColorTable,sizeof(RGBQUAD),256,fp);
			fwrite(&allhead, 54, 1, fpo);
			fwrite(pColorTable, sizeof(RGBQUAD) * 256, 1, fpo);
			break;
		default:
			break;
	}
	return true;
}

bool Rbmp::isNew(ppPIXELS imageData)
{
	if(imageData == NULL)
	{
		return true;
	}
	if ((imageData[0][1].getY() != 0) || (imageData[0][1].getY() != 0) ||
			(imageData[0][1].getEdge() != 0) || (imageData[0][1].getRGB().rgbRed != 0) ||
			(imageData[0][1].getRGB().rgbGreen != 0) || (imageData[0][1].getRGB().rgbBlue != 0))
	{
		return false;
	}
	return true;
}
bool Rbmp::setHead(BMPALLHEAD& allhead,int W,int H)
{
	if(W <= 0 || H <= 0)
	{
		printf("In %s W is %d H is %d!\n",__FUNCTION__,W,H);
		return false;
	}
	bmpWidth = W;
	bmpHeight = H;
	allhead.infoHead.biWidth = W;
	allhead.infoHead.biHeight = H;
	int lineByte = (W * biBitCount + 31) / 32 * 4;
	allhead.infoHead.biSizeImage = H * lineByte;
	allhead.bmpHead.bfSize = H * lineByte + 54;
	//printf("allhead:%p,lineByte:%d\n",&allhead,lineByte);
	return true;
}

bool Rbmp::writeAllData(ppPIXELS& imageData)
{
	int H = allhead.infoHead.biHeight;
	int W = allhead.infoHead.biWidth;
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
	//setHead(lineByte,W,H);
	//printf("lineByte:%d\n",lineByte);
	BYTE8 *linedata = new BYTE8[lineByte];
	memset(linedata, 0, lineByte);
	if (pColorTable)
	{
		memset(pColorTable, 0, sizeof(RGBQUAD) * 256);
	}
	int state;
	int x, k;
	//printf("WWW:%d HHH:%d\n", W, H);
	for (int y = (H - 1); y >= 0; y--)
	{
		for (x = 0, k = 0; x < W; x++, k++)
		{
			//printf("x:%d\n",x);
			switch (biBitCount)
			{
				case 24:
					imageData[y][x].setData(linedata[k], linedata[k + 1], linedata[k + 2]);
					k += 2;
					break;
				case 8:
					state = addColorTable(imageData[y][x], linedata[k]);
					if (state != -1)
						linedata[k] = state;
					break;
				default:
					break;
			}
		}
		//printf("???????????fpo:%p linedata:%p\n",fpo,linedata);
		fwrite(linedata, lineByte, 1, fpo);
		//printf("y:%d\n",y);
	}
	// free memory
	if (linedata)
		delete[]linedata;
	delImageData(imageData, H);
	return true;

}

// return value = -1:the color is exist
int Rbmp::addColorTable(PIXELS pixel, BYTE8 & linedata)
{
	int i = 0;
	while (i < 256)
	{
		// pColorTable[i++] == pixel.getRGB()
		if (pColorTable[i].rgbBlue == pixel.getRGB().rgbBlue &&
				pColorTable[i].rgbRed == pixel.getRGB().rgbRed &&
				pColorTable[i].rgbGreen == pixel.getRGB().rgbGreen)
		{
			linedata = i;
			return -1;
		}
		i++;
	}
	/*
	pColorTable[globalI] = pixel.getRGB(); printf("new颜色表:%d
			",globalI); pixel.show_PIXELS(); printf("\n"); globalI++; 
	*/
	return globalI - 1;
}

bool Rbmp::deal_image(const char* dealType)
{
	if(!dealType)
	{
		printf("Not deal with!\n");
		return true;
	}
	while(*dealType)
	{
		switch(*dealType)
		{
			case 'T':
				cout << "  -T     imageTranspose  : Transpose a iamge\n";
				imageTranspose();
				break;
			case 'R':
				cout << "  -R     imageRevolution : Revolution a image\n";
				imageRevolution(bmpWidth/2,bmpHeight/2,-45);
				break;
			case 's':
				cout << "  -s     imageSpherize   : Spherize a image\n";
				imageSpherize();
				break;
			case 'Z':
				cout << "  -Z     imageZoom       : zoom a image\n";
				imageZoom(0.5,0.5);
				break;
			case 'M':
				cout << "  -M     imageMirror     : Mirror a image\n";
				imageMirror(UD);
				break;
			case 'S':
				cout << "  -S     imageShear      : Shear a image\n";
				imageShear(true,-45.0);
				break;
			case 'm':
				cout << "  -m     imageMove       : move a image\n";
				imageMove(100,100);
				break;
			case 'C':
				cout << "  -C     getImage3Color  : get a image's 3(R,G,B) color image\n";
				getImage3Color(Green);
				break;
			case 'H':
				cout << "  -H     genHistogram    : get a image's 3(R,G,B) color Histogram\n";
				genBardiagram(Red);
				genBardiagram(Green);
				genBardiagram(Blue);
				genBardiagram();
				break;
			case 'B':
				cout << "  -B     genBardiagram   : get a image's 3(R,G,B) color Bar diagram\n";
				genHistogram(Red);
				genHistogram(Green);
				genHistogram(Blue);
				genHistogram();
				break;
			case 'b':
				cout << "  -b     backGround_ize   : get a image's part of backGround\n";
				backGround_ize();
				break;
			case 'h':
				cout << "  -h     boundarysHL      : change boundarys line to HightLight\n";
				boundarysHL();
				break;
			case 'c':
				cout << "  -c     imageCutOut      : cutout the effective part of the image\n";
				imageCutOut();
				break;
			case 'd':
				cout << "  -d     imageDensity     :  Change a image each pixel's Idensity\n";
				imageDensity(2);
				break;
			default:
				printf("Not deal with!\n");
				break;
		}
		dealType++;
	}
	return true;
}

bool Rbmp::genBardiagram(colorType color)
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
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
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
	int x, k;
	FILE* fpg = NULL;
	size_t pos = bmppath.rfind(".bmp");
	string outpath;
	if(pos != string::npos)
		outpath = bmppath.substr(0,pos) + color2str(color) +"_bar.bmp";
	else
		outpath = bmppath + color2str(color) +"_htg.bmp";
	//printf("genHistogram :%s Rmax:%f Gmax:%f Blue:%f\n",outpath.c_str() ,Rmax,Gmax,Bmax);
	fpg = fopen(outpath.c_str(),"wb");
	BMPALLHEAD HallHead;
	memcpy(&HallHead,&allhead,sizeof(BMPALLHEAD));
	if(setHead(HallHead,W,H))
	{
		fwrite(&HallHead, 54, 1, fpg);
	}
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
	//write allData
	BYTE8 *linedata = new BYTE8[lineByte];
	memset(linedata,0,lineByte);
	bool flag = true;
	for (int y = 0; y < H; y++)
	{
		for (x = 0, k = 0; x < W; x++, k++)
		{
			if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
			{
				linedata[k++] = 255, linedata[k++] = 255, linedata[k] = 255;
			}
			else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
					|| (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
			{
				linedata[k++] = 0, linedata[k++] = 0, linedata[k] = 0;
			}
			else
			{
				switch(color)
				{
					case Red:
						if(RED[x-Rb]*Rscale > y-Db)
						{
							flag = false;
							linedata[k+2] = 255;
						}
						else
							flag = true;
						break;
					case Green:
						if(GRE[x-Rb]*Gscale > y-Db)
						{
							flag = false;
							linedata[k+1] = 255;
						}
						else
							flag = true;
						break;
					case Blue:
						if(BLU[x-Rb]*Bscale > y-Db)
						{
							flag = false;
							linedata[k] = 255;
						}
						else
							flag = true;
						break;
					case Pricolor:
						if(ALL[x-Rb]*Ascale > y-Db)
						{
							flag = false;
							linedata[k] = 57,linedata[k+1] = 77,linedata[k+2] = 118;
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
					linedata[k] = 255, linedata[k+1] = 255, linedata[k+2] = 255;
				}
				k+=2;
			}
		}
		fwrite(linedata, lineByte, 1, fpg);
	}
#undef  Hh
	if (linedata)
	{
		delete[]linedata;
		linedata = NULL;
	}
	if(fpg)
	{
		fclose(fpg);
		fpg = NULL;
	}
	return true;
}
bool Rbmp::genHistogram(colorType color)
{
	if(NULL == allData)
	{
		printf("In %s allData is NULL\n",__FUNCTION__);
		return false;
	}
	float PIXELSCOUNT = bmpWidth * bmpHeight;
	int RED[256] = {0};
	float FRED[256] = {0};
	int GRE[256] = {0};
	float FGRE[256] = {0};
	int BLU[256] = {0};
	float FBLU[256] = {0};
	int ALL[256] = {0};
	float FALL[256] = {0};
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
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
		printf("%d %d %d %d %d\n",i,RED[i],GRE[i],BLU[i],ALL[i]);
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
	printf("PIXELSCOUNT：%f\n",PIXELSCOUNT);
	printf("R:%f G:%f B:%f A:%f\n",Rmax,Gmax,Bmax,Amax);
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
	printf("R:%f G:%f B:%f A:%f\n",Rscale,Gscale,Bscale,Ascale);
	int x, k;
	FILE* fpg = NULL;
	size_t pos = bmppath.rfind(".bmp");
	string outpath;
	if(pos != string::npos)
		outpath = bmppath.substr(0,pos) + color2str(color) +"_htg.bmp";
	else
		outpath = bmppath + color2str(color) +"_htg.bmp";
	//printf("genHistogram :%s Rmax:%f Gmax:%f Blue:%f\n",outpath.c_str() ,Rmax,Gmax,Bmax);
	fpg = fopen(outpath.c_str(),"wb");
	BMPALLHEAD HallHead;
	memcpy(&HallHead,&allhead,sizeof(BMPALLHEAD));
	if(setHead(HallHead,W,H))
	{
		fwrite(&HallHead, 54, 1, fpg);
	}
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
	//write allData
	BYTE8 *linedata = new BYTE8[lineByte];
	memset(linedata,0,lineByte);
	bool flag = true;
	for (int y = 0; y < H; y++)
	{
		for (x = 0, k = 0; x < W; x++, k++)
		{
			if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
			{
				linedata[k++] = 255, linedata[k++] = 255, linedata[k] = 255;
			}
			else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
					|| (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
			{
				linedata[k++] = 0, linedata[k++] = 0, linedata[k] = 0;
			}
			else
			{
				switch(color)
				{
					case Red:
						if(FRED[x-Rb]*Rscale > y-Db)
						{
							flag = false;
							linedata[k+2] = 255;
						}
						else
							flag = true;
						break;
					case Green:
						if(FGRE[x-Rb]*Gscale > y-Db)
						{
							flag = false;
							linedata[k+1] = 255;
						}
						else
							flag = true;
						break;
					case Blue:
						if(FBLU[x-Rb]*Bscale > y-Db)
						{
							flag = false;
							linedata[k] = 255;
						}
						else
							flag = true;
						break;
					case Pricolor:
						if(FALL[x-Rb]*Ascale > y-Db)
						{
							flag = false;
							linedata[k] = 57,linedata[k+1] = 77,linedata[k+2] = 118;
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
					linedata[k] = 255, linedata[k+1] = 255, linedata[k+2] = 255;
				}
				k+=2;
			}
		}
		fwrite(linedata, lineByte, 1, fpg);
	}
#undef  Hh
	if (linedata)
	{
		delete[]linedata;
		linedata = NULL;
	}
	if(fpg)
	{
		fclose(fpg);
		fpg = NULL;
	}
	return true;
}
ppPIXELS Rbmp::imageMirror(Method method)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	switch (method)
	{
		case UD:					// up down change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[bmpHeight - 1 - y][x];
				}
			}
			break;
		case LR:					// left right change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][bmpWidth - 1 - x];
				}
			}
			break;
		case UR:					// up down & left right change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[bmpHeight - 1 - y][bmpWidth - 1 - x];
				}
			}
			break;
		default:					// NONE
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x];
				}
			}
			break;
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::getImage3Color(colorType color)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	switch (color)
	{
		case Red:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x].get3Color(Red);
				}
			}
			break;
		case Green:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x].get3Color(Green);
				}
			}
			break;
		case Blue:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x].get3Color(Blue);
				}
			}
			break;
		default:
			break;
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

// move the image:x>0,y>0 ->right down ;x<0 y<0 -> left up
ppPIXELS Rbmp::imageMove(int mx, int my)
{
#define MOVEX(Mx,x) ((Mx)>0 ? (x <= Mx) : (x > bmpWidth-1+Mx))
#define MOVEY(My,y) ((My)>0 ? (y <= My) : (y > bmpHeight-1+My))
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if ((MOVEY(my, y) || MOVEX(mx, x)))
				allData[y][x].setRGB(255, 255, 255);
			else
				allData[y][x] = tmpimageData[y - my][x - mx];
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::imageDatadup2(ppPIXELS imageData, ppPIXELS &tmpimageData)
{
	// tmpimageData = new pPIXELS[bmpWidth];
	tmpimageData = (ppPIXELS)calloc(bmpWidth, sizeof(PIXELS));
	if(!tmpimageData)
	{
		printf("In %s calloc memory is failed\n",__FUNCTION__);
	}
	for (int y = 0; y < bmpHeight; y++)
	{
		// tmpimageData[y] = new PIXELS[bmpWidth];
		tmpimageData[y] = (pPIXELS)calloc(bmpWidth, sizeof(PIXELS));
		if(!tmpimageData[y])
		{
			printf("In %s[y] calloc memory is failedp\n",__FUNCTION__);
		}
		memcpy(tmpimageData[y], imageData[y], sizeof(PIXELS) * bmpWidth);
	}
	return tmpimageData;
}

ppPIXELS Rbmp::imageZoom(float scalex, float scaley)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	if (scalex <= 0.0 || scaley <= 0.0)
	{
		printf("In %s zoom number is <= 0 ,is wrong",__FUNCTION__);
		return NULL;
	}
	else
	{
		int W = bmpWidth * scalex;
		int H = bmpHeight * scaley;
		delImageData(allData, bmpHeight);	// free
		if (allData == NULL)	// renew
		{
			newImageData(allData, W, H);
			// printf("new is ok\n");
		}
		//printf("imageZoom: WWW:%d HHH:%d\n",W,H);
		for (int y = 0; y < H; y++)
		{
			for (int x = 0; x < W; x++)
			{
				int nx = (int)(x / scalex);
				int ny = (int)(y / scaley);
				allData[y][x] = tmpimageData[ny][nx];
				allData[y][x].setXY(x, y);
				// imageData[y][x].show_PIXELS();
				// tmpimageData[y/scaley][x/scalex].show_PIXELS();
				// printf("\n");
			}
		}
		// first free tmpimageData
		delImageData(tmpimageData, bmpHeight);
		// then set allhead
		setHead(allhead,W,H);
	}
	return allData;
}

ppPIXELS Rbmp::imageTranspose(bool AR)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	int W = bmpHeight;
	int H = bmpWidth;
	delImageData(allData, bmpHeight);	// free
	if (allData == NULL)		// renew
	{
		newImageData(allData, W, H);
		// printf("new is ok\n");
	}
	// printf("WWW:%d HHH:%d\n",W,H);
#define AorR(x,y) ((AR) ? (tmpimageData[x][H-1-y]) : (tmpimageData[W-1-x][y]))
	for (int y = 0;y < H;y++)
	{
		for (int x = 0;x < W;x++)
		{
			allData[y][x] = AorR(x,y);
		}
	}
	// first free tmpimageData
	delImageData(tmpimageData, bmpHeight);
	// then set allhead
	setHead(allhead,W,H);
	return allData;
}

ppPIXELS Rbmp::imageShear(bool XorY,float angle)
{
	angle = D2R(angle);
	ppPIXELS tmpimageData = NULL;
	int nx, ny;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	int H = XorY ? bmpHeight : (int)(bmpHeight + bmpWidth * ABS(tan(angle)));
	int W = XorY ? (int)(bmpWidth + bmpHeight * ABS(tan(angle))) : bmpWidth;
	//printf("Shear after: W:%d H:%d\n",W,H);
	delImageData(allData, bmpHeight);	// free
	if (allData == NULL)	// renew
	{
		newImageData(allData, W, H);
		// printf("new is ok\n");
	}
	for (int y = 0; y < H; y++)
	{
		for (int x = 0; x < W; x++)
		{
			//right up
			/*
			nx = XorY ? x + tan(angle) * (y - bmpHeight): x;
			ny = XorY ? y : y + tan(angle) * (x - bmpWidth);
			left down
			nx = XorY ? x - tan(angle) * (y - bmpHeight)- bmpHeight: x;
			ny = XorY ? y : y - tan(angle) * (x - bmpWidth) - bmpWidth;
			*/
			nx = (XorY ? ((angle > 0) ? (x + tan(angle) * (y - bmpHeight)) : (x + tan(angle) * (y - bmpHeight) - bmpHeight)) : x);
			ny = (XorY ? y : ((angle > 0) ? (y + tan(angle) * (x - bmpWidth)) :(y + tan(angle) * (x - bmpWidth) - bmpWidth)));
			//printf("Shear after: x:%d nx:%d y:%d ny:%d\n",x,nx,y,ny);
			if (nx < 0 || ny < 0 || nx >= bmpWidth || ny >= bmpHeight)
				allData[y][x].setRGB(255, 255, 255);
			else
				allData[y][x] = tmpimageData[ny][nx];
		}
	}	// then set allhead
	//delImageData(imageData,H);
	setHead(allhead,W,H);
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::imageRevolution(int px,int py,float angle)
{
	angle = D2R(angle);
	ppPIXELS tmpimageData;
	int nx, ny;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{

			nx = (x - px) * cos(angle) - (y - py) * sin(angle) + px;
			ny = (x - px) * sin(angle) + (y - py) * cos(angle) + py;
			//printf("angle: %f cos:%f X:%d nx:%d\t Y:%d ny:%d\n",angle, cos(angle),x,nx,y,ny);
			if (nx < 0 || ny < 0 || nx >= bmpWidth || ny >= bmpHeight)
				allData[y][x].setRGB(255, 255, 255);
			else
				allData[y][x] = tmpimageData[ny][nx];
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::imageSpherize(float radius)
{
	ppPIXELS tmpimageData;
	float w = bmpWidth / 2.0;
	float h = bmpHeight / 2.0;
	float wh = w / h;
	float mx1, mx2;
	int nx;
	float radius2 = radius * radius;
	float dealt;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	if (radius <= 0.0)			// oval
	{
		for (int y = 0; y < bmpHeight; y++)
		{
			//mx1 = w + (w * sqrt(2 * h * y - y * y)) / h;
			//mx2 = w - (w * sqrt(2 * h * y - y * y)) / h;
			mx1 = w + wh * sqrt(2 * h * y - y * y);
			mx2 = w - wh * sqrt(2 * h * y - y * y);
			dealt = (mx1 - mx2) / bmpWidth;
			//printf("Y:%d, %d - %d %d ;%f\n", y, mx1, mx2, mx1 - mx2, dealt);
			for (int x = 0; x < bmpWidth; x++)
			{
				if (x < mx2 || x > mx1 || dealt == 0)
					allData[y][x].setRGB(255, 255, 255);
				else
				{
					nx = (x - mx2) / dealt;
					if (nx >= bmpWidth)
					{
						nx = bmpWidth - 1;
					}
					allData[y][x] = tmpimageData[y][nx];
				}
			}
		}
	}

	else// circle
	{
		for (int y = 0; y < bmpHeight; y++)
		{
			mx1 = w + sqrt(radius2 - (y - h) * (y - h));
			// mx1 = w + sqrt(2*h*y-y*y);
			mx2 = w - sqrt(radius2 - (y - h) * (y - h));
			// mx2 = w - sqrt(2*h*y-y*y);
			dealt = (mx1 - mx2) / (bmpWidth * 1.0);
			//printf("Y:%d, %d - %d ;%f\n", y, mx1, mx2, dealt);
			for (int x = 0; x < bmpWidth; x++)
			{
				// if(x < mx2 || x > mx1)
				if (x < mx2 || x > mx1 || dealt == 0)
					allData[y][x].setRGB(255, 255, 255);
				else
				{
					nx = (x - mx2) / dealt;
					if (nx >= bmpWidth)
					{
						nx = bmpWidth - 1;
					}
					allData[y][x] = tmpimageData[y][nx];
				}
			}
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}
ppPIXELS Rbmp::imageSpatialize(string outpath)
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
	ppPIXELS imageData[6] = {NULL};
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
				imageShear(false,45.0);
				H = allhead.infoHead.biHeight;
				LH = H;
				break;
			case Right:
				imageShear(false,-45.0);
				H = allhead.infoHead.biHeight;
				RH = H;
				break;
			case Up:
				imageShear(true,45.0);
				W = allhead.infoHead.biWidth;
				UH = allhead.infoHead.biHeight;
				break;
			case Down:
				imageShear(true,-45.0);
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
	newImageData(newImage,W,H);
	//memset(newImage,255,sizeof(PIXELS)*W*H);
	//printf("imageData:%p,fpo:%p",imageData[Down],fpo);
	//imageData[Down][0][0].show_PIXELS();
	writeAllData(newImage);
	//free a image memory!
	if(LH)
	{
		//printf("Lwwww%d\n",LH);
		delImageData(imageData[Left],LH);
	}
	if(RH)
	{
		//printf("Rwwww%d\n",RH);
		delImageData(imageData[Right],RH);
	}
	if(UH)
	{
		//printf("Uwwww%d\n",UH);
		delImageData(imageData[Up],UH);
	}
	if(DH)
	{
		//printf("Dwwww%d ",DH);
		delImageData(imageData[Down],DH);
	}
	if(FH)
	{
		//printf("Fwwww%d\n",FH);
		delImageData(imageData[Front],FH);
	}
	if(BH)
	{
		//printf("Bwwww%d\n",BH);
		delImageData(imageData[Back],BH);
	}
	return NULL;
}
ppPIXELS Rbmp::imageDensity(float scale)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	if(scale <= 0)
	{
		return NULL;
	}
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			allData[y][x] = tmpimageData[y][x]*scale;
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}
ppPIXELS Rbmp::newImageData(ppPIXELS &imageData, int W, int H)
{
	// malloc some memroy
	// imageData = new pPIXELS[W];
	imageData = (ppPIXELS)calloc(W, sizeof(PIXELS));
	if(!imageData)
		printf("In %s new imageData wrong!\n",__FUNCTION__);
	// printf("new:(W,H):%d,%d\n",W,H);
	int y;
	for (y = 0; y < H; y++)
	{
		// imageData[y] = new PIXELS[W];
		imageData[y] = (pPIXELS)calloc(W, sizeof(PIXELS));
		if (!imageData[y])
			printf("In %s new imageData[y] wrong!\n",__FUNCTION__);
	}
	return imageData;
}

bool Rbmp::delImageData(ppPIXELS& imageData, int H)
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
		//printf("In delImageData imageData = NULL\n");
	}
	return true;
}

void Rbmp::get_image_msg()
{
	// 显示位图文件头结构BITMAPFILEHEADER
	show_bmp_head(head);
	// 显示位图信息头结构变量,存放在变量head中
	show_info_head(infohead);
}

void Rbmp::show_bmp_head(BITMAPFILEHEADER & head)
{
	cout << "位图文件头:" << endl;
	cout << "\t文件头类型  : " << head.bfType << endl;
	cout << "\t文件的大小  : " << head.bfSize << endl;
	cout << "\t保留字 _1   : " << head.bfReserved1 << endl;
	cout << "\t保留字 _2   : " << head.bfReserved2 << endl;
	cout << "\tRGB偏移字节 : " << head.bfOffBits << endl << endl;
}

void Rbmp::show_info_head(BITMAPINFOHEADER & infohead)
{
	cout << "位图的信息头:" << endl;
	cout << "\t结构体的长度: " << infohead.biSize << endl;
	cout << "\t位图宽(像素): " << infohead.biWidth << endl;
	cout << "\t位图高(像素): " << infohead.biHeight << endl;
	cout << "\t位图的平面数: " << infohead.biPlanes << endl;
	cout << "\t采用颜色位数: " << infohead.biBitCount << endl;
	cout << "\t压缩的方式  : " << infohead.biCompression << endl;
	cout << "\t位图数据大小: " << infohead.biSizeImage << endl;
	cout << "\tX 方向分辨率: " << infohead.biXPelsPerMeter << endl;
	cout << "\tY 方向分辨率: " << infohead.biYPelsPerMeter << endl;
	cout << "\t使用的颜色数: " << infohead.biClrUsed << endl;
	cout << "\t重要颜色数  : " << infohead.biClrImportant << endl;
}

bool Rbmp::out_range_error(PIXELS pixel)
{
	int x = pixel.getX();
	int y = pixel.getY();
	if ((x >= bmpWidth) || (y >= bmpHeight) || (x < 0) || (y < 0))
		return true;
	else
		return false;
}
void Rbmp::show_6path(map<Position,string> pathl)
{
	map<Position,string>::iterator it;
	for(it = pathl.begin(); it != pathl.end() ; ++it)
	{
		cout<<"key: " << it->first <<" (" << Pos2str(it->first)
			<< ")" << "\tvalue: " << it->second << endl;
	}
}
bool Rbmp::setBackground(RGBQUAD rgb)
{
	if(OUTRANGE(rgb.rgbRed) || OUTRANGE(rgb.rgbGreen) || OUTRANGE(rgb.rgbBlue))
		return false;
	backGround = rgb;
	return true;
}
bool Rbmp::setBackground(U8 r,U8 g,U8 b)
{
	if(OUTRANGE(r) || OUTRANGE(g) || OUTRANGE(b))
		return false;
	backGround.rgbRed = r;
	backGround.rgbGreen = g;
	backGround.rgbBlue = b;
	return true;
}
bool Rbmp::setBackground(const PIXELS& pixel)
{
	RGBQUAD rgb = pixel.getRGB();
	return setBackground(rgb);
}
U32 Rbmp::setGranularity(U32 gran,bool opeartor)
{
	granularity = gran;
	granOpeartor = opeartor;
	return gran;
}
bool Rbmp::isEdge(int x,int y)
{
	if(x <= 0 || x >= bmpWidth ||
			y <= 0 || y >= bmpHeight)
		return true;
	else
		return false;
}
bool Rbmp::getRpoint(Position& direction,int& x,int& y)
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

bool Rbmp::getLpoint(Position& direction,int& x,int& y)
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
bool Rbmp::backGround_ize()
{
	for (int y = 0;y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if(!alikeBackground(allData[y][x]))
			{
				allData[y][x].setRGB(0,0,0);
			}
		}
	}
	return true;
}
//return: true trackDown again
/*
bool Rbmp::testStartP(PIXELS& pixel,int range)
{
    range = testRange;
    int x = pixel.getX();
    int y = pixel.getY();
    if(x < range || x > bmpWidth - 1 - range)
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
}
*/
void getNext(Position& pos, int &x,int& y,int& nexts,int& step)
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

bool Rbmp::testStartP(PIXELS& pixel)
{
    int step = 1;
    int x = pixel.getX();
    int y = pixel.getY();
    Position nextPos = Left;
    int numPoint = 1;
    int nexts = 0;
//    pixel.show_PIXELS();
//    printf("\n");
    getNext(nextPos,x,y,nexts,step);
//    allData[y][x].show_PIXELS();
//    printf("\n");
    while( !isEdge(x,y) && numPoint < 25 && allData[y][x].getEdge() >= 0)
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
//#define FramesRelation()
void Rbmp::linker(const Frames& frame)
{
	printf("Frames point:%ld\n",frame.size());
	vector<FramePoint>::const_iterator itPoint;
	for(itPoint = frame.begin(); itPoint != frame.end();++itPoint)
	{
		printf("UPy:%d Downy:%d Leftx:%d Rightx:%d\n",
				(*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
	}
}
//note: index not safe
PIXELS* Rbmp::getBLpixel(dPIXELS& boundaryline,int index)
{
	int bsize = boundaryline.size();
	if(ABS(index) > bsize)
		return NULL;
	if(index < 0)
		index = bsize + index;
	return &boundaryline[index];
}

PState Rbmp::getPointState(const PIXELS& pixel)
{
    int x = pixel.getX();
    int y = pixel.getY();
    float right = getSimilarity(Right,x,y);
    float left = getSimilarity(Left,x,y);
    if(right > left)
        return INPOT;
    else if (right < left)
        return OUTPOT;
    else
        return ONLYPOT;

}
PState Rbmp::getPointState(int x,int y)
{
    if(allData[y][x].getpPos().first == Down)
        return INPOT;
    if(allData[y][x].getpPos().first == Up)
        return OUTPOT;
}

void  Rbmp::genSkipTable(PIXELS& pixel)
{
    limitXXY footprint;
    int beforeX = 0;
    int x = pixel.getX();
    int y = pixel.getY();
    PState pstate = getPointState(x,y);
    switch (pstate)
    {
    case INPOT:
        skipLine.push(x);
        break;
    case OUTPOT:
        if(!skipLine.empty())
        {
            beforeX = skipLine.top();
            footprint.add(allData[y][beforeX],allData[y][x],skipTable);
            skipLine.pop();
        }
        break;
    case ONLYPOT:
        footprint.add(allData[y][x],allData[y][x],skipTable);
        break;
    default:
        break;
    }
}