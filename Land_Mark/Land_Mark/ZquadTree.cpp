#include "ZquadTree.h"
#include <iostream>
#include <cmath>

#define MIN_SIZE 2

ZQuadTree::ZQuadTree(int cx, int cy)
{
	m_pParent = NULL;
	for(int i=0;i<4;i++)
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}
	m_nCorner[CORNER_TL].x = m_nCorner[CORNER_TL].y = 0;
	m_nCorner[CORNER_TR].x = cx; m_nCorner[CORNER_TR].y = 0;
	m_nCorner[CORNER_BL].x = 0; m_nCorner[CORNER_BL].y = cy;
	m_nCorner[CORNER_BR].x = cx; m_nCorner[CORNER_BR].y = cy;

	m_nCenter.x = cx/2;
	m_nCenter.y = cy/2;
	m_fRadius = 0.0f;
	m_bCulled = false;
}

ZQuadTree::ZQuadTree(ZQuadTree* pParent)
{
	m_pParent = NULL;
	m_nCenter.x = m_nCenter.y = 0;
	for(int i = 0; i < 4; i++)
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
		m_nCorner[i].x = m_nCorner[i].y = 0;
	}

	m_fRadius = 0.0f;
	m_bCulled = false;
}

ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

bool ZQuadTree::_IsVisible(CGeoMapData &pHeightMap, Point &bottomLeft, Vertex &pCamera, float fLODratio)
{
	return ((m_nCorner[CORNER_TR].x - m_nCorner[CORNER_TL].x) <= _GetLODLevel(pHeightMap, bottomLeft, pCamera, fLODratio));
}

void ZQuadTree::_Destroy()
{
	for(int i=0; i<4; i++)	delete m_pChild[i];
}

bool ZQuadTree::_SetCorners(const Point & nCornerTL, const Point & nCornerTR, const Point & nCornerBL, const Point & nCornerBR)
{
	m_nCorner[CORNER_TL].x = nCornerTL.x;
	m_nCorner[CORNER_TL].y = nCornerTL.y;
	m_nCorner[CORNER_TR].x = nCornerTR.x;
	m_nCorner[CORNER_TR].y = nCornerTR.y;
	m_nCorner[CORNER_BL].x = nCornerBL.x;
	m_nCorner[CORNER_BL].y = nCornerBL.y;
	m_nCorner[CORNER_BR].x = nCornerBR.x;
	m_nCorner[CORNER_BR].y = nCornerBR.y;

	m_nCenter.x = (nCornerTL.x + nCornerTR.x)/2;
	m_nCenter.y = (nCornerBL.y + nCornerTL.y)/2;

	return true;
}

ZQuadTree* ZQuadTree::_AddChild(const Point & nCornerTL, const Point & nCornerTR, const Point & nCornerBL, const Point & nCornerBR)
{
	ZQuadTree* pChild;

	pChild = new ZQuadTree(this);
	pChild -> _SetCorners(nCornerTL, nCornerTR, nCornerBL, nCornerBR);

	return pChild;
}

bool ZQuadTree::_SubDivide()
{
	Point nTopEdgeCenter;
	Point nBottomEdgeCenter;
	Point nLeftEdgeCenter;
	Point nRightEdgeCenter;
	Point nCentralPoint;

	if(m_nCorner[CORNER_TR].x - m_nCorner[CORNER_TL].x <= MIN_SIZE)
		return false;

	//상단변 가운데
	nTopEdgeCenter.x = (m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x)/2;
	nTopEdgeCenter.y = m_nCorner[CORNER_TL].y;

	//하단변 가운데
	nBottomEdgeCenter.x = (m_nCorner[CORNER_BL].x + m_nCorner[CORNER_BR].x)/2;
	nBottomEdgeCenter.y = m_nCorner[CORNER_BL].y;

	//좌측변 가운데
	nLeftEdgeCenter.x = m_nCorner[CORNER_TL].x;
	nLeftEdgeCenter.y = (m_nCorner[CORNER_TL].y + m_nCorner[CORNER_BL].y)/2;

	//우측변 가운데
	nRightEdgeCenter.x = m_nCorner[CORNER_TR].x;
	nRightEdgeCenter.y = (m_nCorner[CORNER_TR].y + m_nCorner[CORNER_BR].y)/2;

	nCentralPoint.x = (m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x)/2;
	nCentralPoint.y = (m_nCorner[CORNER_TR].y + m_nCorner[CORNER_BR].y)/2;

	//4개의 자식노드 추가
	m_pChild[CORNER_TL] = _AddChild(m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint);
	m_pChild[CORNER_TR] = _AddChild(nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter);
	m_pChild[CORNER_BL] = _AddChild(nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter);
	m_pChild[CORNER_BR] = _AddChild(nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR]);

	return true;
}

