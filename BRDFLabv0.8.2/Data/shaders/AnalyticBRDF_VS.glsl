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

attribute vec4 vertex;

uniform vec3 incidentVector;					// incident direction

uniform float fixed;				// implementation (fixed if true(1) and linear if false(0)		// TODO: use this as a define?
uniform float channels;			// number of channels/wavelenghts					// TODO: use this as a define?
uniform float lobes;				// number of lobes
uniform float params[100];		// list of parameters (lobe type, scale factor, BRDF parameters, ...)	// TODO: 100?
uniform vec3 scale;				// global color scale (e.g. to choose a channel)

uniform float cubeRoot;
uniform float applyLog;

vec3 spherical2cart( vec2 uv )
{
	vec3 val = vec3(0.,0.,0.);

	val.x = sin(uv.y)*cos(uv.x);
	val.y = sin(uv.y)*sin(uv.x);
	val.z = cos(uv.y);
	

	return val;
}


vec2 cart2spherical( vec3 pos )
{

	// code from colbert et al.

	vec3 npos = normalize(pos);
	
	float phi = atan(npos.y,npos.x);
	phi = (phi>0.)?phi:((2.*3.1415926535) + phi);
	
	float theta = acos(npos.z);
	
	return vec2(phi,theta);
}

void main()
{
gl_Position = vec4(0.,0.,0.,0.);
}
