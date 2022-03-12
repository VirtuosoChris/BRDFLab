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

#version 120

uniform int tilt;
uniform float depth;
uniform sampler2D map;

/*

	gl_TexCoord[0] = pos; // pos camera space
	gl_TexCoord[1] = eye; // light camera space
	gl_TexCoord[2] = gl_ModelViewMatrix*vec4(normal.xyz,0.); // normal
	gl_TexCoord[3] = gl_ModelViewMatrix*vec4(tangent.xyz,0.); // tangent
	gl_TexCoord[4] = vec4(vertex,1.); // uv

*/

// Expand a range-compressed vector
vec3 expand(vec3 v)
{
	return (v - 0.5) * 2.;
}


vec3
relief(vec2 uv, vec3 viewDirection, float curDepth){

	vec2 p = uv.xy;

	if(viewDirection.z < 0.){
	
		float hDisp = 1.-curDepth;
	
		float scaleFactor = 1./abs(viewDirection.z);
		
		vec2 disp = viewDirection.xy*scaleFactor*hDisp;
		
		int nSteps = 50;
		float incDepth = hDisp/float(nSteps);
		
		vec2 incDisp = depth*disp/(5.*float(nSteps));
		for( int i = 0; i < nSteps; i++ )
		{
			float reliefHeight = 1.-texture2D( map, p.xy ).w;
			if ( curDepth < reliefHeight )
			{
				p += incDisp;
				curDepth += incDepth;
			}
			else break;
		}
		
		float inc = incDepth;
		vec2 v = incDisp;
		for(int i=0; i<50;i++)
		{
			v *= 0.5;
			inc *= 0.5;
			float reliefHeight = 1.-texture2D( map, p.xy ).w;
			if ( curDepth < reliefHeight ){
				p += v;
				curDepth += inc;
			}
			else if ( curDepth > reliefHeight ){
				p -= v;
				curDepth -= inc;
			}
	//		else break;
		}
	
	}else{ // going up
	
		float hDisp = curDepth;
	
		float scaleFactor = 1./abs(viewDirection.z);
		
		vec2 disp = viewDirection.xy*scaleFactor*hDisp;
		
		int nSteps = 50;
		float incDepth = hDisp/float(nSteps);
		
		vec2 incDisp = depth*disp/(5.*float(nSteps));
		for( int i = 0; i < nSteps; i++ )
		{
			float reliefHeight = 1.-texture2D( map, p.xy ).w;
			if ( curDepth > reliefHeight )
			{
				p += incDisp;
				curDepth -= incDepth;
			}
			else break;
		}
		
		float inc = incDepth;
		vec2 v = incDisp;
		for(int i=0; i<50;i++)
		{
			v *= 0.5;
			inc *= 0.5;
			float reliefHeight = 1.-texture2D( map, p.xy ).w;
			if ( curDepth > reliefHeight ){
				p += v;
				curDepth += inc;
			}
			else if ( curDepth < reliefHeight ){
				p -= v;
				curDepth -= inc;
			}
	//		else break;
		}	
	
	}
	return vec3(p.xy,curDepth);
}




void main()																					
{
	vec2 uv = gl_TexCoord[4].xy*float(tilt);
		
	vec3 normal = normalize(gl_TexCoord[2].xyz);
	vec3 tangent = normalize(gl_TexCoord[3].xyz);
	
	// compute tbn matrix
	vec3 binormal = cross(normal,tangent);
	mat3 localToWorld = mat3(tangent,binormal,normal);
	mat3 worldToLocal = transpose(localToWorld);
	
	vec3 viewDirection = worldToLocal*gl_TexCoord[0].xyz;
	
	vec3 p = relief(uv,vec3(viewDirection.xy,viewDirection.z), 0.);
	
	// get normal
	vec3 normalHit = expand(texture2D(map,p.xy).rgb);
	float nDotL = max(dot(worldToLocal*vec3(0.,0.,1.),normalHit),0.0); // Convert light to TBN space.
	gl_FragColor = vec4(0.,nDotL,0.,1.);
	return;


}