int ZQuadTree::_GenTriIndex(int nTris, std::vector<Point> &vArray, CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum &pFrustum, float fLODRatio)
{
	if(m_bCulled)
	{
		//printf("%d %d bCulled\n", m_nCenter.x, m_nCenter.y);
		m_bCulled = false;
		return nTris;
	}

	if(_IsVisible(pHeightMap, bottomLeft, pFrustum.GetCamera(), fLODRatio))
	{
		if(m_nCorner[CORNER_TR].x - m_nCorner[CORNER_TL].x <= MIN_SIZE)
		{
			//좌측 상단 삼각형;
			nTris++;
			vArray.push_back(m_nCorner[CORNER_TL]);
			vArray.push_back(m_nCorner[CORNER_TR]);
			vArray.push_back(m_nCorner[CORNER_BL]);

			//우측 하단 삼각형
			nTris++;
			vArray.push_back(bottomLeft + m_nCorner[CORNER_BL]);
			vArray.push_back(bottomLeft + m_nCorner[CORNER_TR]);
			vArray.push_back(bottomLeft + m_nCorner[CORNER_BR]);

			return nTris;
		}

		bool b[4] = {true, true, true, true};

		if(m_pNeighbor[EDGE_UP]) b[EDGE_UP] = m_pNeighbor[EDGE_UP] ->_IsVisible(pHeightMap, bottomLeft, pFrustum.GetCamera(), fLODRatio);
		if(m_pNeighbor[EDGE_DN]) b[EDGE_DN] = m_pNeighbor[EDGE_DN] ->_IsVisible(pHeightMap, bottomLeft, pFrustum.GetCamera(), fLODRatio);
		if(m_pNeighbor[EDGE_LT]) b[EDGE_LT] = m_pNeighbor[EDGE_LT] ->_IsVisible(pHeightMap, bottomLeft, pFrustum.GetCamera(), fLODRatio);
		if(m_pNeighbor[EDGE_RT]) b[EDGE_RT] = m_pNeighbor[EDGE_RT] ->_IsVisible(pHeightMap, bottomLeft, pFrustum.GetCamera(), fLODRatio);

		if(b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT])
		{
			//좌측 상단 삼각형;
			nTris++;
			vArray.push_back(m_nCorner[CORNER_TL]);
			vArray.push_back(m_nCorner[CORNER_TR]);
			vArray.push_back(m_nCorner[CORNER_BL]);

			//우측 하단 삼각형
			nTris++;
			vArray.push_back(m_nCorner[CORNER_BL]);
			vArray.push_back(m_nCorner[CORNER_TR]);
			vArray.push_back(m_nCorner[CORNER_BR]);

			return nTris;
		}

		Point p;
		if(!b[EDGE_UP])
		{
			p.x = (m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x) / 2;
			p.y = m_nCorner[CORNER_TL].y;

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_TL]);
			vArray.push_back( p);

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(p);
			vArray.push_back(m_nCorner[CORNER_TR]);
		}
		else
		{
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_TL]);
			vArray.push_back(m_nCorner[CORNER_TR]);
		}

		if(!b[EDGE_DN])
		{
			p.x = (m_nCorner[CORNER_BL].x + m_nCorner[CORNER_BR].x) / 2;
			p.y = m_nCorner[CORNER_BL].y;

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_BR]);
			vArray.push_back(p);

			nTris++;
			vArray.push_back(bottomLeft + m_nCenter);
			vArray.push_back(p);
			vArray.push_back(m_nCorner[CORNER_BL]);
		}
		else
		{
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_BR]);
			vArray.push_back(m_nCorner[CORNER_BL]);
		}

		if(!b[EDGE_LT])
		{
			p.x = m_nCorner[CORNER_TL].x;
			p.y = (m_nCorner[CORNER_TL].y + m_nCorner[CORNER_BL].y) / 2;

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_BL]);
			vArray.push_back(p);

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(p);
			vArray.push_back(m_nCorner[CORNER_TL]);
		}
		else
		{
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_BL]);
			vArray.push_back(m_nCorner[CORNER_TL]);
		}

		if(!b[EDGE_RT])
		{
			p.x = m_nCorner[CORNER_TR].x;
			p.y = (m_nCorner[CORNER_TR].y + m_nCorner[CORNER_BR].y) / 2;

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_TR]);
			vArray.push_back(p);

			nTris++;
			vArray.push_back(m_nCenter);
			vArray.push_back(p);
			vArray.push_back(m_nCorner[CORNER_BR]);
		}
		else
		{
			vArray.push_back(m_nCenter);
			vArray.push_back(m_nCorner[CORNER_TR]);
			vArray.push_back(m_nCorner[CORNER_BR]);
		}

		return nTris;
	}

	if(m_pChild[CORNER_TL]) nTris = m_pChild[CORNER_TL] -> _GenTriIndex(nTris, vArray, pHeightMap, bottomLeft, pFrustum, fLODRatio);
	if(m_pChild[CORNER_TR]) nTris = m_pChild[CORNER_TR] -> _GenTriIndex(nTris, vArray, pHeightMap, bottomLeft, pFrustum, fLODRatio);
	if(m_pChild[CORNER_BL]) nTris = m_pChild[CORNER_BL] -> _GenTriIndex(nTris, vArray, pHeightMap, bottomLeft, pFrustum, fLODRatio);
	if(m_pChild[CORNER_BR]) nTris = m_pChild[CORNER_BR] -> _GenTriIndex(nTris, vArray, pHeightMap, bottomLeft, pFrustum, fLODRatio);
	
	return nTris;
}

