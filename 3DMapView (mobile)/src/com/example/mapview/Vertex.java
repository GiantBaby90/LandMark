package com.example.mapview;

public class Vertex {
	public 	float x, y, z;

	public 	Vertex()
	{
		x = y = z = 0.0f;
	}

	public 	Vertex(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	public 	Vertex assign(Vertex v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return this;
	}

	public 	Vertex add(Vertex v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return this;
	}
}
