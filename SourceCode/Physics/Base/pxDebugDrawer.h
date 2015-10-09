/*
=============================================================================
	File:	pxDebugDrawer.h
	Desc:
=============================================================================
*/

#ifndef __PX_DEBUG_DRAWER_H__
#define __PX_DEBUG_DRAWER_H__

MX_SWIPED("bullet")
struct pxDebugDrawer
{
	enum	DebugDrawModes
	{
		DBG_NoDebug=0,
		DBG_DrawWireframe = 1,
		DBG_DrawAabb=2,
		DBG_DrawFeaturesText=4,
		DBG_DrawContactPoints=8,
		DBG_NoDeactivation=16,
		DBG_NoHelpText = 32,
		DBG_DrawText=64,
		DBG_ProfileTimings = 128,
		DBG_EnableSatComparison = 256,
		DBG_DisableBulletLCP = 512,
		DBG_EnableCCD = 1024,
		DBG_DrawConstraints = (1 << 11),
		DBG_DrawConstraintLimits = (1 << 12),
		DBG_FastWireframe = (1<<13),
		DBG_MAX_DEBUG_DRAW_MODE
	};

public:
	virtual void drawLine(const pxVec3& from,const pxVec3& to,const pxVec3& color)=0;

	virtual void	drawSphere(pxReal radius, const pxTransform& transform, const pxVec3& color)
	{
		pxVec3 start = transform.GetOrigin();

		const pxVec3 xoffs = transform.GetBasis() * pxVec3(radius,0,0);
		const pxVec3 yoffs = transform.GetBasis() * pxVec3(0,radius,0);
		const pxVec3 zoffs = transform.GetBasis() * pxVec3(0,0,radius);

		// XY 
		drawLine(start-xoffs, start+yoffs, color);
		drawLine(start+yoffs, start+xoffs, color);
		drawLine(start+xoffs, start-yoffs, color);
		drawLine(start-yoffs, start-xoffs, color);

		// XZ
		drawLine(start-xoffs, start+zoffs, color);
		drawLine(start+zoffs, start+xoffs, color);
		drawLine(start+xoffs, start-zoffs, color);
		drawLine(start-zoffs, start-xoffs, color);

		// YZ
		drawLine(start-yoffs, start+zoffs, color);
		drawLine(start+zoffs, start+yoffs, color);
		drawLine(start+yoffs, start-zoffs, color);
		drawLine(start-zoffs, start-yoffs, color);
	}
	
	virtual void	drawSphere (const pxVec3& p, pxReal radius, const pxVec3& color)
	{
		pxTransform tr;
		tr.SetIdentity();
		tr.SetOrigin(p);
		drawSphere(radius,tr,color);
	}
	
	virtual	void	drawTriangle(const pxVec3& v0,const pxVec3& v1,const pxVec3& v2,const pxVec3& /*n0*/,const pxVec3& /*n1*/,const pxVec3& /*n2*/,const pxVec3& color, pxReal alpha)
	{
		drawTriangle(v0,v1,v2,color,alpha);
	}
	virtual	void	drawTriangle(const pxVec3& v0,const pxVec3& v1,const pxVec3& v2,const pxVec3& color, pxReal /*alpha*/)
	{
		drawLine(v0,v1,color);
		drawLine(v1,v2,color);
		drawLine(v2,v0,color);
	}

	//virtual void	drawContactPoint(const pxVec3& PointOnB,const pxVec3& normalOnB,pxReal distance,int lifeTime,const pxVec3& color)=0;

	//virtual void	reportErrorWarning(const char* warningString) = 0;

	//virtual void	draw3dText(const pxVec3& location,const char* textString) = 0;
	//
	//virtual void	setDebugMode(int debugMode) =0;
	//
	//virtual int		getDebugMode() const = 0;