bool ZQuadTree::_BuildQuadTree(CGeoMapData &pHeightMap, Point &bottomLeft)
{
	const unsigned char *buffer = pHeightMap.GetGeoBuffer();
	int nXSize = pHeightMap.GetXSize();
	int nBands = pHeightMap.GetNBands();

	m_fRadius = _GetDistance(Vertex (bottomLeft.x + m_nCorner[CORNER_TL].x, bottomLeft.y + m_nCorner[CORNER_TL].y, 0),
		Vertex(bottomLeft.x + m_nCorner[CORNER_BR].x, bottomLeft.y + m_nCorner[CORNER_BR].y, 0)) / 2.0f;

	if(_SubDivide())
	{
		m_pChild[CORNER_TL] -> _BuildQuadTree(pHeightMap, bottomLeft);
		m_pChild[CORNER_TR] -> _BuildQuadTree(pHeightMap, bottomLeft);
		m_pChild[CORNER_BL] -> _BuildQuadTree(pHeightMap, bottomLeft);
		m_pChild[CORNER_BR] -> _BuildQuadTree(pHeightMap, bottomLeft);
	}

	return true;
}

bool ZQuadTree::Build(CGeoMapData &pHeightMap, Point &bottomLeft)
{
	_BuildQuadTree(pHeightMap, bottomLeft);

	_BuildNeighborNode(this, pHeightMap);
	return true;
}

