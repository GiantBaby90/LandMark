#include "CSingleLevelMap.h"
#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

CSingleLevelMap::CSingleLevelMap(int level)
{
	this->level = level;
	curImage.x = curImage.y =  1 << level;
	glGenTextures(10, texture_list);

	for(int i = 0; i < DIRECTION_COUNT; i++)
	{
		geoMaps[i] = new CGeoMapData();
		load_Image[i] = openFile((DIRECTION) i);
	}
}

CSingleLevelMap::~CSingleLevelMap()
{
	for(int i=0;i<DIRECTION_COUNT;i++)
		delete geoMaps[i];
}

#include <io.h>
bool CSingleLevelMap::openFile(DIRECTION dir)
{
	bool map_load = false;
	wchar_t server[256];
	wchar_t convert[256];

	char local[256];
	const char *fileName = makeFileName(dir);

	swprintf_s(convert, L"dem/%hs", fileName);
	swprintf_s(server, L"%hs", SERVER_PATH);
	_tcscat(server, convert);

	strcpy(local, DIRECTORY_PATH);
	strcat(local, "dem/");
	strcat(local, fileName);	
	swprintf_s(convert, L"%hs", local);

	URLDownloadToFile ( NULL, server, convert, 0, NULL );
	
	if(access(local, 0) == 0 && !geoMaps[(int) dir]->openFile(local)) //고도 데이터가 존재하지 않을 경우
		map_load = false;

	else //고도 데이터가 존재할 경우 위성 사진을 텍스쳐 매핑 준비
	{
		const unsigned char *image;
		CGeoMapData temp;
		int nXSize;
		int nYSize;

		map_load = true;
		swprintf_s(convert, L"%hs", fileName);
		swprintf_s(server, L"%hsimg/", SERVER_PATH);
		_tcscat(server, convert);

		strcpy(local, DIRECTORY_PATH);
		strcat(local, "img/");
		strcat(local, fileName);	
		swprintf_s(convert, L"%hs", local);

		URLDownloadToFile ( NULL, server, convert, 0, NULL );

		if(access(local, 0) == 0 && !temp.openFile(local))
		{
			image = geoMaps[dir]->GetGeoBuffer();
			nXSize = geoMaps[dir]->GetXSize();
			nYSize = geoMaps[dir]->GetYSize();
		}
		else
		{
			image = temp.GetGeoBuffer();
			nXSize = temp.GetXSize();
			nYSize = temp.GetYSize();
		}
		glBindTexture(GL_TEXTURE_2D, texture_list[dir]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nXSize, nYSize, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte *)image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		/*
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		*/
		
	}

	delete fileName;
	return map_load;
}

const char* CSingleLevelMap::makeFileName(DIRECTION dir)
{
	char *fileName = new char[256];
	Point tempPoint(curImage);
	int convert_DIR = (int) dir;

	if(convert_DIR > SOUTH_EAST) //SOUTH_EAST(5)보다 크면 NORTH(6)이므로, y값 증가
	{
		tempPoint.y++;
		convert_DIR = convert_DIR - NORTH;
	}
	else if(convert_DIR > EAST)//NORTH(6)보다 작고, EAST(2)보다 크면 SOUTH(3)이므로, y값 감소
	{
		tempPoint.y--;
		convert_DIR = convert_DIR - SOUTH;
	}

	switch(convert_DIR) //EAST와 WEST에 따라 x값 변화
	{
	case EAST:
		tempPoint.x++;
		break;
	case WEST:
		tempPoint.x--;
		break;
	default:
		break;
	}
	
	char temp[10];

	sprintf(temp,"%d/", level);
	strcpy(fileName, temp);

	sprintf(temp,"%d_", (int) tempPoint.x);
	strcat(fileName, temp);

	sprintf(temp,"%d", (int) tempPoint.y);
	strcat(fileName, temp);

	strcat(fileName, ".png");
	return fileName;
}

int CSingleLevelMap::mapCalculation(const Point& p)
{
	int dir = 0;
	if(p.x > curImage.x) dir += (int) EAST;
	if(p.y > curImage.y) dir += (int) NORTH;
	if(p.x < curImage.x) dir += (int) WEST;
	if(p.y < curImage.y) dir += (int) SOUTH;

	return dir;
}