	virtual void drawAabb(const pxVec3& from,const pxVec3& to,const pxVec3& color)
	{

		pxVec3 halfExtents = (to-from)* 0.5f;
		pxVec3 center = (to+from) *0.5f;
		int i,j;

		pxVec3 edgecoord(1.f,1.f,1.f),pa,pb;
		for (i=0;i<4;i++)
		{
			for (j=0;j<3;j++)
			{
				pa = pxVec3(edgecoord[0]*halfExtents[0], edgecoord[1]*halfExtents[1],		
					edgecoord[2]*halfExtents[2]);
				pa+=center;

				int othercoord = j%3;
				edgecoord[othercoord]*=-1.f;
				pb = pxVec3(edgecoord[0]*halfExtents[0], edgecoord[1]*halfExtents[1],	
					edgecoord[2]*halfExtents[2]);
				pb+=center;

				drawLine(pa,pb,color);
			}
			edgecoord = pxVec3(-1.f,-1.f,-1.f);
			if (i<3)
				edgecoord[i]*=-1.f;
		}
	}
	virtual void drawTransform(const pxTransform& transform, pxReal orthoLen)
	{
		pxVec3 start = transform.GetOrigin();
		drawLine(start, start+transform.GetBasis() * pxVec3(orthoLen, 0, 0), pxVec3(0.7f,0,0));
		drawLine(start, start+transform.GetBasis() * pxVec3(0, orthoLen, 0), pxVec3(0,0.7f,0));
		drawLine(start, start+transform.GetBasis() * pxVec3(0, 0, orthoLen), pxVec3(0,0,0.7f));
	}

	virtual void drawArc(const pxVec3& center, const pxVec3& normal, const pxVec3& axis, pxReal radiusA, pxReal radiusB, pxReal minAngle, pxReal maxAngle, 
				const pxVec3& color, bool drawSect, pxReal stepDegrees = pxReal(10.f))
	{
		const pxVec3& vx = axis;
		pxVec3 vy = normal.Cross(axis);
		pxReal step = RAD2DEG(stepDegrees);
		int nSteps = (int)((maxAngle - minAngle) / step);
		if(!nSteps) nSteps = 1;
		pxVec3 prev = center + radiusA * vx * mxCos(minAngle) + radiusB * vy * mxSin(minAngle);
		if(drawSect)
		{
			drawLine(center, prev, color);
		}
		for(int i = 1; i <= nSteps; i++)
		{
			pxReal angle = minAngle + (maxAngle - minAngle) * pxReal(i) / pxReal(nSteps);
			pxVec3 next = center + radiusA * vx * mxCos(angle) + radiusB * vy * mxSin(angle);
			drawLine(prev, next, color);
			prev = next;
		}
		if(drawSect)
		{
			drawLine(center, prev, color);
		}
	}
	virtual void drawSpherePatch(const pxVec3& center, const pxVec3& up, const pxVec3& axis, pxReal radius, 
		pxReal minTh, pxReal maxTh, pxReal minPs, pxReal maxPs, const pxVec3& color, pxReal stepDegrees = pxReal(10.f))
	{
		pxVec3 vA[74];
		pxVec3 vB[74];
		pxVec3 *pvA = vA, *pvB = vB, *pT;
		pxVec3 npole = center + up * radius;
		pxVec3 spole = center - up * radius;
		pxVec3 arcStart;
		pxReal step = RAD2DEG(stepDegrees);
		const pxVec3& kv = up;
		const pxVec3& iv = axis;
		pxVec3 jv = kv.Cross(iv);
		bool drawN = false;
		bool drawS = false;
		if(minTh <= -MX_HALF_PI)
		{
			minTh = -MX_HALF_PI + step;
			drawN = true;
		}
		if(maxTh >= MX_HALF_PI)
		{
			maxTh = MX_HALF_PI - step;
			drawS = true;
		}
		if(minTh > maxTh)
		{
			minTh = -MX_HALF_PI + step;
			maxTh =  MX_HALF_PI - step;
			drawN = drawS = true;
		}
		int n_hor = (int)((maxTh - minTh) / step) + 1;
		if(n_hor < 2) n_hor = 2;
		pxReal step_h = (maxTh - minTh) / pxReal(n_hor - 1);
		bool isClosed = false;
		if(minPs > maxPs)
		{
			minPs = -MX_PI + step;
			maxPs =  MX_PI;
			isClosed = true;
		}
		else if((maxPs - minPs) >= MX_PI * pxReal(2.f))
		{
			isClosed = true;
		}
		else
		{
			isClosed = false;
		}
		int n_vert = (int)((maxPs - minPs) / step) + 1;
		if(n_vert < 2) n_vert = 2;
		pxReal step_v = (maxPs - minPs) / pxReal(n_vert - 1);
		for(int i = 0; i < n_hor; i++)
		{
			pxReal th = minTh + pxReal(i) * step_h;
			pxReal sth = radius * mxSin(th);
			pxReal cth = radius * mxCos(th);
			for(int j = 0; j < n_vert; j++)
			{
				pxReal psi = minPs + pxReal(j) * step_v;
				pxReal sps = mxSin(psi);
				pxReal cps = mxCos(psi);
				pvB[j] = center + cth * cps * iv + cth * sps * jv + sth * kv;
				if(i)
				{
					drawLine(pvA[j], pvB[j], color);
				}
				else if(drawS)
				{
					drawLine(spole, pvB[j], color);
				}
				if(j)
				{
					drawLine(pvB[j-1], pvB[j], color);
				}
				else
				{
					arcStart = pvB[j];
				}
				if((i == (n_hor - 1)) && drawN)
				{
					drawLine(npole, pvB[j], color);
				}
				if(isClosed)
				{
					if(j == (n_vert-1))
					{
						drawLine(arcStart, pvB[j], color);
					}
				}
				else
				{
					if(((!i) || (i == (n_hor-1))) && ((!j) || (j == (n_vert-1))))
					{
						drawLine(center, pvB[j], color);
					}
				}
			}
			pT = pvA; pvA = pvB; pvB = pT;
		}
	}
	
