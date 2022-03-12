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
attribute vec3 normal;
//attribute vec3 tangent;

uniform vec4 lightPos;
uniform vec4 eyePos;

uniform mat4 worldMatrix;

void main()																					
{				
																			
	//Transform the vertex (ModelViewProj matrix)											
	gl_Position = ftransform();	//gl_ModelViewProjectionMatrix*vertex;//

	vec3 color = vec3( 0.,1.,0.);
	float ambient = 0.3;
	float diffuse = 0.6;
	float specular = 0.;
	float n = 100;
	

	vec3 normal = normalize((worldMatrix*vec4(normal,1.)).xyz);
	

	vec3 position = (worldMatrix*vertex).xyz;
	
	
	vec3 viewerDirection  = normalize(eyePos.xyz-position);
	
	vec3 u = normalize(eyePos.xyz-position);
	vec3 v = normalize(reflect(-u,normal));
	
	float dotVH = dot(v,viewerDirection);
	
	if(dotVH > 0.)
		specular = pow(dotVH,n);
	
	
	diffuse = dot(normal,u);
	
	
//	gl_TexCoord[0] = vec4(normal,1.);										
	gl_TexCoord[0] = vec4(color*ambient+color*diffuse+vec3(1.,1.,1.)*specular,1.);
															
}
