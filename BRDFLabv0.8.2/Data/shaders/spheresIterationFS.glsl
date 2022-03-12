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


//inputs:
// light position
// texture 
// height of the relief mapping
// 

uniform float nSpheres;


// grid 
uniform int width;
uniform int height;
uniform int depth;


uniform float texWidth;
uniform float indexWidth;
uniform float gridWidth;

uniform sampler2D data;
uniform sampler2D index;
uniform sampler2D grid;

uniform vec3 eyeDirection;

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

// parameters: origin, iDir, distance , normal
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


/*
Formulation from:
http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld013.htm
*/
void
intersectSphere(
	in vec3 O,
	in float r,
	in vec3 p0,
	in vec3 V,
	inout vec4 point,
	inout vec3 normal)
{

	float r2 = pow(r,2.);
	vec3 L = O - p0;
	float tca = dot(L,V);
	
	if(tca < 0.)
		return;
	
	float d2 = dot(L,L)-pow(tca,2.);
	if(d2 > r2)
		return;
		
	float thc = sqrt(r2-d2);
	
	float t = tca - thc;
	
	float t1 = tca + thc;
	
	if(t1 < t)
		t = t1;
	
	if(point.w < t)
		return;
	else{
		
		point.xyz = p0+t*V;
		point.w = t;
		normal = normalize(point.xyz - O);
	
	}	
}



void main()																					
{

	//compute the uv coordinates for this point
	// to find out the initial intersection uv coordinates.
	vec2 uv = gl_TexCoord[0].xy*0.5+0.5; 


//	gl_FragData[0] = vec4(texture2D(index,uv));
//	gl_FragData[1] = vec4(texture2D(grid,uv));
//	return;


	// ray information
	vec3 iPoint = vec3(uv,0.99);
	vec3 iDir = normalize(-eyeDirection);

	vec4 point = vec4(0.,0.,0.,100000000000.);
	vec3 normal = vec3(0.,0.,0.);
	
	
		// grid traversal
	
	// needed vars for grid traversal.
	int stepX;
	int stepY;
	int stepZ;
	
	float tMaxX;
	float tMaxY;
	float tMaxZ;
	
	float tDeltaX;
	float tDeltaY;
	float tDeltaZ;
	
	// find out the voxel position
	
	vec3 origGridSpace = vec3 (iPoint.x * float(width),iPoint.y * float(height), iPoint.z*float(depth) );
	
	int X = int(floor(origGridSpace.x));
	int Y = int(floor(origGridSpace.y));	 
	int Z = int(floor(origGridSpace.z));	 
	 
	// set the tMax and tDelta.
	
	if(iDir.x > 0.){
		tMaxX = length(rayPlaneIntersection(origGridSpace, iDir, float(X)+1., vec3(-1.,0.,0.))-origGridSpace);
		tDeltaX = length(rayPlaneIntersection(vec3(0.,0.,0.), iDir, 1., vec3(-1.,0.,0.)));
		stepX = 1;
	}else{
		tMaxX = length(rayPlaneIntersection(origGridSpace, iDir, -float(X), vec3(1.,0.,0.))-origGridSpace);
		tDeltaX = length(rayPlaneIntersection(vec3(0.,0.,0.), iDir, -1., vec3(1.,0.,0.)));
		stepX = -1;
	}
	
	if(iDir.y > 0.){
		tMaxY = length(rayPlaneIntersection(origGridSpace, iDir, float(Y)+1., vec3(0.,-1.,0.))-origGridSpace);
		tDeltaY = length(rayPlaneIntersection(vec3(0.,0.,0.), iDir, 1., vec3(0.,-1.,0.)));
		stepY = 1;
	}else{
		tMaxY = length(rayPlaneIntersection(origGridSpace, iDir, -float(Y), vec3(0.,1.,0.))-origGridSpace);
		tDeltaY = length(rayPlaneIntersection(vec3(0.,0.,0.), iDir, -1., vec3(0.,1.,0.)));
		stepY = -1;
	}

	if(iDir.z > 0.){
		tMaxZ = length(rayPlaneIntersection(origGridSpace, iDir, float(Z)+1., vec3(0.,0.,-1))-origGridSpace);
		tDeltaZ = length(rayPlaneIntersection(vec3(0.,0.,0.), iDir, 1., vec3(0.,0.,-1.)));
		stepZ = 1;
	}else{
		tMaxZ = length(rayPlaneIntersection(origGridSpace, iDir, -float(Z), vec3(0.,0.,1.))-origGridSpace);
		tDeltaZ = length(rayPlaneIntersection(vec3(0.,0.,0.), iDir, -1., vec3(0.,0.,1.)));
		stepZ = -1;
	}

	int gridPositions = width*height*depth;
	
	int outside = 0;
	
	while(outside == 0 && point.w == 100000000000.)
	{	
		float startPos = texture2D(grid,get2DCoord(float(Z*width*height+Y*width+X),gridWidth)).r * 65535.;
	
		float indexSphere = 0.;
	
		while (startPos != 65535.){
			vec4 spheresIndex = texture2D(index,get2DCoord(startPos,indexWidth)) * 65535.;
				
			vec4 sphereInfo;
			
			// test sphere 1
			if (spheresIndex.r != 65535.){
				sphereInfo = texture2D(data,get2DCoord(spheresIndex.r,texWidth));
				intersectSphere(sphereInfo.xyz, sphereInfo.w, iPoint, iDir, point, normal);
			}
			
			// test sphere 2
			if (spheresIndex.g != 65535.){
				sphereInfo = texture2D(data,get2DCoord(spheresIndex.g,texWidth));
				intersectSphere(sphereInfo.xyz, sphereInfo.w, iPoint, iDir, point, normal);
			}
			
			// test sphere 3	
			if (spheresIndex.b != 65535.){
				sphereInfo = texture2D(data,get2DCoord(spheresIndex.b,texWidth));
				intersectSphere(sphereInfo.xyz, sphereInfo.w, iPoint, iDir, point, normal);
			}	
			
			// test sphere 4
			if (spheresIndex.a != 65535.){
				sphereInfo = texture2D(data,get2DCoord(spheresIndex.a,texWidth));
				intersectSphere(sphereInfo.xyz, sphereInfo.w, iPoint, iDir, point, normal);
			}
			
			if(spheresIndex.a != 65535.)
				startPos += 1.;
			else
				startPos = spheresIndex.a;
				
		}
		
			
		if (tMaxX < tMaxY){
			if (tMaxX < tMaxZ){
				tMaxX += tDeltaX;
				X += stepX;
				if(X >= width || X < 0)
					outside = 1;
			}else{
				Z += stepZ;
				tMaxZ += tDeltaZ;
				if(Z >= depth || Z < 0)
					outside = 1;
			}
		}else{
			if(tMaxY < tMaxZ){
				Y += stepY;
				tMaxY += tDeltaY;
				if(Y >= height || Y < 0)
					outside = 1;
			}else{
				Z += stepZ;
				tMaxZ += tDeltaZ;
				if(Z >= depth || Z < 0)
					outside = 1;
			}	
		}		



	}
		
	if(point.w != 100000000000.){
	
		vec3 outgoingDirection = reflect(iDir.xyz, normal.xyz);
		
		gl_FragData[0] = vec4(outgoingDirection.xyz,0.);// store reflected direction + status
		gl_FragData[1] = vec4(point.xyz + normal.xyz*0.001,100000000000.);// store uv coordinates + heigh
	
	}else{
	
		gl_FragData[0] = vec4(1.,0.,-1.,1.);
		gl_FragData[1] = vec4(0.,1.,0.,1.);
	
	}
	
	return;
	
}
