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

uniform vec3 lightPos;
uniform vec3 eyePos;
uniform float key;
uniform float gamma;

uniform float channelsToDisplay;
uniform sampler3D brdfData;

#define PI 3.14159265

// definition of the coordinateTransformations.glsl file functions used
vec3 spherical2cart(vec2);
vec2 cart2spherical(vec3);

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

Input parameters from vertex shader
	gl_TexCoord[0] = vertex
	gl_TexCoord[1] = normal
	gl_TexCoord[2] = tangent

*/


void main()																					
{

	vec2 keyGamma = vec2 (key,gamma);

	vec3 vertexWorld = gl_TexCoord[0].xyz;
	vec3 normal = normalize(gl_TexCoord[1].xyz);
	vec3 tangent = normalize(gl_TexCoord[2].xyz);
	
	// compute tbn matrix
	vec3 binormal = cross(normal,tangent);
	mat3 tangentspace = mat3(tangent,binormal,normal);
	tangentspace = transpose(tangentspace);
	
	
	// pass eye and light vectors to tangent space
	vec3 eyeVector = normalize(eyePos.xyz - vertexWorld.xyz);
	vec3 lightVector = normalize(lightPos.xyz - vertexWorld.xyz);
	
	vec3 eyeVectorTangentSpace = tangentspace*eyeVector;
	vec3 lightVectorTangentSpace = tangentspace*lightVector;
	
	vec2 incident = cart2spherical(lightVectorTangentSpace); // phi_i, theta_i
	vec2 outgoing = cart2spherical(eyeVectorTangentSpace); // phi_o, theta_o
	
	// rotation about phi angle.
	outgoing.x -= incident.x;
	
	if(outgoing.x < 0.)
		outgoing.x += 2.*PI;
	
	float cosThetaIn = cos(incident.y);
		
	// normalize to access the texture
	incident.y /= PI/2.;
	outgoing.x /= 2.*PI;
	outgoing.y /= PI/2.;
	
	// increment half a texture's pixel to read the center of the texel when accessing the 3D texture.
	vec2 vary = vec2(1./360.,1./180.);

	outgoing.x = mix(0.+vary.x, 1.-vary.x, outgoing.x);
	outgoing.y = mix(0.+vary.y, 1.-vary.y, outgoing.y);
	incident.y = mix(0.+vary.y, 1.-vary.y, incident.y);	
	
	vec3 intensity;
	
	if(channelsToDisplay == 1.)
		intensity = cosThetaIn*texture3D(brdfData,vec3(outgoing.x + 1./360., outgoing.y + 1./180., incident.y + 1./180.)).x*vec3(1.,1.,1.);
	else
		intensity = cosThetaIn*texture3D(brdfData,vec3(outgoing.x + 1./360., outgoing.y + 1./180., incident.y + 1./180.)).xyz;

	gl_FragColor = vec4(colorCorrect(intensity.xyz*max(0.,dot(lightVector, normal.xyz)), keyGamma),1.);
}
