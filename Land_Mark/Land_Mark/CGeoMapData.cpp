#include "CGeoMapData.h"

CGeoMapData::CGeoMapData()
{
	imageBuf = NULL;
	srcDataset = NULL;
	GDALAllRegister();
	nYSize = nBands = nXSize = 0;
	
}

CGeoMapData::CGeoMapData(const char* filePath)
{
	GDALAllRegister();
	openFile(filePath);
}

CGeoMapData::~CGeoMapData()
{
	removeFile();
}

bool CGeoMapData::openFile(const char *filePath)
{
	if(srcDataset)
	{
		CPLFree(imageBuf);
		GDALClose(srcDataset);
	}
	srcDataset = (GDALDataset *) GDALOpen(filePath, GA_ReadOnly);
	if(srcDataset == NULL) return false;

	nXSize = srcDataset->GetRasterXSize();
	nYSize = srcDataset->GetRasterYSize();
	nBands = srcDataset->GetRasterCount();

	imageBuf = (unsigned char *) CPLMalloc (nXSize * nBands * nYSize);

	for(int iLine = 0; iLine < nYSize; iLine++) //수정
	{
		GDALDatasetRasterIO( srcDataset, GF_Read, 0, iLine, nXSize, 1, 
			&imageBuf[(nYSize-iLine-1) * nXSize * nBands], nXSize, 1, GDT_Byte, 
			nBands, NULL, nBands, nXSize * nBands, 1 );
	}

	return true;
}

const unsigned char* CGeoMapData::GetGeoBuffer()
{
	return imageBuf;
}

int CGeoMapData::GetXSize() //변경
{
	return nXSize;
}

int CGeoMapData::GetYSize() //변경
{
	return nYSize;
}

int CGeoMapData::GetNBands() //변경
{
	return nBands;
}

bool CGeoMapData::removeFile()
{
	if(imageBuf)
	{
		CPLFree(imageBuf);
		imageBuf = NULL;
	}
	if(srcDataset)
	{
		GDALClose(srcDataset);
		srcDataset = NULL;
	}

	return true;
}

unsigned char CGeoMapData::getPosition(int x, int y)
{
	if(imageBuf == NULL) return 0;
	return imageBuf[nBands * (nXSize * y + x)];
}