#include "CGeoMapData.h"
#include "CPixel.h"
#include "GL\freeglut.h"

#ifndef _CSINGLE_LEVEL_MAP_H_
#define _CSINGLE_LEVEL_MAP_H_

#define SERVER_PATH "http://localhost/zzz/"
#define DIRECTORY_PATH "D:/maps/"


#define MAX_LEVEL 5
#define MAX_MAP_WIDTH_COUNT 32

#define DIRECTION_COUNT 9

enum DIRECTION{
		CUR, WEST, EAST,
		SOUTH, SOUTH_WEST, SOUTH_EAST,
		NORTH, NORTH_WEST, NORTH_EAST
	};

class CSingleLevelMap
{
private:
	
	int level;
	unsigned int texture_list[10]; //이미지 매핑을 위한 배열
	bool load_Image[9]; //고도 데이터 Image Load 여부
	CGeoMapData *geoMaps[9];

	Point curImage; //현재 이미지의 번호

	const char* makeFileName(DIRECTION dir); //현재 좌표를 기준으로 방향 값에 해당하는 파일명 생성

	void mapMove(DIRECTION dir); //지도를 이동시킨다
	int mapCalculation(const Point& p);

public:
	CSingleLevelMap(int level = 1);
	~CSingleLevelMap();

	bool refresh(const Point& p);
	bool removeFile(DIRECTION dir); //파일 삭제

	bool removeAll();

	bool *getLoadImage();
	unsigned int *getTextList();
	CGeoMapData **getGeoMapData();

	bool openFile(DIRECTION dir); //파일 열기

	void setCurrentImage(Point &p);
	void setLevel(int level);
	Point getCurrentImage();
};

#endif