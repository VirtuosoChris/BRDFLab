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

// Uniform parameters

// total amount of the lambertian term
uniform vec3 diffuse;

// camera position.
uniform vec4 eyePos;

// tone mapping parameters
uniform float key;
uniform float gamma;

// dual paraboloid env. maps.
uniform sampler2D front;
uniform sampler2D back;


uniform float fixed;	// implementation (fixed if true(1) and linear if false(0)		// TODO: use this as a define?
uniform float channels;	// number of channels/wavelenghts					// TODO: use this as a define?
uniform float lobes;	// number of lobes
uniform int nSamplesPerLobe;

#define MAXPARAMETERS 100

// specular lobes parameters
uniform float params[MAXPARAMETERS];		// list of parameters (lobe type, scale factor, BRDF parameters, ...)	// TODO: 100?

#define MAXSAMPLES 100
uniform vec2 samples[MAXSAMPLES];


// definition of the coordinateTransformations.glsl file functions used
vec3 spherical2cart(vec2);
vec2 cart2spherical(vec3);


// Get a filtered sample from a dual-paraboloid
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

// Tonemap and gamma-correct (extracted from Colbert et al.)
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

mat3 constructCoordFrame(vec3 u) {
	
	mat3 smpl2Local;
	
	smpl2Local._m20_m21_m22 = u;
	smpl2Local._m00_m01_m02 = normalize(vec3(0.,u.z,-u.y));
	smpl2Local._m10_m11_m12 = cross(smpl2Local._m20_m21_m22,smpl2Local._m00_m01_m02);

/*
	vec3 normal = u;
	vec3 tangent = normalize(vec3(-u.z, 0., u.x));
	vec3 binormal = cross(normal,tangent);
	
	mat3 smpl2Local = mat3(tangent,binormal,normal);
	*/
	return smpl2Local;
}
