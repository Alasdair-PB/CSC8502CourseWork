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
			followingPath = true;
			bezierT = 0.1f;
			currentCurveIndex = 1;
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

		void AddPath(Vector3 windingPointA, 
			Vector3 windingPointB, Vector3 endPoint)
		{
			Vector3 startPoint = bezierPath.back().p3;
			bezierPath.push_back(BezierCurve{
				startPoint,
				windingPointA,
				windingPointB,
				endPoint
			});
		}


		void AddCircuit(float radius, Vector3 offset, int segments = 8) {
			bezierPath.clear();
			float angleStep = 2.0f * PI / segments;

			for (int i = 0; i < segments; ++i)
			{
				float angle1 = i * angleStep;
				float angle2 = (i + 1) * angleStep;

				Vector3 startPoint = Vector3(radius * cos(angle1), 0, radius * sin(angle1)) + offset;
				Vector3 endPoint = Vector3(radius * cos(angle2), 0, radius * sin(angle2)) + offset;

				Vector3 tangent1 = Vector3(-radius * sin(angle1), 0, radius * cos(angle1));  
				Vector3 tangent2 = Vector3(-radius * sin(angle2), 0, radius * cos(angle2)); 

				Vector3 windingPointA = startPoint + tangent1 * 0.5f;
				Vector3 windingPointB = endPoint + tangent2 * 0.5f;

				AddPath(startPoint, windingPointA, windingPointB, endPoint);
			}
		}



		Vector3 GetNextPosition(float dt, bool hidden = false) 
		{
			bezierT += (dt * bezierSpeed);

			while (bezierT > 1.0f) {
				bezierT -= 1.0f;
				currentCurveIndex += 1;

				if (currentCurveIndex >= bezierPath.size()) 
					currentCurveIndex = 0;  
			}
			return bezierPath[currentCurveIndex].GetPoint(bezierT);
		}



		void SetPathing(bool state) {
			 followingPath = state;
		}


		void FlipPathing() {
			followingPath = !followingPath;
		}



	protected:
	
		bool looped = true;
		bool reverse = false;
		std::vector <BezierCurve> bezierPath;
		bool followingPath = true;
		float bezierT = 0.0f;
		int currentCurveIndex = 1;
		float bezierSpeed = 0.1f;


};