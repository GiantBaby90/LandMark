#include "CMipMap.h"


CMipMap::CMipMap(int level)
{
	currentLevel = level;

	for(int i = 0; i < 3; i++)
		geoMap[i] = new CSingleLevelMap(level - 1 + i);

	leftBottomPosition[NORTH].y = leftBottomPosition[NORTH_WEST].y = leftBottomPosition[NORTH_EAST].y = WIDTH_HARF_LENGHT -10;
	leftBottomPosition[NORTH_EAST].x = leftBottomPosition[EAST].x = leftBottomPosition[SOUTH_EAST].x = WIDTH_HARF_LENGHT;//-10;

	leftBottomPosition[CUR].x = leftBottomPosition[NORTH].x = leftBottomPosition[SOUTH].x = -WIDTH_HARF_LENGHT;
	leftBottomPosition[WEST].y = leftBottomPosition[CUR].y = leftBottomPosition[EAST].y = -WIDTH_HARF_LENGHT;

	leftBottomPosition[WEST].x = leftBottomPosition[NORTH_WEST].x = leftBottomPosition[SOUTH_WEST].x = -WIDTH_HARF_LENGHT * 3;// + 10;
	leftBottomPosition[SOUTH_WEST].y = leftBottomPosition[SOUTH].y = leftBottomPosition[SOUTH_EAST].y = -WIDTH_HARF_LENGHT * 3 + 10;

	currentImage = geoMap[CURRENT_LEVEL]->getCurrentImage();
	

	quadTreeBuild();
	glEnable(GL_TEXTURE_2D);
}

CMipMap::~CMipMap()
{
	for(int i = 0; i < 3; i++)
		delete geoMap[i];
}
/*
bool *getLoadImage();
	unsigned int *getTextList();
	CGeoMapData **getGeoMapData();
*/
void CMipMap::quadTreeBuild()
{
	CGeoMapData **map = geoMap[CURRENT_LEVEL]->getGeoMapData();

	for(int i = 0; i < DIRECTION_COUNT; i++)
		quadTree[i].Build(*map[i], leftBottomPosition[i]);
}

void CMipMap::GenerateIndex(CFrustum& pFrustum, float fLODRatio)
{
	bool *load_Image = geoMap[CURRENT_LEVEL]->getLoadImage();
	CGeoMapData **map = geoMap[CURRENT_LEVEL]->getGeoMapData();

	for(int i=0;i<DIRECTION_COUNT;i++)
	{
		VertexArray[i].clear();
		if(load_Image[i] != false) quadTree[WEST].GenerateIndex(VertexArray[i], *map[i], leftBottomPosition[i], pFrustum, fLODRatio);
	}
}

void CMipMap::Draw()
{
	bool *load_Image = geoMap[CURRENT_LEVEL]->getLoadImage();
	CGeoMapData **map = geoMap[CURRENT_LEVEL]->getGeoMapData();
	unsigned int *texture_list = geoMap[CURRENT_LEVEL]->getTextList();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for(int i = 0; i < DIRECTION_COUNT; i++)
	{
		if(VertexArray[i].size() != 0) _drawMaps(texture_list[i], VertexArray[i], *map[i], leftBottomPosition[i]);
	}
	glFlush();
}

void CMipMap::_drawMaps(unsigned int texture_id, std::vector<Point> &vertexArray, CGeoMapData& heightMap, Point &bottomLeft)
{
	std::vector<Point>::iterator iter = vertexArray.begin();
	int nXSize = heightMap.GetXSize();
	int nYSize = heightMap.GetYSize();

	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glBegin(GL_TRIANGLES);
	while(iter != vertexArray.end())
	{
//		glColor3f(1.0,0.0,0.0);
		glTexCoord2d((double)iter->x/nXSize, (double)iter->y/nYSize);
		glVertex3f(bottomLeft.x + iter->x, bottomLeft.y + iter->y, heightMap.getPosition(iter->x, iter->y) * DEPTH * (currentLevel + 1));
		iter++;

//		glColor3f(0.0,1.0,0.0);
		glTexCoord2d((double)iter->x/nXSize, (double)iter->y/nYSize);
		glVertex3f(bottomLeft.x + iter->x, bottomLeft.y + iter->y, heightMap.getPosition(iter->x, iter->y) * DEPTH * (currentLevel + 1));
		iter++;

//		glColor3f(0.0,0.0,1.0);
		glTexCoord2d((double)iter->x/nXSize, (double)iter->y/nYSize);
		glVertex3f(bottomLeft.x + iter->x, bottomLeft.y + iter->y, heightMap.getPosition(iter->x, iter->y) * DEPTH * (currentLevel + 1));
		iter++;
	}
	glEnd();
	glPopMatrix();
}

