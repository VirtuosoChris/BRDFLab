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



uniform vec4 color;
uniform vec4 eyePos;


/**

Input parameters:

gl_TexCoord[0] -> vertex position
gl_TexCoord[1] -> vertex normal

*/




void main()																					
{							
	vec3 eyeDir = normalize(eyePos.xyz - gl_TexCoord[0].xyz);
	vec3 L = normalize((eyePos.xyz+vec3(0.,0.,120.)) - gl_TexCoord[0].xyz);
	vec3 N = gl_TexCoord[1].xyz;
	vec3 halfVector = (L+eyeDir)/2.;
	
	float diffuse = max(dot(N,L),0.);
	float specular = pow(max(0., dot(halfVector, N)), 150.);		
		
		
	//paint the lobe normal															
	gl_FragColor = vec4(diffuse*color.xyz+specular*vec3(1.,1.,1.),1.);//vec4(gl_TexCoord[1].xyz+eyePos.xyz*0.0000001,1);//
//	gl_FragColor = vec4(gl_TexCoord[1].xyz,1.); // paint normal
}