	virtual void drawBox(const pxVec3& bbMin, const pxVec3& bbMax, const pxVec3& color)
	{
		drawLine(pxVec3(bbMin[0], bbMin[1], bbMin[2]), pxVec3(bbMax[0], bbMin[1], bbMin[2]), color);
		drawLine(pxVec3(bbMax[0], bbMin[1], bbMin[2]), pxVec3(bbMax[0], bbMax[1], bbMin[2]), color);
		drawLine(pxVec3(bbMax[0], bbMax[1], bbMin[2]), pxVec3(bbMin[0], bbMax[1], bbMin[2]), color);
		drawLine(pxVec3(bbMin[0], bbMax[1], bbMin[2]), pxVec3(bbMin[0], bbMin[1], bbMin[2]), color);
		drawLine(pxVec3(bbMin[0], bbMin[1], bbMin[2]), pxVec3(bbMin[0], bbMin[1], bbMax[2]), color);
		drawLine(pxVec3(bbMax[0], bbMin[1], bbMin[2]), pxVec3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(pxVec3(bbMax[0], bbMax[1], bbMin[2]), pxVec3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(pxVec3(bbMin[0], bbMax[1], bbMin[2]), pxVec3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(pxVec3(bbMin[0], bbMin[1], bbMax[2]), pxVec3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(pxVec3(bbMax[0], bbMin[1], bbMax[2]), pxVec3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(pxVec3(bbMax[0], bbMax[1], bbMax[2]), pxVec3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(pxVec3(bbMin[0], bbMax[1], bbMax[2]), pxVec3(bbMin[0], bbMin[1], bbMax[2]), color);
	}
	virtual void drawBox(const pxVec3& bbMin, const pxVec3& bbMax, const pxTransform& trans, const pxVec3& color)
	{
		drawLine(trans * pxVec3(bbMin[0], bbMin[1], bbMin[2]), trans * pxVec3(bbMax[0], bbMin[1], bbMin[2]), color);
		drawLine(trans * pxVec3(bbMax[0], bbMin[1], bbMin[2]), trans * pxVec3(bbMax[0], bbMax[1], bbMin[2]), color);
		drawLine(trans * pxVec3(bbMax[0], bbMax[1], bbMin[2]), trans * pxVec3(bbMin[0], bbMax[1], bbMin[2]), color);
		drawLine(trans * pxVec3(bbMin[0], bbMax[1], bbMin[2]), trans * pxVec3(bbMin[0], bbMin[1], bbMin[2]), color);
		drawLine(trans * pxVec3(bbMin[0], bbMin[1], bbMin[2]), trans * pxVec3(bbMin[0], bbMin[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMax[0], bbMin[1], bbMin[2]), trans * pxVec3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMax[0], bbMax[1], bbMin[2]), trans * pxVec3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMin[0], bbMax[1], bbMin[2]), trans * pxVec3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMin[0], bbMin[1], bbMax[2]), trans * pxVec3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMax[0], bbMin[1], bbMax[2]), trans * pxVec3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMax[0], bbMax[1], bbMax[2]), trans * pxVec3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * pxVec3(bbMin[0], bbMax[1], bbMax[2]), trans * pxVec3(bbMin[0], bbMin[1], bbMax[2]), color);
	}

protected:
	virtual ~pxDebugDrawer() {}
};

#endif // !__PX_DEBUG_DRAWER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