bool CSingleLevelMap::refresh(const Point& p)
{
	int dir = mapCalculation(p);
	switch(dir)
	{
	case NORTH: //북쪽일 경우 모든 지형을 한칸씩 내리고 새로운 지형을 지형 위에 형성
		mapMove(NORTH);

		removeFile(NORTH);
		load_Image[NORTH] = openFile(NORTH);

		removeFile(NORTH_WEST);
		load_Image[NORTH_WEST] = openFile(NORTH_WEST);

		removeFile(NORTH_EAST);
		load_Image[NORTH_EAST] = openFile(NORTH_EAST);
		break;

	case NORTH_WEST:
		mapMove(NORTH);
		mapMove(WEST);

		removeFile(NORTH);
		load_Image[NORTH] = openFile(NORTH);

		removeFile(NORTH_EAST);
		load_Image[NORTH_EAST] = openFile(NORTH_EAST);

		removeFile(NORTH_WEST);
		load_Image[NORTH_WEST] = openFile(NORTH_WEST);

		removeFile(WEST);
		load_Image[WEST] = openFile(WEST);

		removeFile(SOUTH_WEST);
		load_Image[SOUTH_WEST] = openFile(SOUTH_WEST);
		break;

	case NORTH_EAST:
		mapMove(NORTH);
		mapMove(EAST);

		removeFile(NORTH);
		load_Image[NORTH] = openFile(NORTH);

		removeFile(NORTH_WEST);
		load_Image[NORTH_WEST] = openFile(NORTH_WEST);

		removeFile(NORTH_EAST);
		load_Image[NORTH_EAST] = openFile(NORTH_EAST);

		removeFile(EAST);
		load_Image[EAST] = openFile(EAST);

		removeFile(SOUTH_EAST);
		load_Image[SOUTH_EAST] = openFile(SOUTH_EAST);
		break;
		
	case SOUTH:
		mapMove(SOUTH);

		removeFile(SOUTH);
		load_Image[SOUTH] = openFile(SOUTH);

		removeFile(SOUTH_WEST);
		load_Image[SOUTH_WEST] = openFile(SOUTH_WEST);

		removeFile(SOUTH_EAST);
		load_Image[SOUTH_EAST] = openFile(SOUTH_EAST);
		break;

	case SOUTH_WEST:
		mapMove(SOUTH);
		mapMove(WEST);

		removeFile(SOUTH);
		load_Image[SOUTH] = openFile(SOUTH);

		removeFile(SOUTH_EAST);
		load_Image[SOUTH_EAST] = openFile(SOUTH_EAST);

		removeFile(SOUTH_WEST);
		load_Image[SOUTH_WEST] = openFile(SOUTH_WEST);

		removeFile(WEST);
		load_Image[WEST] = openFile(WEST);

		removeFile(NORTH_WEST);
		load_Image[NORTH_WEST] = openFile(NORTH_WEST);
		break;

	case SOUTH_EAST:
		mapMove(SOUTH);
		mapMove(EAST);

		removeFile(SOUTH);
		load_Image[SOUTH] = openFile(SOUTH);

		removeFile(SOUTH_WEST);
		load_Image[SOUTH_WEST] = openFile(SOUTH_WEST);

		removeFile(SOUTH_EAST);
		load_Image[SOUTH_EAST] = openFile(SOUTH_EAST);

		removeFile(EAST);
		load_Image[EAST] = openFile(EAST);

		removeFile(NORTH_EAST);
		load_Image[NORTH_EAST] = openFile(NORTH_EAST);
		break;

	case WEST:
		mapMove(WEST);

		removeFile(WEST);
		load_Image[WEST] = openFile(WEST);

		removeFile(NORTH_WEST);
		load_Image[NORTH_WEST] = openFile(NORTH_WEST);

		removeFile(SOUTH_WEST);
		load_Image[SOUTH_WEST] = openFile(SOUTH_WEST);
		break;

	case EAST:
		mapMove(EAST);

		removeFile(EAST);
		load_Image[EAST] = openFile(EAST);

		removeFile(NORTH_EAST);
		load_Image[NORTH_EAST] = openFile(NORTH_EAST);

		removeFile(SOUTH_EAST);
		load_Image[SOUTH_EAST] = openFile(SOUTH_EAST);
		break;

	default :
		break;
	}
	return true;
}

