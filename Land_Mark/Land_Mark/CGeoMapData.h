#ifndef _CGEO_MAP_H_
#define _CGEO_MAP_H_

#include "gdal_priv.h"
#include "CPixel.h"

class CGeoMapData
{
private:
	unsigned char *imageBuf;
	GDALDataset *srcDataset;

	int nXSize; //추가
	int nYSize;
	int nBands;

public:
	CGeoMapData();
	CGeoMapData(const char *filePath);
	~CGeoMapData();
	
	bool openFile(const char *filePath);
	bool removeFile(); //추가
	unsigned char getPosition(int x, int y); //추가
	const unsigned char * GetGeoBuffer();

	int GetXSize();
	int GetYSize();
	int GetNBands();
};

#endif