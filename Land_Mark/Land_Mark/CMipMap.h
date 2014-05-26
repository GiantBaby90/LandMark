#include "CSingleLevelMap.h"
#include "ZquadTree.h"
#include <vector>

#ifndef _CMIP_MAP_H_
#define _CMIP_MAP_H_

#define WIDTH_HARF_LENGHT 256
#define DEPTH 0.5

#define HEIGHT 600

class CMipMap
{
private:

	enum MAP_LEVEL
	{
		PRE_LEVEL = 0, CURRENT_LEVEL, POST_LEVEL
	};

	ZQuadTree quadTree[9];
	std::vector<Point> VertexArray[9];

	int currentLevel;

	Point currentImage;
	Point leftBottomPosition[9];
	CSingleLevelMap *geoMap[3];

	void quadTreeBuild();
	
	void _drawMaps(unsigned int texture_id, std::vector<Point> &vertexArray, CGeoMapData& heightMap, Point &bottomLeft);
public:

	CMipMap(int level = 1);
	~CMipMap();

	Point refresh(const Point& p); //지형 갱신

	void GenerateIndex(CFrustum& pFrustum, float fLODRatio);
	void Draw();

	bool levelUp(const Point& p);
	bool levelDown(const Point& p);

	int getCurrentLevel();
};

#endif