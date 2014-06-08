package com.example.mapview;

import java.util.Vector;

public class ZQuadTree {
	private final int CORNER_TL = 0;
	private final int CORNER_TR = 1;
	private final int CORNER_BL = 2;
	private final int CORNER_BR = 3;

	private final int EDGE_UP = 0;
	private final int EDGE_DN = 1;
	private final int EDGE_LT = 2;
	private final int EDGE_RT = 3;

	private final int FRUSTUM_OUT = 0; // 완전히 벗어남
	private final int FRUSTUM_PRTIALLY_IN = 1; // 일부분만 벗어남
	private final int FRUSTUM_COMPLETELY_IN = 2; // 완전히 포함됨

	private final int MIN_SIZE = 32;

	ZQuadTree[] m_pChild = new ZQuadTree[4];
	ZQuadTree m_pParent;
	ZQuadTree[] m_pNeighbor = new ZQuadTree[4];

	Point m_nCenter = new Point();
	Point[] m_nCorner = new Point[4];

	boolean m_bCulled; // 절두체에서 컬링된 노드인가
	float m_fRadius; // 노드를 감싸는 경계구

	ZQuadTree(int cx, int cy) {
		m_pParent = null;
		for (int i = 0; i < 4; i++) {
			m_pChild[i] = null;
			m_pNeighbor[i] = null;
		}
		m_nCorner[CORNER_TL] = new Point(0, 0);
		m_nCorner[CORNER_TR] = new Point(cx, 0);
		m_nCorner[CORNER_BL] = new Point(0, cy);
		m_nCorner[CORNER_BR] = new Point(cx, cy);

		m_nCenter.x = cx / 2;
		m_nCenter.y = cy / 2;
		m_fRadius = 0.0f;
		m_bCulled = false;
	}

	ZQuadTree(ZQuadTree pParent) {
		m_pParent = null;
		m_nCenter.x = m_nCenter.y = 0;
		for (int i = 0; i < 4; i++) {
			m_pChild[i] = null;
			m_pNeighbor[i] = null;
			m_nCorner[i] = new Point(0, 0);
		}

		m_fRadius = 0.0f;
		m_bCulled = false;
	}

	boolean _IsVisible(Vertex pCamera, float fLODratio) {
		return ((m_nCorner[CORNER_TR].x - m_nCorner[CORNER_TL].x) <= _GetLODLevel(
				pCamera, fLODratio));
	}

	boolean _SetCorners(Point nCornerTL, Point nCornerTR, Point nCornerBL,
			Point nCornerBR) {
		m_nCorner[CORNER_TL].x = nCornerTL.x;
		m_nCorner[CORNER_TL].y = nCornerTL.y;
		m_nCorner[CORNER_TR].x = nCornerTR.x;
		m_nCorner[CORNER_TR].y = nCornerTR.y;
		m_nCorner[CORNER_BL].x = nCornerBL.x;
		m_nCorner[CORNER_BL].y = nCornerBL.y;
		m_nCorner[CORNER_BR].x = nCornerBR.x;
		m_nCorner[CORNER_BR].y = nCornerBR.y;

		m_nCenter.x = (nCornerTL.x + nCornerTR.x) / 2;
		m_nCenter.y = (nCornerBL.y + nCornerTL.y) / 2;

		return true;
	}

	ZQuadTree _AddChild(Point nCornerTL, Point nCornerTR, Point nCornerBL,
			Point nCornerBR) {
		ZQuadTree pChild;

		pChild = new ZQuadTree(this);
		pChild._SetCorners(nCornerTL, nCornerTR, nCornerBL, nCornerBR);

		return pChild;
	}

