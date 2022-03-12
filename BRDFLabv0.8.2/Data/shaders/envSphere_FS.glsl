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

// tone mapping parameters
uniform float key;
uniform float gamma;

// dual paraboloid env. maps.
uniform sampler2D front;
uniform sampler2D back;


/// Get a filtered sample from a dual-paraboloid
vec4 getSample(sampler2D front, sampler2D back, vec4 u_w) {

	float scaleParabolidMap = 1./2.4;//1.7;//

	if (u_w.z < 0) {
		vec4 texcoord = vec4(u_w.xy/(1-u_w.z)*(scaleParabolidMap)+0.5,u_w.ww);		
		return texture2DLod(front, texcoord.xy,texcoord.z);
	} else {
		vec4 texcoord = vec4(-u_w.xy/(1+u_w.z)*(scaleParabolidMap)+0.5,u_w.ww);
		return texture2DLod(back, texcoord.xy,texcoord.z);
	}
}


/// Tonemap and gamma-correct
vec3 colorCorrect(vec3 c, vec2 keyGamma) {
	// uncomment for no gamma correction or tone mapping
	// (useful for obtaining HDR images out of the system)
	//return c;
	
	vec3 lum = vec3(0.2126, 0.7152, 0.0722);
	
	/// TONE MAPPING
	// compress the luminance domain
	vec3 L_r;
	L_r.x = dot(lum,c);
	L_r.y = L_r.x*keyGamma.x;
	L_r.z = L_r.y / (1. + L_r.y);
	c *= L_r.z/L_r.x;
	
	// correct for the gamma
	c = pow(c,keyGamma.yyy);
	
	return c;
}


/**
Parameters from the Vertex Shader:

	//parameters to fragment shader
	gl_TexCoord[0] // normal
*/



void main()																					
{	
	// tone mapping
	vec2 keyGamma = vec2(key,gamma);

	vec3 L = getSample(front,back,vec4(gl_TexCoord[0].xyz,0.)).rgb;

	//set the final color as the output, applying the tone map.
	gl_FragColor = vec4(colorCorrect(L,keyGamma),1.);
	
}




