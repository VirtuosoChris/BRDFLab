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
attribute vec3 normal;
attribute vec3 tangent;

uniform mat4 worldMatrix;


void main()																					
{																							
	//Transform the vertex (ModelViewProj matrix)											
	gl_Position = ftransform();

	//parameters to fragment shader
	gl_TexCoord[0] = vec4(normalize((worldMatrix*vec4(vertex.xyz,1.)).xyz),1.); //vertex in world coordinates
	gl_TexCoord[1] = vec4(normalize((worldMatrix*vec4(normal.xyz,1.)).xyz),1.); // normal in world coordinates
	gl_TexCoord[2] =vec4(normalize((worldMatrix*vec4(tangent.xyz,1.)).xyz),1.); // tangent in world coordinates
}