void CSingleLevelMap::mapMove(DIRECTION dir)
{
	CGeoMapData *temp;
	int texture_move;
	int load_img;

	switch(dir)
	{
	case NORTH:
		temp = geoMaps[SOUTH];
		geoMaps[SOUTH] = geoMaps[CUR];
		geoMaps[CUR] = geoMaps[NORTH];
		geoMaps[NORTH] = temp;

		temp = geoMaps[SOUTH_EAST];
		geoMaps[SOUTH_EAST] = geoMaps[EAST];
		geoMaps[EAST] = geoMaps[NORTH_EAST];
		geoMaps[NORTH_EAST] = temp;

		temp = geoMaps[SOUTH_WEST];
		geoMaps[SOUTH_WEST] = geoMaps[WEST];
		geoMaps[WEST] = geoMaps[NORTH_WEST];
		geoMaps[NORTH_WEST] = temp;

		texture_move = texture_list[SOUTH];
		texture_list[SOUTH] = texture_list[CUR];
		texture_list[CUR] = texture_list[NORTH];
		texture_list[NORTH] = texture_move;

		texture_move = texture_list[SOUTH_EAST];
		texture_list[SOUTH_EAST] = texture_list[EAST];
		texture_list[EAST] = texture_list[NORTH_EAST];
		texture_list[NORTH_EAST] = texture_move;

		texture_move = texture_list[SOUTH_WEST];
		texture_list[SOUTH_WEST] = texture_list[WEST];
		texture_list[WEST] = texture_list[NORTH_WEST];
		texture_list[NORTH_WEST] = texture_move;

		load_img = load_Image[SOUTH];
		load_Image[SOUTH] = load_Image[CUR];
		load_Image[CUR] = load_Image[NORTH];
		load_Image[NORTH] = load_img;

		load_img = load_Image[SOUTH_EAST];
		load_Image[SOUTH_EAST] = load_Image[EAST];
		load_Image[EAST] = load_Image[NORTH_EAST];
		load_Image[NORTH_EAST] = load_img;

		load_img = load_Image[SOUTH_WEST];
		load_Image[SOUTH_WEST] = load_Image[WEST];
		load_Image[WEST] = load_Image[NORTH_WEST];
		load_Image[NORTH_WEST] = load_img;

		curImage.y ++;
		break;

	case SOUTH:
		temp = geoMaps[NORTH];
		geoMaps[NORTH] = geoMaps[CUR];
		geoMaps[CUR] = geoMaps[SOUTH];
		geoMaps[SOUTH] = temp;

		temp = geoMaps[NORTH_EAST];
		geoMaps[NORTH_EAST] = geoMaps[EAST];
		geoMaps[EAST] = geoMaps[SOUTH_EAST];
		geoMaps[SOUTH_EAST] = temp;

		temp = geoMaps[NORTH_WEST];
		geoMaps[NORTH_WEST] = geoMaps[WEST];
		geoMaps[WEST] = geoMaps[SOUTH_WEST];
		geoMaps[SOUTH_WEST] = temp;

		texture_move = texture_list[NORTH];
		texture_list[NORTH] = texture_list[CUR];
		texture_list[CUR] = texture_list[SOUTH];
		texture_list[SOUTH] = texture_move;

		texture_move = texture_list[NORTH_EAST];
		texture_list[NORTH_EAST] = texture_list[EAST];
		texture_list[EAST] = texture_list[SOUTH_EAST];
		texture_list[SOUTH_EAST] = texture_move;

		texture_move = texture_list[NORTH_WEST];
		texture_list[NORTH_WEST] = texture_list[WEST];
		texture_list[WEST] = texture_list[SOUTH_WEST];
		texture_list[SOUTH_WEST] = texture_move;

		load_img = load_Image[NORTH];
		load_Image[NORTH] = load_Image[CUR];
		load_Image[CUR] = load_Image[SOUTH];
		load_Image[SOUTH] = load_img;

		load_img = load_Image[NORTH_EAST];
		load_Image[NORTH_EAST] = load_Image[EAST];
		load_Image[EAST] = load_Image[SOUTH_EAST];
		load_Image[SOUTH_EAST] = load_img;

		load_img = load_Image[NORTH_WEST];
		load_Image[NORTH_WEST] = load_Image[WEST];
		load_Image[WEST] = load_Image[SOUTH_WEST];
		load_Image[SOUTH_WEST] = load_img;

		curImage.y --;
		break;

	case EAST:
		temp = geoMaps[WEST];
		geoMaps[WEST] = geoMaps[CUR];
		geoMaps[CUR] = geoMaps[EAST];
		geoMaps[EAST] = temp;

		temp = geoMaps[NORTH_WEST];
		geoMaps[NORTH_WEST] = geoMaps[NORTH];
		geoMaps[NORTH] = geoMaps[NORTH_EAST];
		geoMaps[NORTH_EAST] = temp;

		temp = geoMaps[SOUTH_WEST];
		geoMaps[SOUTH_WEST] = geoMaps[SOUTH];
		geoMaps[SOUTH] = geoMaps[SOUTH_EAST];
		geoMaps[SOUTH_EAST] = temp;

		texture_move = texture_list[WEST];
		texture_list[WEST] = texture_list[CUR];
		texture_list[CUR] = texture_list[EAST];
		texture_list[EAST] = texture_move;

		texture_move = texture_list[SOUTH_WEST];
		texture_list[SOUTH_WEST] = texture_list[SOUTH];
		texture_list[SOUTH] = texture_list[SOUTH_EAST];
		texture_list[SOUTH_EAST] = texture_move;

		texture_move = texture_list[NORTH_WEST];
		texture_list[NORTH_WEST] = texture_list[NORTH];
		texture_list[NORTH] = texture_list[NORTH_EAST];
		texture_list[NORTH_EAST] = texture_move;

		load_img = load_Image[WEST];
		load_Image[WEST] = load_Image[CUR];
		load_Image[CUR] = load_Image[EAST];
		load_Image[EAST] = load_img;

		load_img = load_Image[SOUTH_WEST];
		load_Image[SOUTH_WEST] = load_Image[SOUTH];
		load_Image[SOUTH] = load_Image[SOUTH_EAST];
		load_Image[SOUTH_EAST] = load_img;

		load_img = load_Image[NORTH_WEST];
		load_Image[NORTH_WEST] = load_Image[NORTH];
		load_Image[NORTH] = load_Image[NORTH_EAST];
		load_Image[NORTH_EAST] = load_img;

		curImage.x ++;
		break;

	case WEST:
		temp = geoMaps[EAST];
		geoMaps[EAST] = geoMaps[CUR];
		geoMaps[CUR] = geoMaps[WEST];
		geoMaps[WEST] = temp;

		temp = geoMaps[NORTH_EAST];
		geoMaps[NORTH_EAST] = geoMaps[NORTH];
		geoMaps[NORTH] = geoMaps[NORTH_WEST];
		geoMaps[NORTH_WEST] = temp;

		temp = geoMaps[SOUTH_EAST];
		geoMaps[SOUTH_EAST] = geoMaps[SOUTH];
		geoMaps[SOUTH] = geoMaps[SOUTH_WEST];
		geoMaps[SOUTH_WEST] = temp;


		texture_move = texture_list[EAST];
		texture_list[EAST] = texture_list[CUR];
		texture_list[CUR] = texture_list[WEST];
		texture_list[WEST] = texture_move;

		texture_move = texture_list[NORTH_EAST];
		texture_list[NORTH_EAST] = texture_list[NORTH];
		texture_list[NORTH] = texture_list[NORTH_WEST];
		texture_list[NORTH_WEST] = texture_move;

		texture_move = texture_list[SOUTH_EAST];
		texture_list[SOUTH_EAST] = texture_list[SOUTH];
		texture_list[SOUTH] = texture_list[SOUTH_WEST];
		texture_list[SOUTH_WEST] = texture_move;

		load_img = load_Image[EAST];
		load_Image[EAST] = load_Image[CUR];
		load_Image[CUR] = load_Image[WEST];
		load_Image[WEST] = load_img;

		load_img = load_Image[SOUTH_EAST];
		load_Image[SOUTH_EAST] = load_Image[SOUTH];
		load_Image[SOUTH] = load_Image[SOUTH_WEST];
		load_Image[SOUTH_WEST] = load_img;

		load_img = load_Image[NORTH_EAST];
		load_Image[NORTH_EAST] = load_Image[NORTH];
		load_Image[NORTH] = load_Image[NORTH_WEST];
		load_Image[NORTH_WEST] = load_img;

		curImage.x --;
		break;

	default:
		break;
	}
}

#include <direct.h>
bool CSingleLevelMap::removeFile(DIRECTION dir)//파일 삭제
{
	geoMaps[dir]->removeFile();
	load_Image[dir] = false;
	return true;
}

bool *CSingleLevelMap::getLoadImage()
{
	return load_Image;
}
unsigned int *CSingleLevelMap::getTextList()
{
	return texture_list;
}

CGeoMapData **CSingleLevelMap::getGeoMapData()
{
	return geoMaps;
}

Point CSingleLevelMap::getCurrentImage()
{
	return curImage;
}

bool CSingleLevelMap::removeAll()
{
	for(int i=0;i<DIRECTION_COUNT;i++)
		removeFile((DIRECTION)i);

	return true;
}

void CSingleLevelMap::setCurrentImage(Point &p)
{
	curImage = p;
}

void CSingleLevelMap::setLevel(int level)
{
	this->level = level;
}