	boolean _SubDivide() {
		Point nTopEdgeCenter = new Point();
		Point nBottomEdgeCenter = new Point();
		Point nLeftEdgeCenter = new Point();
		Point nRightEdgeCenter = new Point();
		Point nCentralPoint = new Point();

		if (m_nCorner[CORNER_TR].x - m_nCorner[CORNER_TL].x <= MIN_SIZE)
			return false;

		// 상단변 가운데
		nTopEdgeCenter.x = (m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x) / 2;
		nTopEdgeCenter.y = m_nCorner[CORNER_TL].y;

		// 하단변 가운데
		nBottomEdgeCenter.x = (m_nCorner[CORNER_BL].x + m_nCorner[CORNER_BR].x) / 2;
		nBottomEdgeCenter.y = m_nCorner[CORNER_BL].y;

		// 좌측변 가운데
		nLeftEdgeCenter.x = m_nCorner[CORNER_TL].x;
		nLeftEdgeCenter.y = (m_nCorner[CORNER_TL].y + m_nCorner[CORNER_BL].y) / 2;

		// 우측변 가운데
		nRightEdgeCenter.x = m_nCorner[CORNER_TR].x;
		nRightEdgeCenter.y = (m_nCorner[CORNER_TR].y + m_nCorner[CORNER_BR].y) / 2;

		nCentralPoint.x = (m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x) / 2;
		nCentralPoint.y = (m_nCorner[CORNER_TR].y + m_nCorner[CORNER_BR].y) / 2;

		// 4개의 자식노드 추가
		m_pChild[CORNER_TL] = _AddChild(m_nCorner[CORNER_TL], nTopEdgeCenter,
				nLeftEdgeCenter, nCentralPoint);
		m_pChild[CORNER_TR] = _AddChild(nTopEdgeCenter, m_nCorner[CORNER_TR],
				nCentralPoint, nRightEdgeCenter);
		m_pChild[CORNER_BL] = _AddChild(nLeftEdgeCenter, nCentralPoint,
				m_nCorner[CORNER_BL], nBottomEdgeCenter);
		m_pChild[CORNER_BR] = _AddChild(nCentralPoint, nRightEdgeCenter,
				nBottomEdgeCenter, m_nCorner[CORNER_BR]);

		return true;
	}

