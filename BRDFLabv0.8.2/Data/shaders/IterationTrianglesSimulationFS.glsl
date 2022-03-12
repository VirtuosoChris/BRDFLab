/*
	Copyright (c) 2009.
		Adria Fores
		Sumanta N. Pattanaik
		Carles Bosch
		Xavier Pueyo

	Contact: adriafores@gmail.com	

	This file is part of BRDFLab.

    BRDFLab is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BRDFLab is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BRDFLab.  If not, see <http://www.gnu.org/licenses/>.
*/


#define MAX_DISTANCE 1000000000.


// general simulations
uniform vec3 eyeDirection;

// simulation specific
uniform float vertexWidth;
uniform float indexWidth;
uniform float nTriangles;
uniform float maxHeight;

// grid 
uniform int width;
uniform int height;


// data structure
uniform sampler2D vertexTexture;
uniform sampler2D indexTexture;

// iteration data
uniform sampler2D directions;
uniform sampler2D hits;



vec2 get2DCoord( float pos, float texwidth )
{
	vec2 uv;

	// Rectangular coordinate => ( 0..texwidth, 0..texwidth )
	uv.y = floor( pos / texwidth );
	uv.x = pos - texwidth * uv.y;		// pos % texwidth


	// Convert to uniform space / normalize => ( 0..1, 0..1 )
	uv = ( uv + 0.5 ) / texwidth;

	return uv;
}

/**
///Ray – triangle intersection routine [Möller]
///@param vec3 orig Ray origin
///@param vec3 dir Ray direction
///@param vec3 v0 Triangle 1st vertex
///@param vec3 v1 Triangle 2nd vertex
///@param vec3 v2 Triangle 3rd vertex
///@param float last_hit_distance Record of the previous hit (u,v,t)
///@return float computed distance t’ if it finds a better hit than
///             last_hit_distance, last_hit_distance otherwise.
**/
float IntersectTriangle(vec3 orig, vec3 dir,
						vec3 v0, vec3 v1, vec3 v2,
						float last_hit_distance)
{
	vec3 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det;
	float u,v,t;
	
	//find vectors for two edges sharing v0
	edge1 = v1 - v0;// swap to support anti clockwise triangle order 
	edge2 = v2 - v0;//
	
	//begin calculating determinant - also used to calculate U param
	pvec = cross(dir,edge2);
	
	//if determinant is near zero, ray lies in plane of triangle
	det = dot(pvec, edge1);
	
	if(det<0.000001)
		return last_hit_distance;
	
	//calculate distance from v0 to ray origin
	tvec = orig - v0;
	
	//calculate U param and test bounds
	u = dot(tvec, pvec);
	if(u < 0.0 || u > det)
		return last_hit_distance;
		
	//prepare to test V param
	qvec = cross(tvec, edge1);
	
	// calculate V param and test bounds
	v = dot(dir, qvec);
	if(v < 0.0 || u + v > det)
		return last_hit_distance;
	
	//calculate t, scale params, ray intersect triangle 
	t = dot(edge2, qvec);
	inv_det = 1.0 / det;
	t *= inv_det;
	
	if(t < last_hit_distance)
		return t;
	else
		return last_hit_distance;
	
}	


int getTriangleIndex(int X, int Y, int width, int height){

	if((X >= 0 ) && (Y >= 0) && (X < width) && (Y < height))
		return (Y*width+X)*2;
	else
		return width*height*2;
}

float testTriangle(vec3 orig, vec3 dir, int tri, inout vec3 v0, inout vec3 v1, inout vec3 v2){

	
	// get the vertex position of the index
	vec3 triangleIndex = texture2D(indexTexture,get2DCoord(float(tri), indexWidth)).xyz;
	
	v0 = texture2D(vertexTexture,get2DCoord(triangleIndex.x,vertexWidth)).xyz;
	
	v1 = texture2D(vertexTexture,get2DCoord(triangleIndex.y,vertexWidth)).xyz;
		
	v2 = texture2D(vertexTexture,get2DCoord(triangleIndex.z,vertexWidth)).xyz;
	
	// test if the ray intersect the triangle
	return IntersectTriangle(orig, dir,
                      v0, v1, v2,
                      MAX_DISTANCE);	
		

}

// parameters: origin, dir, distance , normal
vec3 rayPlaneIntersection(vec3 p0, vec3 V, float d, vec3 N){

	// plane normal
//	vec3 N = vec3(0.,0.,1.);
	
	float j = dot(V,N);
	
	if(j == 0.){
		return vec3(1000.,1000.,1000.);
	}else{			
		float t = -(dot(p0,N)+d)/j;

		// final pointX
		return p0+t*V;
	}
}

