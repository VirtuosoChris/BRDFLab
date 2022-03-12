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

uniform int tilt;
uniform float depth;

uniform sampler2D map;
uniform vec3 eyeDirection;

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
		float incDepth = 1./float(nSteps);
		
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
		float incDepth = 1./float(nSteps);
		
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

	//compute the uv coordinates for this point
	// to find out the initial intersection uv coordinates.
	vec2 uv = gl_TexCoord[0].xy*0.5+0.5; 
	
	uv *= float(tilt);

	vec3 textureNormal;
	
	vec3 lightDir = normalize(-eyeDirection);

	vec3 hitPoint = relief(uv,lightDir,0.);

	//relief
	textureNormal = normalize(expand(texture2D(map,hitPoint.xy).xyz));
		
	vec3 reflectedDir = reflect(lightDir,textureNormal);
	
	gl_FragData[0] = vec4(reflectedDir,0.);// store reflected direction + status
	gl_FragData[1] = vec4(hitPoint,1.);// store uv coordinates + heigh
	
	
}
