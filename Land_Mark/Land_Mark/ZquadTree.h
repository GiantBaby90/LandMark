#ifndef _Z_QUAD_TREE_H_
#define _Z_QUAD_TREE_H_

#include <vector>
#include "CPixel.h"
#include "CFrustum.h"
#include "CGeoMapData.h"

class ZQuadTree
{
	enum CornerType{CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR};
	enum Neighbor{EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT};

	enum QuadLocation{FRUSTUM_OUT = 0, //완전히 벗어남
						FRUSTUM_PRTIALLY_IN = 1, //일부분만 벗어남
						FRUSTUM_COMPLETELY_IN = 2}; // 완전히 포함됨


private:
	ZQuadTree *m_pChild[4];
	ZQuadTree *m_pParent;
	ZQuadTree *m_pNeighbor[4];

	Point m_nCenter;
	Point m_nCorner[4];

	bool m_bCulled; //절두체에서 컬링된 노드인가
	float m_fRadius; //노드를 감싸는 경계구

private:
	//자식 노드를 추가
	ZQuadTree *_AddChild(const Point &nCornerTL, const Point &nCornerTR, const Point &nCornerBL, const Point &nCornerBR);

	//4개의 코너값 세팅
	bool _SetCorners(const Point &nCornerTL, const Point &nCornerTR, const Point &nCornerBL, const Point &nCornerBR);

	//쿼드트리를 4개의 하위 트리로 하위 분할(subdivide)
	bool _SubDivide();

	//출력 가능한 노드 판별
	bool _IsVisible(CGeoMapData &pHeightMap, Point &bottomLeft, Vertex &pCamera, float fLODRatio);

	//출력할 폴리곤의 인덱스를 생성
	int _GenTriIndex(int nTriangles ,std::vector<Point> &vArray, CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum &pFrustum, float fLODRatio);
	
	//메모리에서 쿼드트리를 삭제
	void _Destroy();

	//두 점 사이의 거리를 계산
	float _GetDistance(Vertex &v1, Vertex &v2);

	//LOD값을 계산
	int _GetLODLevel(CGeoMapData &pHeightMap, Point &bottomLeft, Vertex &pCamera, float fLODRatio);

	//이웃노드 생성
	void _BuildNeighborNode(ZQuadTree* pRoot, CGeoMapData &pHeightMap);

	//쿼드트리 생성
	bool _BuildQuadTree(CGeoMapData &pHeightMap, Point &bottomLeft);

	//쿼드트리 노드 검색
	ZQuadTree* _FindNode(CGeoMapData &pHeightMap, Point &_0, Point &_1, Point &_2, Point &_3);

	//Node의 정보
	int _GetNodeIndex(int i, CGeoMapData& pHeightMap, Point& _0, Point& _1, Point& _2, Point& _3);

	//사각형 내에 있는지 판별
	bool _IsInRect(Point &TL, Point &BR, Point &target);

public:
	ZQuadTree(int cx = 512, int cy = 512);
	ZQuadTree(ZQuadTree *pParent);

	~ZQuadTree();

	//쿼드트리 구축
	bool Build(CGeoMapData &pHeightMap, Point &bottomLeft);

	//삼각형의 인덱스를 만들고, 출력할 삼각형의 개수를 반환
	int GenerateIndex(std::vector<Point> &vArray, CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum, float fLODRatio);

	int _IsInFrustum(CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum);
	void _FrustumCull(CGeoMapData &HeightMap, Point &bottomLeft, CFrustum& pFrustum);
};

//ZFrustum 384페이지

#endif