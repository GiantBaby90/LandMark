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

	enum QuadLocation{FRUSTUM_OUT = 0, //������ ���
						FRUSTUM_PRTIALLY_IN = 1, //�Ϻκи� ���
						FRUSTUM_COMPLETELY_IN = 2}; // ������ ���Ե�


private:
	ZQuadTree *m_pChild[4];
	ZQuadTree *m_pParent;
	ZQuadTree *m_pNeighbor[4];

	Point m_nCenter;
	Point m_nCorner[4];

	bool m_bCulled; //����ü���� �ø��� ����ΰ�
	float m_fRadius; //��带 ���δ� ��豸

private:
	//�ڽ� ��带 �߰�
	ZQuadTree *_AddChild(const Point &nCornerTL, const Point &nCornerTR, const Point &nCornerBL, const Point &nCornerBR);

	//4���� �ڳʰ� ����
	bool _SetCorners(const Point &nCornerTL, const Point &nCornerTR, const Point &nCornerBL, const Point &nCornerBR);

	//����Ʈ���� 4���� ���� Ʈ���� ���� ����(subdivide)
	bool _SubDivide();

	//��� ������ ��� �Ǻ�
	bool _IsVisible(CGeoMapData &pHeightMap, Point &bottomLeft, Vertex &pCamera, float fLODRatio);

	//����� �������� �ε����� ����
	int _GenTriIndex(int nTriangles ,std::vector<Point> &vArray, CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum &pFrustum, float fLODRatio);
	
	//�޸𸮿��� ����Ʈ���� ����
	void _Destroy();

	//�� �� ������ �Ÿ��� ���
	float _GetDistance(Vertex &v1, Vertex &v2);

	//LOD���� ���
	int _GetLODLevel(CGeoMapData &pHeightMap, Point &bottomLeft, Vertex &pCamera, float fLODRatio);

	//�̿���� ����
	void _BuildNeighborNode(ZQuadTree* pRoot, CGeoMapData &pHeightMap);

	//����Ʈ�� ����
	bool _BuildQuadTree(CGeoMapData &pHeightMap, Point &bottomLeft);

	//����Ʈ�� ��� �˻�
	ZQuadTree* _FindNode(CGeoMapData &pHeightMap, Point &_0, Point &_1, Point &_2, Point &_3);

	//Node�� ����
	int _GetNodeIndex(int i, CGeoMapData& pHeightMap, Point& _0, Point& _1, Point& _2, Point& _3);

	//�簢�� ���� �ִ��� �Ǻ�
	bool _IsInRect(Point &TL, Point &BR, Point &target);

public:
	ZQuadTree(int cx = 512, int cy = 512);
	ZQuadTree(ZQuadTree *pParent);

	~ZQuadTree();

	//����Ʈ�� ����
	bool Build(CGeoMapData &pHeightMap, Point &bottomLeft);

	//�ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ
	int GenerateIndex(std::vector<Point> &vArray, CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum, float fLODRatio);

	int _IsInFrustum(CGeoMapData &pHeightMap, Point &bottomLeft, CFrustum& pFrustum);
	void _FrustumCull(CGeoMapData &HeightMap, Point &bottomLeft, CFrustum& pFrustum);
};

//ZFrustum 384������

#endif