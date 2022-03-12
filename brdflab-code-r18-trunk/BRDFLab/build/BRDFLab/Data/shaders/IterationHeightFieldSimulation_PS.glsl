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

//uniform int tilt;
uniform float depth;

uniform sampler2D map;
uniform sampler2D directions;
uniform sampler2D hits;

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
		float incDepth = curDepth/float(nSteps);
		
		vec2 incDisp = depth*disp/(5.*float(nSteps));
		for( int i = 0; i < nSteps; i++ )
		{
			float reliefHeight = 1.-texture2D( map, p.xy ).w;
			if ( curDepth < reliefHeight )
			{
				p += incDisp;
				curDepth -= incDepth;
			}
			else break;
		}
		
		if(curDepth > 0.05){
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
		}else{
			curDepth = 2000.;
		}
	
	}
	return vec3(p.xy,curDepth);
}

/**

Information to access textures:

map texture: uv
data textures: vec2(uv.x,1-uv.y)

directions texture:
RGB -> direction
A -> status (0->compute intersection, 1-> finished)

hit texture:
RG -> uv coordinates of the hit
B -> height

*/



void main()																					
{
	vec2 uv = gl_TexCoord[0].xy*0.5+0.5; 
	
	// recover direction
	vec4 dir = texture2D(directions,vec2(uv.x,1.-uv.y));
	
	// compute intersection (status=0) or passthrough (status=1)
	if(dir.a != 1.){
	
		vec4 hit = texture2D(hits,vec2(uv.x,1.-uv.y));
		
		// compute relief mapping from the specified direction and hit point
		vec3 hitPoint = relief(hit.xy,dir.xyz,hit.z-0.0001);
		
		if(hitPoint.z != 2000.){ // the ray comes out the surface
			
			// reflect the ray given the normal at the intersected point
			vec3 textureNormal = normalize(expand(texture2D(map,hitPoint.xy).xyz));
			
			vec3 reflectedDir = reflect(dir.xyz,textureNormal);
		
			gl_FragData[0] = vec4(reflectedDir,0.);
			gl_FragData[1] = vec4(hitPoint.xy,texture2D(map,hitPoint.xy).a,1.);
		
		}else{

			gl_FragData[0] = vec4(dir.xyz,1.);
			gl_FragData[1] = vec4(1.,1.,0.,1.);

		}
		
	
	}else{
		
		//information pass through
		gl_FragData[0] = dir;
		gl_FragData[1] = vec4(1.,0.,1.,1.);
	}

	return;
	
	
	
}