void main()																					
{

	//compute the uv coordinates for this point
	// to find out the initial intersection uv coordinates.
	vec2 uv = gl_TexCoord[0].xy*0.5+0.5; 
	
	// recover direction
	vec4 iDir = texture2D(directions,vec2(uv.x,1.-uv.y));
	
	// compute intersection (status=0) or passthrough (status=1)
	if(iDir.a != 1.){
		vec3 dir = iDir.xyz;
		vec3 orig = texture2D(hits,vec2(uv.x,1.-uv.y)).xyz;
	
	
		//vars to store the closest triangle vertices
		vec3 v0, v1, v2;
			
		// last intersection point
		float last_hit_distance = MAX_DISTANCE;
		
		// grid traversal
		
		// needed vars for grid traversal.
		int stepX;
		int stepY;
		
		float tMaxX;
		float tMaxY;
		
		float tDeltaX;
		float tDeltaY;
		
		// find out the voxel position
		
		vec3 origGridSpace = vec3 (orig.x * float(width),orig.y * float(height), orig.z );
		
		int X = int(floor(origGridSpace.x));
		int Y = int(floor(origGridSpace.y));	 
		 
		// set the tMax and tDelta.
		
		if(dir.x > 0.){
			tMaxX = length(rayPlaneIntersection(origGridSpace, dir, float(X)+1., vec3(-1.,0.,0.))-origGridSpace);
			tDeltaX = length(rayPlaneIntersection(vec3(0.,0.,0.), dir, 1., vec3(-1.,0.,0.)));
			stepX = 1;
		}else{
			tMaxX = length(rayPlaneIntersection(origGridSpace, dir, -float(X), vec3(1.,0.,0.))-origGridSpace);
			tDeltaX = length(rayPlaneIntersection(vec3(0.,0.,0.), dir, -1., vec3(1.,0.,0.)));
			stepX = -1;
		}
		
		if(dir.y > 0.){
			tMaxY = length(rayPlaneIntersection(origGridSpace, dir, float(Y)+1., vec3(0.,-1.,0.))-origGridSpace);
			tDeltaY = length(rayPlaneIntersection(vec3(0.,0.,0.), dir, 1., vec3(0.,-1.,0.)));
			stepY = 1;
		}else{
			tMaxY = length(rayPlaneIntersection(origGridSpace, dir, -float(Y), vec3(0.,1.,0.))-origGridSpace);
			tDeltaY = length(rayPlaneIntersection(vec3(0.,0.,0.), dir, -1., vec3(0.,1.,0.)));
			stepY = -1;
		}
		
	//	gl_FragData[1] = vec4(tMaxX,tMaxY,tDeltaX,tDeltaY);
	//	gl_FragData[0] = vec4(width,height, 0.,1.);
	//	gl_FragData[1] = vec4(X,Y,0.,1.);
	//	return;

		int gridPositions = width*height*2;
		
		while(last_hit_distance == MAX_DISTANCE)
		{
		
			int index = getTriangleIndex(X,Y,width,height);

			// if we are inside the grid.
			if(index < gridPositions){
				
				// test triangle 1
				last_hit_distance = testTriangle(orig, dir, index, v0, v1, v2);

				// test triangle 2
				if(last_hit_distance == MAX_DISTANCE)
					last_hit_distance = testTriangle(orig, dir, index+1, v0, v1, v2);		
			/*	
				if(last_hit_distance < MAX_DISTANCE){
					gl_FragData[0] = vec4(index.xxx,1.);
					return;
				}
			*/
				
			}else{ // we are outside the grid, do whatever. FINISH! 
				last_hit_distance = MAX_DISTANCE+1.;
				
				// write the incoming direction to this pass, and set ray finished.
				gl_FragData[0] = vec4(iDir.xyz,1.);
			//	gl_FragData[1] = vec4(0.,1.,0.,1.);
				return;
			}
			
			
				
			if (tMaxX < tMaxY){

				tMaxX += tDeltaX;
				X += stepX;

			}else{
			
				tMaxY += tDeltaY;
				Y += stepY;

			}			
			
		}
		
		// if last_hit_distance < MAX_DISTANCE
		// we have found a hit.
		// otherwise
		// the ray goes out.
		
		if(last_hit_distance < MAX_DISTANCE){		
			// compute the normal for the triangle.
			vec3 v01 = normalize(v1 - v0);
			vec3 v02 = normalize(v2 - v0);
			
			vec3 normal = normalize(cross(v01,v02));
		
			vec3 oDir = reflect(dir, normal);
			vec3 intersectedPoint = orig + dir * last_hit_distance;
			
			gl_FragData[0] = vec4(oDir,0.);// store reflected direction + status
			gl_FragData[1] = vec4(intersectedPoint + normal*0.001,1.);// store uv coordinates + heigh
		
		}else{ // the ray does not hit the surface
		
			gl_FragData[0] = vec4(iDir.xyz,1.);
			gl_FragData[1] = vec4(0.,1.,0.,1.);
		
		}
			
	}else{
		//information pass through
		gl_FragData[0] = iDir;
		gl_FragData[1] = vec4(1.,0.,1.,1.);
	}
	
	return;
}
