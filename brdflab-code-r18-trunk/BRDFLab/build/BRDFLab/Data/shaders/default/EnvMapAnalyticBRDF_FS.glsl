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

// uniform parameters

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

void main()
{
	gl_FragColor = vec4(0.,0.,0.,0.);
}
