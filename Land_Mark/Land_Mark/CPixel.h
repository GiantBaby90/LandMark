#ifndef _PIXEL_H_
#define _PIXEL_H_

class Point
{
public:
	int x, y;

	Point()
	{
		x = y = 0;
	}

	Point(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	Point &operator=(Point &p)
	{
		x = p.x;
		y = p.y;
		return *this;
	}

	Point operator+(Point &p)
	{
		return Point(x+p.x, y+p.y); 
	}

	Point operator-(Point &p)
	{
		return Point(x-p.x, y-p.y); 
	}

	bool operator==(Point &p)
	{
		if(p.x - 2 < x && p.y - 2 < y && p.x + 2 > x && p.y + 2 > y) return true;
		return false;
	}
};

class Vertex
{
public:
	float x, y, z;

	Vertex()
	{
		x = y = z = 0.0f;
	}

	Vertex(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};
#endif