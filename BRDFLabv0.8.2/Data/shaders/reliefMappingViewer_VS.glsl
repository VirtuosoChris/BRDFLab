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
//attribute vec4 uv0;
//attribute vec3 normal;
//attribute vec3 tangent;


//uniform mat4 worldMatrix;

uniform vec4 eyePos;
uniform vec4 lightPos;

void main()																					
{																	
	//Transform the vertex (ModelViewProj matrix)											
	gl_Position = ftransform();	//gl_ModelViewProjectionMatrix*vertex;//
		
	// light in view space
	vec4 light = gl_ModelViewMatrix*lightPos;
	
	// vertex position in view space
	vec4 pos = gl_ModelViewMatrix*vertex;
	
	gl_TexCoord[0] = pos;
	gl_TexCoord[1] = light;
	gl_TexCoord[2] = gl_ModelViewMatrix*vec4(0.,0.,1.,0.); // normal
	gl_TexCoord[3] = gl_ModelViewMatrix*vec4(1.,0.,0.,0.); // tangent
	gl_TexCoord[4] = vertex;


}