	int _GenTriIndex(int nTris, Vector<Point> vArray, Frustum pFrustum,
			float fLODRatio) {
		if (m_bCulled) {
			// printf("%d %d bCulled\n", m_nCenter.x, m_nCenter.y);
			m_bCulled = false;
			return nTris;
		}

		if (_IsVisible(pFrustum.GetCamera(), fLODRatio)) {
			if (m_nCorner[CORNER_TR].x - m_nCorner[CORNER_TL].x <= MIN_SIZE) {
				// 좌측 상단 삼각형;
				nTris++;
				vArray.add(m_nCorner[CORNER_TL]);
				vArray.add(m_nCorner[CORNER_TR]);
				vArray.add(m_nCorner[CORNER_BL]);

				// 우측 하단 삼각형
				nTris++;
				vArray.add(m_nCorner[CORNER_BL]);
				vArray.add(m_nCorner[CORNER_TR]);
				vArray.add(m_nCorner[CORNER_BR]);

				return nTris;
			}

			boolean[] b = { true, true, true, true };

			if (m_pNeighbor[EDGE_UP] != null)
				b[EDGE_UP] = m_pNeighbor[EDGE_UP]._IsVisible(
						pFrustum.GetCamera(), fLODRatio);
			if (m_pNeighbor[EDGE_DN] != null)
				b[EDGE_DN] = m_pNeighbor[EDGE_DN]._IsVisible(
						pFrustum.GetCamera(), fLODRatio);
			if (m_pNeighbor[EDGE_LT] != null)
				b[EDGE_LT] = m_pNeighbor[EDGE_LT]._IsVisible(
						pFrustum.GetCamera(), fLODRatio);
			if (m_pNeighbor[EDGE_RT] != null)
				b[EDGE_RT] = m_pNeighbor[EDGE_RT]._IsVisible(
						pFrustum.GetCamera(), fLODRatio);

			if (b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT]) {
				// 좌측 상단 삼각형;
				nTris++;
				vArray.add(m_nCorner[CORNER_TL]);
				vArray.add(m_nCorner[CORNER_TR]);
				vArray.add(m_nCorner[CORNER_BL]);

				// 우측 하단 삼각형
				nTris++;
				vArray.add(m_nCorner[CORNER_BL]);
				vArray.add(m_nCorner[CORNER_TR]);
				vArray.add(m_nCorner[CORNER_BR]);

				return nTris;
			}

			Point p = new Point();
			if (!b[EDGE_UP]) {
				p.x = (m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x) / 2;
				p.y = m_nCorner[CORNER_TL].y;

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_TL]);
				vArray.add(p);

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(p);
				vArray.add(m_nCorner[CORNER_TR]);
			} else {
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_TL]);
				vArray.add(m_nCorner[CORNER_TR]);
			}

			if (!b[EDGE_DN]) {
				p.x = (m_nCorner[CORNER_BL].x + m_nCorner[CORNER_BR].x) / 2;
				p.y = m_nCorner[CORNER_BL].y;

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_BR]);
				vArray.add(p);

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(p);
				vArray.add(m_nCorner[CORNER_BL]);
			} else {
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_BR]);
				vArray.add(m_nCorner[CORNER_BL]);
			}

			if (!b[EDGE_LT]) {
				p.x = m_nCorner[CORNER_TL].x;
				p.y = (m_nCorner[CORNER_TL].y + m_nCorner[CORNER_BL].y) / 2;

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_BL]);
				vArray.add(p);

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(p);
				vArray.add(m_nCorner[CORNER_TL]);
			} else {
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_BL]);
				vArray.add(m_nCorner[CORNER_TL]);
			}

			if (!b[EDGE_RT]) {
				p.x = m_nCorner[CORNER_TR].x;
				p.y = (m_nCorner[CORNER_TR].y + m_nCorner[CORNER_BR].y) / 2;

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_TR]);
				vArray.add(p);

				nTris++;
				vArray.add(m_nCenter);
				vArray.add(p);
				vArray.add(m_nCorner[CORNER_BR]);
			} else {
				vArray.add(m_nCenter);
				vArray.add(m_nCorner[CORNER_TR]);
				vArray.add(m_nCorner[CORNER_BR]);
			}

			return nTris;
		}

		if (m_pChild[CORNER_TL] != null)
			nTris = m_pChild[CORNER_TL]._GenTriIndex(nTris, vArray, pFrustum,
					fLODRatio);
		if (m_pChild[CORNER_TR] != null)
			nTris = m_pChild[CORNER_TR]._GenTriIndex(nTris, vArray, pFrustum,
					fLODRatio);
		if (m_pChild[CORNER_BL] != null)
			nTris = m_pChild[CORNER_BL]._GenTriIndex(nTris, vArray, pFrustum,
					fLODRatio);
		if (m_pChild[CORNER_BR] != null)
			nTris = m_pChild[CORNER_BR]._GenTriIndex(nTris, vArray, pFrustum,
					fLODRatio);

		return nTris;
	}

	boolean _BuildQuadTree() {
		byte[] buffer = MainConst.imageBuf;
		// Vertex(m_nCorner[CORNER_TL].x, m_nCorner[CORNER_TL].y, buffer[nBands
		// * (nXSize * m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TL].y)])
		// Vertex(m_nCorner[CORNER_BR].x, m_nCorner[CORNER_BR].y, buffer[nBands
		// * (nXSize * m_nCorner[CORNER_BR].x + m_nCorner[CORNER_BR].y)])

		m_fRadius = _GetDistance(new Vertex(m_nCorner[CORNER_TL].x,
				m_nCorner[CORNER_TL].y, buffer[0]), new Vertex(
				m_nCorner[CORNER_BR].x, m_nCorner[CORNER_BR].y, buffer[0])) / 2.0f;

		if (_SubDivide()) {
			m_pChild[CORNER_TL]._BuildQuadTree();
			m_pChild[CORNER_TR]._BuildQuadTree();
			m_pChild[CORNER_BL]._BuildQuadTree();
			m_pChild[CORNER_BR]._BuildQuadTree();
		}

		return true;
	}

	boolean Build() {
		_BuildQuadTree();

		_BuildNeighborNode(this);
		return true;
	}

	int GenerateIndex(Vector<Point> vArray, Frustum pFrustum, float fLODRatio) {
		_FrustumCull(pFrustum);
		return _GenTriIndex(0, vArray, pFrustum, fLODRatio);
	}

	int _IsInFrustum(Frustum pFrustum) {
		byte[] buffer = MainConst.imageBuf;
		int[] b = new int[4];
		boolean bInSphere;

		bInSphere = pFrustum.SphereInFrustum(m_nCenter.x, m_nCenter.y,
				buffer[0], m_fRadius);
		if (!bInSphere)
			return FRUSTUM_OUT;

		b[0] = pFrustum.PointInFrustum(m_nCorner[0].x, m_nCorner[0].y,
				buffer[0]);
		b[1] = pFrustum.PointInFrustum(m_nCorner[1].x, m_nCorner[1].y,
				buffer[0]);
		b[2] = pFrustum.PointInFrustum(m_nCorner[2].x, m_nCorner[2].y,
				buffer[0]);
		b[3] = pFrustum.PointInFrustum(m_nCorner[3].x, m_nCorner[3].y,
				buffer[0]);

		if ((b[0] + b[1] + b[2] + b[3]) == 4)
			return FRUSTUM_COMPLETELY_IN;

		return FRUSTUM_PRTIALLY_IN;
	}

	void _FrustumCull(Frustum pFrustum) {
		int ret;

		ret = _IsInFrustum(pFrustum);

		switch (ret) {
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

		if (m_pChild[0] != null)
			m_pChild[0]._FrustumCull(pFrustum);
		if (m_pChild[1] != null)
			m_pChild[1]._FrustumCull(pFrustum);
		if (m_pChild[2] != null)
			m_pChild[2]._FrustumCull(pFrustum);
		if (m_pChild[3] != null)
			m_pChild[3]._FrustumCull(pFrustum);
	}

	float _GetDistance(Vertex v1, Vertex v2) {
		float x = (v1.x - v2.x) * (v1.x - v2.x);
		float y = (v1.y - v2.y) * (v1.y - v2.y);
		float z = (v1.z - v2.z) * (v1.z - v2.z);

		return (float) Math.sqrt((double) x + y + z);
	}

	boolean _GetNodeIndex(int i, Point _0, Point _1, Point _2, Point _3) {

		Point _a, _b, _c, _d;
		int gapX, gapY;

		_a = _0;
		_b = _1;
		_c = _2;
		_d = _3;
		gapX = _b.x - _a.x;
		gapY = _c.y - _a.y;

		switch (i) {
		case EDGE_UP:
			_0 = new Point(_a.x, (_a.y - gapY));
			_1 = new Point(_b.x, (_b.y - gapY));
			_2 = _a;
			_3 = _b;
			break;
		case EDGE_DN:
			_0 = _c;
			_1 = _d;
			_2 = new Point(_c.x, (_c.y + gapY));
			_3 = new Point(_d.x, (_d.y + gapY));
			break;
		case EDGE_LT:
			_0 = new Point((_a.x - gapX), _a.y);
			_1 = _a;
			_2 = new Point((_c.x - gapX), _c.y);
			_3 = _c;
			break;
		case EDGE_RT:
			_0 = _b;
			_1 = new Point((_b.x + gapX), _b.y);
			_2 = _d;
			_3 = new Point((_d.x + gapX), _d.y);
			break;
		}
		if (!_IsInRect(new Point(0, 0), new Point(MainConst.nXSize,
				MainConst.nYSize), new Point((_0.x + _1.x) / 2,
				(_0.y + _2.y) / 2)))
			return false;
		return true;
	}

	void _BuildNeighborNode(ZQuadTree pRoot) {
		boolean n;
		Point _0, _1, _2, _3;

		for (int i = 0; i < 4; i++) {
			_0 = m_nCorner[CORNER_TL];
			_1 = m_nCorner[CORNER_TR];
			_2 = m_nCorner[CORNER_BL];
			_3 = m_nCorner[CORNER_BR];

			n = _GetNodeIndex(i, _0, _1, _2, _3);
			if (n)
				m_pNeighbor[i] = pRoot._FindNode(_0, _1, _2, _3);
		}

		if (m_pChild[0] != null) {
			m_pChild[0]._BuildNeighborNode(pRoot);
			m_pChild[1]._BuildNeighborNode(pRoot);
			m_pChild[2]._BuildNeighborNode(pRoot);
			m_pChild[3]._BuildNeighborNode(pRoot);
		}
	}

	ZQuadTree _FindNode(Point _0, Point _1, Point _2, Point _3) {
		if (m_nCorner[CORNER_TL] == _0 && m_nCorner[CORNER_TR] == _1
				&& m_nCorner[CORNER_BL] == _2 && m_nCorner[CORNER_BR] == _3)
			return this;

		if (m_pChild[0] != null) {
			Point pt = new Point((_0.x + _1.x) / 2, (_0.y + _2.y) / 2);
			// Point pt((m_nCorner[CORNER_TL].x + m_nCorner[CORNER_TR].x)/2,
			// (m_nCorner[CORNER_TL].y + m_nCorner[CORNER_BL].y)/2);

			if (_IsInRect(m_pChild[CORNER_TL].m_nCorner[CORNER_TL],
					m_pChild[CORNER_TL].m_nCorner[CORNER_BR], pt))
				return m_pChild[CORNER_TL]._FindNode(_0, _1, _2, _3);

			if (_IsInRect(m_pChild[CORNER_TR].m_nCorner[CORNER_TL],
					m_pChild[CORNER_TR].m_nCorner[CORNER_BR], pt))
				return m_pChild[CORNER_TR]._FindNode(_0, _1, _2, _3);

			if (_IsInRect(m_pChild[CORNER_BL].m_nCorner[CORNER_TL],
					m_pChild[CORNER_BL].m_nCorner[CORNER_BR], pt))
				return m_pChild[CORNER_BL]._FindNode(_0, _1, _2, _3);

			if (_IsInRect(m_pChild[CORNER_BR].m_nCorner[CORNER_TL],
					m_pChild[CORNER_BR].m_nCorner[CORNER_BR], pt))
				return m_pChild[CORNER_BR]._FindNode(_0, _1, _2, _3);
		}
		return null;
	}

	boolean _IsInRect(Point TL, Point BR, Point target) {
		if (TL.x - 1 < target.x && BR.x + 1 > target.x && TL.y - 1 < target.y
				&& BR.y + 1 > target.y)
			return true;

		return false;
	}

	int _GetLODLevel(Vertex pCamera, float fLODRatio) {
		float d = _GetDistance(new Vertex((float) m_nCenter.x,
				(float) m_nCenter.y, (float) 0/* 수정 */), pCamera);
		return Math.max((int) (d * fLODRatio), MIN_SIZE);
	}

}
