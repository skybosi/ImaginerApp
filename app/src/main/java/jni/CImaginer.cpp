#include <math.h>
#include "CImaginer.h"

ppPIXELS CImaginer::newImageData(ppPIXELS &imageData, int W, int H)
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

bool CImaginer::delImageData(ppPIXELS& imageData, int H)
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
		printf("In delImageData imageData = NULL\n");
	}
	return true;
}

bool CImaginer::insert(const PIXELS& cpixel)
{
	int x = cpixel.getX();
	int y = cpixel.getY();
	cimageData[x][y] = cpixel;
	return true;
}

float CImaginer::getSimilarity(PIXELS backPoint, PIXELS currPoint)
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
float CImaginer::getSimilarity(Position direction,int x,int y,int step)
{
	float Similarity = 0;
	//float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
	if(get_pix(x,y).empty())
		return -1;
	PIXELS potCurnt;
	PIXELS potRight;
	switch(direction)
	{
		case Down:
			if(y+step >= cheight)
				return -1;
			potCurnt = cimageData[y][x];
			potRight = cimageData[y+step][x];
			break;
		case Right:
			if(x+step >= cwidth)
				return -1;
			potCurnt = cimageData[y][x];
			potRight = cimageData[y][x+step];
			break;
		case Up:
			if(y-step < 0)
				return -1;
			potCurnt = cimageData[y][x];
			potRight = cimageData[y-step][x];
			break;
		case Left:
			if(x-step < 0)
				return -1;
			potCurnt = cimageData[y][x];
			potRight = cimageData[y][x-step];
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
PIXELS CImaginer::get_pix(int x, int y)
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
	ppot.setRGB(cimageData[y][x]);
	/*
	if(alikeBackground(ppot))
	{
		ppot.show_PIXELS();
		printf("\n");
	}
 */
	return ppot;
}

bool CImaginer::isBoundaryPoint(int& x,int& y)
{
	int i = 0;
	float similarity = 1;
	//float checkSmlrty = 0;
	float avgSimi = 0;
	float diffSim = 0;
	for (i = 0; x < cwidth-1; ++i,++x)
	{
		diffSim  = similarity - avgSimi;
		similarity = getSimilarity(Right,x,y);
		//printf("%3d: num:%lf\tavg:%lf\tdiff:%lf\n", i + 1, similarity, avgSimi, fabs(diffSim));
		avgSimi += diffSim / (i + 1);
		if (fabs(similarity - avgSimi) > 0.12)
		{
			++x;
			setBackground(cimageData[y][x]);
			//printf("finded :%lf\n", similarity);
			//make sure the edge point is not a shade
			//work is not stable, need TODO
			/*
			if(cimageData[y][x].getEdge() >= 0)
			{
				checkSmlrty = getSimilarity(Right,x,y);
				if(checkSmlrty != 1 && checkSmlrty != similarity)
				{
					++x;
					setBackground(cimageData[y][x]);
					similarity = checkSmlrty;
				}
			}
			*/
			baseSmlrty = similarity;
			// printf("baseSmlrty:%lf\n",baseSmlrty);
			break;
		}
	}
	if(x < cwidth-1)
	{
		return true;
	}
	else
		return false;
}

PIXELS CImaginer::get_pix(PIXELS pixel)
{
	PIXELS ppot;
	if (out_range_error(pixel))
	{
		//printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(), pixel.getY());
		ppot.setempty(true);
		return ppot;
	}
	ppot.setXY(pixel.getX(), pixel.getY());
	ppot.setRGB(cimageData[pixel.getY()][pixel.getX()]);
	ppot.show_PIXELS();
	printf("\n");
	return ppot;
}

bool CImaginer::out_range_error(const PIXELS& pixel)
{
	int x = pixel.getX();
	int y = pixel.getY();
	if ((x >= cwidth) || (y >= cheight) || (x < 0) || (y < 0))
		return true;
	else
		return false;
}
bool CImaginer::setBackground(RGBQUAD rgb)
{
	if(OUTRANGE(rgb.rgbRed) || OUTRANGE(rgb.rgbGreen) || OUTRANGE(rgb.rgbBlue))
		return false;
	backGround = rgb;
	return true;
}
bool CImaginer::setBackground(U8 r,U8 g,U8 b)
{
	if(OUTRANGE(r) || OUTRANGE(g) || OUTRANGE(b))
		return false;
	backGround.rgbRed = r;
	backGround.rgbGreen = g;
	backGround.rgbBlue = b;
	return true;
}
bool CImaginer::setBackground(const PIXELS& pixel)
{
	RGBQUAD rgb = pixel.getRGB();
	return setBackground(rgb);
}
bool CImaginer::getBoundaryLine(int& x, int& y)
{
	limitXXY footprint;
	int beforeX;
	beforeX = x;
	size_t bsize = boundarys.size();
	x = trackDown(cimageData[y][x]);
	if( boundarys.size() != bsize || x != beforeX)
	{
		//footprint.add(cimageData[y][beforeX],cimageData[y][x],skipTable);
		return true;
	}
	return false;
	//printf("next footprint'x value:%d\n",x+1);
	//printf("trackDown.... insert\n");
}
//start track down by following clues(顺藤摸瓜)
int  CImaginer::trackDown(PIXELS& startPoint)
{
	int sx = startPoint.getX();
	int x = sx;
	int sy = startPoint.getY();
	int y = sy;
	//make sure not trackDown again
	if(!boundarys.empty() && testStartP(startPoint))
		return sx;
	int nextx = 0;
	dPIXELS boundaryline;
	//SkipTable skipTable2;
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
	//    //startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
	//    SETCURR(Down,startPoint);
	//    startPoint.setpPosStatus();
	//    boundaryline.push_back(startPoint);
	//    /*
	//    printf("push s: ");
	//    startPoint.show_PIXELS();
	//    printf("\n");
	// */
	//    SETCURR(direction,cimageData[y][x]);
	//    /*
	//    if(y != sy)
	//    {
	//        cimageData[y][x].setEdge(-1);
	//    }
	//    else
	//    {
	//        cimageData[y][x].setEdge(-2);
	//    }
	//    */
	//    boundaryline.push_back(cimageData[y][x]);

	//SETCURR(Down,startPoint);
	//startPoint.setpPosStatus();
	//boundaryline.push_back(startPoint);
	Position prevDiret = direction;
	FramePoint framePoint(cheight,cwidth);
	x = sx,y = sy;
	int rx = sx,ry = sy;
	int lx = sx,ly = sy;
	Position ldirection = direction;
	Position rdirection = direction;
	bool rEdge = false;
	bool lEdge = false;

	//anticlockwise(getRpoint)
	x = rx;//rollback lx to x
	y = ry;//rollback ly to y
	direction = rdirection;//rollback rdirection to direction
	while(1)//getRpoint
	{
		PIXELS& prevPoint = cimageData[y][x];
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
				framePoint.setframePoint(direction,cimageData[y][x]);
				//set boundary line point
				boundaryline.push_back(prevPoint);
				break;
			}
			if(prevDiret == direction)
			{
				if(ISV(prevDiret))
				{
					prevPoint.setEdge(-1);
					//genSkipTable(prevPoint,skipTable2);
				}
				else
				{
					prevPoint.setEdge(-2);
				}
			}
			else//direction is change
			{
				prevPoint.setpPosStatus();//set Corner point
				int direSum = prevDiret + direction;
				switch(direSum)
				{
					case 3://reset direction
						prevDiret = direction;
						if(ISV(direction))
						{
							prevPoint.setEdge(-1);
							//genSkipTable(prevPoint,skipTable2);
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
							//genSkipTable(prevPoint,skipTable2);
						}
						break;
					case 5://LR or RL
						prevPoint.setEdge(-1);
						//genSkipTable(prevPoint,skipTable2);
						if(prevDiret > direction)
							prevPoint.setpPos(Up);
						else
							prevPoint.setpPos(Down);
						break;
					default://case 1:
						prevPoint.setEdge(-1);
						//genSkipTable(prevPoint,skipTable2);
						//genSkipTable(prevPoint,skipTable2);
						break;
				}
			}
			//set frame Point
			framePoint.setframePoint(direction,cimageData[y][x]);
			//set boundary line point
			boundaryline.push_back(prevPoint);
			/*
			   printf("push a: ");
			   get_pix(x,y).show_PIXELS();
			   printf("\n");
			   */
		}
		else //at Edge,Over loop
		{
			rEdge = true;
			break;
		}
	}

	//clockwise(getLpoint),if anticlockwise at Edge
	x = lx;//rollback lx to x
	y = ly;//rollback ly to y
	direction = ldirection;//rollback ldirection to direction
	while(rEdge)//getLpoint
	{
		PIXELS& prevPoint = cimageData[y][x];
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
					//genSkipTable(prevPoint,skipTable2);
				}
				//set frame Point
				framePoint.setframePoint(direction,cimageData[y][x]);
				//set boundary line point
				boundaryline.push_back(prevPoint);
				/*
				   printf("push a: ");
				   get_pix(x,y).show_PIXELS();
				   printf("\n");
				   */
				break;
			}
			if(prevDiret == direction)
			{
				if(ISV(prevDiret))
				{
					prevPoint.setEdge(-1);
					//genSkipTable(prevPoint,skipTable2);
				}
				else
				{
					prevPoint.setEdge(-2);
				}
			}
			else//direction is change
			{
				prevPoint.setpPosStatus();//set Corner point
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
							//genSkipTable(prevPoint,skipTable2);
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
							//genSkipTable(prevPoint,skipTable2);
						}
						break;
					case 5://LR or RL
						prevPoint.setEdge(-1);
						//genSkipTable(prevPoint,skipTable2);
						if(prevDiret < direction)
							prevPoint.setpPos(Up);
						else
							prevPoint.setpPos(Down);
						break;
					default://case 1:
						prevPoint.setEdge(-1);
						//genSkipTable(prevPoint,skipTable2);
						//genSkipTable(prevPoint,skipTable2);
						break;
				}
			}
			//set frame Point
			framePoint.setframePoint(direction,cimageData[y][x]);
			//set boundary line point
			boundaryline.push_back(prevPoint);
			/*
			   printf("push a: ");
			   get_pix(x,y).show_PIXELS();
			   printf("\n");
			   */
		}
		else //at Edge,Over loop
		{
			lEdge = true;
			break;
		}
	}
	//TrackTable tracktable;
	//skipTables.push_back(skipTable2);
	//genTrackTable(skipTable2,tracktable);
	//trackTables.push_back(tracktable);
	//startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
