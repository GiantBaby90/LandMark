package com.example.mapview;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import org.gdal.gdal.*;
import org.gdal.gdalconst.*;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class GeoMapData {

	private	byte[] imageBuf;
	private	Dataset srcDataset=null;

	
	public	boolean openFile(String filePath)
	{
		if(srcDataset !=null)	return true;
		
		srcDataset = new Dataset(gdalJNI.Open__SWIG_1(filePath),true);
		
		if(srcDataset ==null) return false;
				
		
		int nXSize = GetXSize();
		int nYSize = GetYSize();
		int nBands = GetNBands();
		

		imageBuf = new byte[(nXSize * nBands * nYSize)]; 

		for(int iLine = 0; iLine < nYSize; iLine++)
		{
			srcDataset.ReadRaster(0, iLine, nXSize, 1, 
					nXSize, 1, gdalconst.GDT_Byte, imageBuf,
					null, nBands, nXSize * nBands, 1 );

		}

		return true;
	}

	int GetXSize()
	{
		return srcDataset.GetRasterXSize();
	}

	int GetYSize()
	{
		return srcDataset.GetRasterYSize();
	}

	int GetNBands()
	{
		return srcDataset.GetRasterCount();
	}
	
	public GeoMapData()
	{
		imageBuf =null;
		/*  gdal
		srcDataset =null;
		gdal.AllRegister();
		*/
		
	}

	public GeoMapData(String filePath)
	{
		openFile(filePath);
		/*  gdal
		gdalJNI.AllRegister();
		gdal.AllRegister();
		*/
	}
	
	
	byte[] GetGeoBuffer()
	{
		return imageBuf;
	}


}
