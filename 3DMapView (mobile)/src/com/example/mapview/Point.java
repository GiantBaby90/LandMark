package com.example.mapview;

public class Point
{
	public int x, y;

	public Point()
	{
		x = y = 0;
	}

	public Point(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	public Point assign(Point p)
	{
		x = p.x;
		y = p.y;
		return this;
	}

	public Boolean equls(Point p)
	{
		if(p.x - 2 < x && p.y - 2 < y && p.x + 2 > x && p.y + 2 > y) return true;
		return false;
	}
}
