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
	unsigned int texture_list[10]; //�̹��� ������ ���� �迭
	bool load_Image[9]; //�� ������ Image Load ����
	CGeoMapData *geoMaps[9];

	Point curImage; //���� �̹����� ��ȣ

	const char* makeFileName(DIRECTION dir); //���� ��ǥ�� �������� ���� ���� �ش��ϴ� ���ϸ� ����

	void mapMove(DIRECTION dir); //������ �̵���Ų��
	int mapCalculation(const Point& p);

public:
	CSingleLevelMap(int level = 1);
	~CSingleLevelMap();

	bool refresh(const Point& p);
	bool removeFile(DIRECTION dir); //���� ����

	bool removeAll();

	bool *getLoadImage();
	unsigned int *getTextList();
	CGeoMapData **getGeoMapData();

	bool openFile(DIRECTION dir); //���� ����

	void setCurrentImage(Point &p);
	void setLevel(int level);
	Point getCurrentImage();
};

#endif