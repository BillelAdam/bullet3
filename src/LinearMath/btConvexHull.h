
/*
Stan Melax Convex Hull Computation
Copyright (c) 2008 Stan Melax http://www.melax.com/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

///includes modifications/improvements by John Ratcliff, see BringOutYourDead below.

#ifndef CD_HULL_H
#define CD_HULL_H

class HullResult
{
public:
	HullResult(void)
	{
		mPolygons = true;
		mNumOutputVertices = 0;
		mOutputVertices = 0;
		mNumFaces = 0;
		mNumIndices = 0;
		mIndices = 0;
	}
	bool                    mPolygons;                  // true if indices represents polygons, false indices are triangles
	unsigned int            mNumOutputVertices;         // number of vertices in the output hull
	float                  *mOutputVertices;            // array of vertices, 3 floats each x,y,z
	unsigned int            mNumFaces;                  // the number of faces produced
	unsigned int            mNumIndices;                // the total number of indices
	unsigned int           *mIndices;                   // pointer to indices.

// If triangles, then indices are array indexes into the vertex list.
// If polygons, indices are in the form (number of points in face) (p1, p2, p3, ..) etc..
};

enum HullFlag
{
	QF_TRIANGLES         = (1<<0),             // report results as triangles, not polygons.
	QF_REVERSE_ORDER     = (1<<1),             // reverse order of the triangle indices.
	QF_SKIN_WIDTH        = (1<<2),             // extrude hull based on this skin width
	QF_DEFAULT           = 0
};


class HullDesc
{
public:
	HullDesc(void)
	{
		mFlags          = QF_DEFAULT;
		mVcount         = 0;
		mVertices       = 0;
		mVertexStride   = sizeof(float)*3;
		mNormalEpsilon  = 0.001f;
		mMaxVertices		= 4096; // maximum number of points to be considered for a convex hull.
		mMaxFaces				= 4096;
		mSkinWidth			= 0.01f; // default is one centimeter
	};

	HullDesc(HullFlag flag,
					 unsigned int vcount,
					 const float *vertices,
					 unsigned int stride)
	{
		mFlags          = flag;
		mVcount         = vcount;
		mVertices       = vertices;
		mVertexStride   = stride;
		mNormalEpsilon  = 0.001f;
		mMaxVertices    = 4096;
		mSkinWidth = 0.01f; // default is one centimeter
	}

	bool HasHullFlag(HullFlag flag) const
	{
		if ( mFlags & flag ) return true;
		return false;
	}

	void SetHullFlag(HullFlag flag)
	{
		mFlags|=flag;
	}

	void ClearHullFlag(HullFlag flag)
	{
		mFlags&=~flag;
	}

	unsigned int      mFlags;           // flags to use when generating the convex hull.
	unsigned int      mVcount;          // number of vertices in the input point cloud
	const float      *mVertices;        // the array of vertices.
	unsigned int      mVertexStride;    // the stride of each vertex, in bytes.
	float             mNormalEpsilon;   // the epsilon for removing duplicates.  This is a normalized value, if normalized bit is on.
	float             mSkinWidth;
	unsigned int      mMaxVertices;               // maximum number of vertices to be considered for the hull!
	unsigned int      mMaxFaces;
};

enum HullError
{
	QE_OK,            // success!
	QE_FAIL           // failed.
};

class HullLibrary
{
public:

	HullError CreateConvexHull(const HullDesc       &desc,           // describes the input request
															HullResult           &result);        // contains the resulst

	HullError ReleaseResult(HullResult &result); // release memory allocated for this result, we are done with it.

private:

	//BringOutYourDead (John Ratcliff): When you create a convex hull you hand it a large input set of vertices forming a 'point cloud'. 
	//After the hull is generated it give you back a set of polygon faces which index the *original* point cloud.
	//The thing is, often times, there are many 'dead vertices' in the point cloud that are on longer referenced by the hull.
	//The routine 'BringOutYourDead' find only the referenced vertices, copies them to an new buffer, and re-indexes the hull so that it is a minimal representation.
	void BringOutYourDead(const float *verts,unsigned int vcount, float *overts,unsigned int &ocount,unsigned int *indices,unsigned indexcount);

	bool    CleanupVertices(unsigned int svcount,
													const float *svertices,
													unsigned int stride,
													unsigned int &vcount,       // output number of vertices
													float *vertices,                 // location to store the results.
													float  normalepsilon,
													float *scale);
};


#endif