#define GRANOPERATION(size) (granOpeartor)?(size > granularity):(size <= granularity)
	if(GRANOPERATION(boundaryline.size()))
	{
		boundarys.push_back(boundaryline);
		framePoint.setBindNum(boundarys.size());
		//frames.push_back(framePoint);
		nextx = boundaryline.size();
	}
	else
	{
		nextx = 0;
		int xx = 0,yy = 0;
		while(!boundaryline.empty())//clean the smaller than the granularity data
		{
			xx = boundaryline.front().getX();
			yy = boundaryline.front().getY();
			cimageData[yy][xx].setEdge(0);
			cimageData[yy][xx].initpPos();
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
bool CImaginer::getRpoint(Position& direction,int& x,int& y)
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

bool CImaginer::getLpoint(Position& direction,int& x,int& y)
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
int CImaginer::alikeBackground(PIXELS pixel)
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
int CImaginer::alikeBackground(int x,int y)
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

bool CImaginer::alikeBackground(RGBQUAD rgb)
{
	if(EQUALBackGround(rgb))
		return true;
	else
		return false;
}

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

bool CImaginer::testStartP(PIXELS& pixel)
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
//    cimageData[y][x].show_PIXELS();
//    printf("\n");
    while( !isEdge(x,y) && numPoint < 25 && cimageData[y][x].getEdge() >= 0)
    {
        getNext(nextPos,x,y,nexts,step);
//        cimageData[y][x].show_PIXELS();
//        printf("\n");
        numPoint++;
    }
    //printf("%d\t",numPoint);
    if(numPoint >=25)
        return false;
    return true;
}
bool CImaginer::isEdge(int x,int y)
{
	if(x <= 0 || x >= cwidth ||
			y <= 0 || y >= cheight)
		return true;
	else
		return false;
}
