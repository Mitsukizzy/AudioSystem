#include "ITPEnginePCH.h"

namespace Collision
{

	bool Intersects(const Sphere& a, const Sphere& b)
	{
		Vector3 diff = a.mCenter - b.mCenter;
		float DistSq = diff.LengthSq();
		float sumRadiiSq = (a.mRadius + b.mRadius) * (a.mRadius + b.mRadius);
		if (DistSq <= sumRadiiSq)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Intersects(const AxisAlignedBox & a, const AxisAlignedBox & b)
	{
		// Tests the 6 cases where AABBs definitely do not intersect
		return ( a.mMax.x >= b.mMin.x && a.mMin.x <= b.mMax.x )
			&& ( a.mMax.y >= b.mMin.y && a.mMin.y <= b.mMax.y )
			&& ( a.mMax.z >= b.mMin.z && a.mMin.z <= b.mMax.z );
	}

	bool SegmentCast(const LineSegment& segment, const AxisAlignedBox& box, Vector3& outPoint)
	{
		Vector3 ray = segment.mEnd - segment.mStart;

		float tmin = 0.0f;
		float tmax = ray.Length();
		float ood;
		float t1;
		float t2;

		ray.Normalize();

		// X
		if ( Math::IsZero( abs( ray.x ) ) )
		{
			// Parallel, check if collision
			if ( segment.mStart.x < box.mMin.x || segment.mStart.x > box.mMax.x )
				return false;
		}
		else
		{
			ood = 1.0f / ray.x;
			t1 = ( box.mMin.x - segment.mStart.x ) * ood;
			t2 = ( box.mMax.x - segment.mStart.x ) * ood;

			if ( t2 < t1 )
				std::swap( t1, t2 );

			tmin = max( tmin, t1 );
			tmax = min( tmax, t2 );

			if ( tmin > tmax )
				return false;
		}

		// Y
		if ( Math::IsZero( abs( ray.y ) ) )
		{
			// Parallel, check if collision
			if ( segment.mStart.y < box.mMin.y || segment.mStart.y > box.mMax.y )
				return false;
		}
		else
		{
			ood = 1.0f / ray.y;
			t1 = ( box.mMin.y - segment.mStart.y ) * ood;
			t2 = ( box.mMax.y - segment.mStart.y ) * ood;

			if ( t2 < t1 )
				std::swap( t1, t2 );

			tmin = max( tmin, t1 );
			tmax = min( tmax, t2 );

			if ( tmin > tmax )
				return false;
		}

		// Z
		if ( Math::IsZero( abs( ray.z ) ) )
		{
			// Parallel, check if collision
			if ( segment.mStart.z < box.mMin.z || segment.mStart.z > box.mMax.z )
				return false;
		}
		else
		{
			ood = 1.0f / ray.z;
			t1 = ( box.mMin.z - segment.mStart.z ) * ood;
			t2 = ( box.mMax.z - segment.mStart.z ) * ood;

			if ( t2 < t1 )
				std::swap( t1, t2 );

			tmin = max( tmin, t1 );
			tmax = min( tmax, t2 );

			if ( tmin > tmax )
				return false;
		}

		outPoint = segment.mStart + ray * tmin;
		return true;
	}

} // namespace
