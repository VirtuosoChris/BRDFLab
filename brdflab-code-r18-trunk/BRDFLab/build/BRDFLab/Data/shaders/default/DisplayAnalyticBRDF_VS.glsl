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

uniform vec3 incidentVector;	// incident direction

uniform float fixed;			// implementation (fixed if true(1) and linear if false(0))		// TODO: use this as a define?
uniform float channels;			// number of channels/wavelenghts					// TODO: use this as a define?
uniform float lobes;			// number of lobes
uniform float params[100];		// list of parameters (lobe type, scale factor, BRDF parameters, ...)	// TODO: 100?
uniform vec3 scale;				// global color scale (e.g. to choose a channel)

uniform float cubeRoot;
uniform float applyLog;


void main()
{
	gl_Position = vec4(0.,0.,0.,0.);
}