Point CMipMap::refresh(const Point& p)
{	
	Point temp(0,0);

	if(p.x > WIDTH_HARF_LENGHT)
	{
		currentImage.x++;
		temp.x -= WIDTH_HARF_LENGHT * 2;
	}

	if(p.x < -WIDTH_HARF_LENGHT)
	{
		currentImage.x--;
		temp.x += WIDTH_HARF_LENGHT * 2;
	}

	if(p.y > WIDTH_HARF_LENGHT)
	{
		currentImage.y++;
		temp.y -= WIDTH_HARF_LENGHT * 2;
	}

	if(p.y < -WIDTH_HARF_LENGHT)
	{
		currentImage.y--;
		temp.y += WIDTH_HARF_LENGHT * 2;
	}

	Point tempPoint(currentImage.x * 2,currentImage.y * 2);

	if((p.x < WIDTH_HARF_LENGHT && p.x > 0) || temp.x < 0) tempPoint.x++;
	if((p.y < WIDTH_HARF_LENGHT && p.y > 0) || temp.y < 0) tempPoint.y++;


	geoMap[POST_LEVEL]->refresh(tempPoint);
	geoMap[CURRENT_LEVEL]->refresh(currentImage);
	geoMap[PRE_LEVEL]->refresh(Point(currentImage.x/2, currentImage.x/2));

	return temp;
}

Point CMipMap::levelDown(const Point& p)
{
	
	CSingleLevelMap *temp = geoMap[POST_LEVEL];
	geoMap[POST_LEVEL] = geoMap[CURRENT_LEVEL];
	geoMap[CURRENT_LEVEL] = geoMap[PRE_LEVEL];
	geoMap[PRE_LEVEL] = temp;

	currentImage = geoMap[CURRENT_LEVEL]->getCurrentImage();
	currentLevel--;

	geoMap[PRE_LEVEL]->removeAll();
	geoMap[PRE_LEVEL]->setLevel(currentLevel-1);

	geoMap[PRE_LEVEL]->setCurrentImage(Point(currentImage.x/2, currentImage.y/2));

	bool *load_Image = geoMap[PRE_LEVEL]->getLoadImage();
	for(int i = 0; i < DIRECTION_COUNT; i++)
		load_Image[i] = geoMap[PRE_LEVEL]->openFile((DIRECTION) i);

	return Point(p.x/2, p.y/2);
}

Point CMipMap::levelUp(const Point& p)
{
	
	CSingleLevelMap *temp = geoMap[PRE_LEVEL];
	geoMap[PRE_LEVEL] = geoMap[CURRENT_LEVEL];
	geoMap[CURRENT_LEVEL] = geoMap[POST_LEVEL];
	geoMap[POST_LEVEL] = temp;
	

	currentImage = geoMap[CURRENT_LEVEL]->getCurrentImage();
	currentLevel++;

	geoMap[POST_LEVEL]->removeAll();
	geoMap[POST_LEVEL]->setLevel(currentLevel+1);
	Point tempPoint(currentImage.x * 2, currentImage.y * 2);

	if((p.x < WIDTH_HARF_LENGHT && p.x > 0)) tempPoint.x++;
	if((p.y < WIDTH_HARF_LENGHT && p.y > 0)) tempPoint.y++;

	geoMap[POST_LEVEL]->setCurrentImage(tempPoint);

	bool *load_Image = geoMap[POST_LEVEL]->getLoadImage();
	for(int i = 0; i < DIRECTION_COUNT; i++)
		load_Image[i] = geoMap[POST_LEVEL]->openFile((DIRECTION) i);

	return Point(p.x*2, p.y*2);
}

int CMipMap::getCurrentLevel()
{
	return currentLevel;
}