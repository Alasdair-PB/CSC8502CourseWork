#pragma once
#include "../nclgl/SceneNode.h"
#include <vector>

struct BezierCurve
{
	Vector3 p0, p1, p2, p3;

	Vector3 GetPoint(float t) const {
		float u = 1 - t;
		float tt = t * t;
		float uu = u * u;
		float uuu = uu * u;
		float ttt = tt * t;

		Vector3 point = p0 * uuu;
		point += p1 * 3 * uu * t;
		point += p2 * 3 * u * tt;
		point += p3 * ttt;
		return point;
	}
};

class Pathing 
{
	public:
		Pathing() 
		{
			followingPath = false;
			bezierT = 0.0f;
			currentCurveIndex = 0;
			bezierSpeed = 0.1f;
			bezierPath = std::vector<BezierCurve>();
		}

		~Pathing(void) {
			bezierPath.clear();
		}

		void SetPathPattern(Vector3 startPoint, Vector3 windingOffsetA, 
			Vector3 windingOffsetB, Vector3 endPointOffset, int pathCount)
		{
			for (int i = 0; i < pathCount; i++)
			{

				Vector3 controlPoint1 = startPoint + windingOffsetA;
				Vector3 controlPoint2 = startPoint + windingOffsetB;
				Vector3 endPoint = startPoint + endPointOffset;

				bezierPath.push_back(BezierCurve{
					startPoint,
					controlPoint1,
					controlPoint2,
					endPoint
				});

				startPoint = endPoint;
			}
		}

		bool ContinePath() { return followingPath && !bezierPath.empty();}

		void AddPath(Vector3 startPoint, Vector3 windingPointA, 
			Vector3 windingPointB, Vector3 endPoint)
		{
			bezierPath.push_back(BezierCurve{
				startPoint,
				windingPointA,
				windingPointB,
				endPoint
			});
		}

		Vector3 GetNextPosition(float dt) {
			bezierT += dt * bezierSpeed;

			while (bezierT > 1.0f) {
				bezierT -= 1.0f;
				currentCurveIndex++;

				if (currentCurveIndex >= bezierPath.size()) {
					followingPath = false;
					return bezierPath.back().GetPoint(1.0f);
				}
			}
			return bezierPath[currentCurveIndex].GetPoint(bezierT);
		}


	protected:
	

		std::vector <BezierCurve> bezierPath;
		bool followingPath = true;
		float bezierT = 0.0f;
		int currentCurveIndex = 0;
		float bezierSpeed = 0.1f;


};