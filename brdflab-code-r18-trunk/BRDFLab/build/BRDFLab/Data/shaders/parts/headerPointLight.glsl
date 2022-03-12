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

uniform float fixed;			// implementation (fixed if true(1) and linear if false(0)		// TODO: use this as a define?
uniform float channels;			// number of channels/wavelenghts					// TODO: use this as a define?
uniform float lobes;			// number of lobes

#define MAXPARAMETERS 100

uniform float params[MAXPARAMETERS];		// list of parameters (lobe type, scale factor, BRDF parameters, ...)	// TODO: 100?

uniform vec3 scale;				// global color scale (e.g. to choose a channel)

uniform vec3 lightPos;
uniform vec3 eyePos;
uniform float key;
uniform float gamma;


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