int ZQuadTree::GenerateIndex(std::vector<Point> &vArray,CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum, float fLODRatio)
{
	_FrustumCull(pHeightMap, bottomLeft, pFrustum);
	return _GenTriIndex(0, vArray, pHeightMap, bottomLeft, pFrustum, fLODRatio);
}

int ZQuadTree::_IsInFrustum(CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum)
{
	bool b[4];
	bool bInSphere;

	bInSphere = pFrustum.SphereInFrustum(bottomLeft.x + m_nCenter.x, bottomLeft.y + m_nCenter.y, 0, m_fRadius);
	if(!bInSphere) return FRUSTUM_OUT;

	b[0] = pFrustum.PointInFrustum(bottomLeft.x + m_nCorner[0].x, bottomLeft.y + m_nCorner[0].y, 0);
	b[1] = pFrustum.PointInFrustum(bottomLeft.x + m_nCorner[1].x, bottomLeft.y + m_nCorner[1].y, 0);
	b[2] = pFrustum.PointInFrustum(bottomLeft.x + m_nCorner[2].x, bottomLeft.y + m_nCorner[2].y, 0);
	b[3] = pFrustum.PointInFrustum(bottomLeft.x + m_nCorner[3].x, bottomLeft.y + m_nCorner[3].y, 0);

	if(b[0] + b[1] + b[2] + b[3] == 4)
		return FRUSTUM_COMPLETELY_IN;

	return FRUSTUM_PRTIALLY_IN;
}

void ZQuadTree::_FrustumCull(CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum)
{
	int ret;

	ret = _IsInFrustum(pHeightMap, bottomLeft, pFrustum);
	
	switch(ret)
	{
	case FRUSTUM_COMPLETELY_IN:
		m_bCulled = false;
		return;

	case FRUSTUM_PRTIALLY_IN:
		m_bCulled = false;
		break;

	case FRUSTUM_OUT:
		m_bCulled = true;
		return;
	}

	if(m_pChild[0]) m_pChild[0] ->_FrustumCull(pHeightMap, bottomLeft, pFrustum);
	if(m_pChild[1]) m_pChild[1] ->_FrustumCull(pHeightMap, bottomLeft, pFrustum);
	if(m_pChild[2]) m_pChild[2] ->_FrustumCull(pHeightMap, bottomLeft, pFrustum);
	if(m_pChild[3]) m_pChild[3] ->_FrustumCull(pHeightMap, bottomLeft, pFrustum);
}

float ZQuadTree::_GetDistance(Vertex &v1, Vertex &v2)
{
	float x = (v1.x - v2.x) * (v1.x - v2.x);
	float y = (v1.y - v2.y) * (v1.y - v2.y);
	float z = (v1.z - v2.z) * (v1.z - v2.z);

	return sqrt(x+y+z);
}

int ZQuadTree::_GetNodeIndex(int i, CGeoMapData& pHeightMap, Point& _0, Point& _1, Point& _2, Point& _3)
{

	Point _a, _b, _c, _d;
	int gapX, gapY;

	_a = _0;
	_b = _1;
	_c = _2;
	_d = _3;
	gapX = _b.x - _a.x;
	gapY = _c.y - _a.y;

	switch(i)
	{
	case EDGE_UP:
		_0 = Point(_a.x, (_a.y - gapY));
		_1 = Point(_b.x, (_b.y - gapY));
		_2 = _a;
		_3 = _b;
		break;
	case EDGE_DN:
		_0 = _c;
		_1 = _d;
		_2 = Point(_c.x, (_c.y + gapY));
		_3 = Point(_d.x, (_d.y + gapY));
		break;
	case EDGE_LT:
		_0 = Point((_a.x - gapX), _a.y);
		_1 = _a;
		_2 = Point((_c.x - gapX), _c.y);
		_3 = _c;
		break;
	case EDGE_RT:
		_0 = _b;
		_1 = Point((_b.x + gapX), _b.y);
		_2 = _d;
		_3 = Point((_d.x + gapX), _d.y);
		break;
	}
	if(!_IsInRect(Point(0,0), Point(pHeightMap.GetXSize(), pHeightMap.GetYSize()), Point((_0.x + _1.x)/2, (_0.y + _2.y)/2))) return false;
	return true;
}

void ZQuadTree::_BuildNeighborNode(ZQuadTree *pRoot, CGeoMapData &pHeightMap)
{
	int n;
	Point _0, _1, _2, _3;

	for(int i=0;i<4;i++)
	{
		_0 = m_nCorner[CORNER_TL];
		_1 = m_nCorner[CORNER_TR];
		_2 = m_nCorner[CORNER_BL];
		_3 = m_nCorner[CORNER_BR];

		n = _GetNodeIndex(i, pHeightMap, _0, _1, _2, _3);
		if(n) m_pNeighbor[i] = pRoot -> _FindNode(pHeightMap, _0, _1, _2, _3);
	}

	if(m_pChild[0])
	{
		m_pChild[0] ->_BuildNeighborNode(pRoot, pHeightMap);
		m_pChild[1] ->_BuildNeighborNode(pRoot, pHeightMap);
		m_pChild[2] ->_BuildNeighborNode(pRoot, pHeightMap);
		m_pChild[3] ->_BuildNeighborNode(pRoot, pHeightMap);
	}
}

ZQuadTree *ZQuadTree::_FindNode(CGeoMapData &pHeightMap, Point &_0, Point &_1, Point &_2, Point &_3)
{
	ZQuadTree *p = NULL;

	if(m_nCorner[CORNER_TL] == _0 && m_nCorner[CORNER_TR] == _1 && m_nCorner[CORNER_BL] == _2 && m_nCorner[CORNER_BR] == _3)
		return this;

	if(m_pChild[0])
	{
		Point pt((_0.x + _1.x)/2, (_0.y + _2.y)/2);
		//Point pt((m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x)/2, (m_nCorner[CORNER_TL].y + m_nCorner[CORNER_BL].y)/2);

		if(_IsInRect(m_pChild[CORNER_TL]->m_nCorner[CORNER_TL], m_pChild[CORNER_TL]->m_nCorner[CORNER_BR], pt))
			return m_pChild[CORNER_TL]->_FindNode(pHeightMap, _0, _1, _2, _3);

		if(_IsInRect(m_pChild[CORNER_TR]->m_nCorner[CORNER_TL], m_pChild[CORNER_TR]->m_nCorner[CORNER_BR], pt))
			return m_pChild[CORNER_TR]->_FindNode(pHeightMap, _0, _1, _2, _3);

		if(_IsInRect(m_pChild[CORNER_BL]->m_nCorner[CORNER_TL], m_pChild[CORNER_BL]->m_nCorner[CORNER_BR], pt))
			return m_pChild[CORNER_BL]->_FindNode(pHeightMap, _0, _1, _2, _3);

		if(_IsInRect(m_pChild[CORNER_BR]->m_nCorner[CORNER_TL], m_pChild[CORNER_BR]->m_nCorner[CORNER_BR], pt))
			return m_pChild[CORNER_BR]->_FindNode(pHeightMap, _0, _1, _2, _3);

	}
	return NULL;
}

bool ZQuadTree::_IsInRect(Point &TL, Point &BR, Point &target)
{
	if(TL.x - 1 < target.x && BR.x + 1 > target.x && TL.y - 1 < target.y && BR.y + 1 > target.y)
		return true;

	return false;
}

int ZQuadTree::_GetLODLevel(CGeoMapData &pHeightMap, Point &bottomLeft, Vertex &pCamera, float fLODRatio)
{
	float d = _GetDistance(Vertex(bottomLeft.x + m_nCenter.x,bottomLeft.y + m_nCenter.y, 0),pCamera);
	return MAX((int) (d * fLODRatio), MIN_SIZE);
}

