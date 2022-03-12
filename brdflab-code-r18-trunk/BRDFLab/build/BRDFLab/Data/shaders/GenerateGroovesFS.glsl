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



//////////////////////////// CONSTANTS & FIELDS ////////////////////////////

#define MAX_PROF_POINTS		5	// ***

#define NPOINTS x		// profile points
#define B x
#define N y
#define P1 xy
#define P2 zw

#define BEG xy			// facet
#define END zw

#define PBEG x			// projected facet
#define PEND y

#define NO_PRIO 0		// facets priority
#define PRIO_LEFT 1
#define PRIO_RIGHT 2
#define PRIO_BOTH 3


uniform float npoints;
uniform vec4 prof1;
uniform vec4 prof2;
uniform vec4 prof3;

void main()																					
{			

//	gl_FragColor = vec4( 0.,1.,0., 1. );
//	gl_FragDepth =  1.;
//	return;

					
	float dist = gl_TexCoord[0].x;
	float pfPoints = npoints;
	vec2 binormal = gl_TexCoord[0].yz;
	
	vec2 profile[MAX_PROF_POINTS];
	profile[0] = prof1.P1;
	profile[1] = prof1.P2;
	profile[2] = prof2.P1;
	profile[3] = prof2.P2;
	profile[4] = prof3.P1;
	
	////// Visibility

	float f1;			// facet index
	float h1;			// ray-facet intersection point (height only)
	vec2 N1;			// normal
	
	// First facet (surface)
	vec4 facet = vec4( -1., 0., profile[0] );
	
	// Profile facets
	for ( float p = 1.; (p < float(MAX_PROF_POINTS)) &&  (p < pfPoints); ++p )
	{
		// Current facet and its projection in the given direction
		facet = vec4( facet.END, profile[int(p)]);

		// Take into account the positive or negative sense of projection
		if ( ( facet.x < dist && dist < facet.z ) ||
			 ( facet.z < dist && dist < facet.x ) )
		{
			float t = ( dist - facet.x ) / ( facet.z - facet.x );
			float h = facet.y + t * ( facet.w - facet.y );

			f1 = p;
//			gl_FragColor = vec4(f1.xxx, 1 );//* 0.95 );
			h1 = h;
			N1 = normalize( vec2( facet.BEG.y - facet.END.y, facet.END.x - facet.BEG.x ) );

//			++n;
			p = pfPoints;
		}
	}
	
	//gl_FragColor = vec4(gl_TexCoord[0].xyz, 1 );//* 0.95 );
	//return;

	////// Output relief data

	float d = -h1;// / (hashTexSize * (MAX_HEIGHT - MAX_DEPTH));	// normalized depth [0..1]	// ***
	vec3 N = vec3( N1.x * binormal, N1.y );

	// Output normal (RGB) and depth (A)
	//gl_FragColor = vec4( N * 0.5 + 0.5, d*10 );//* 0.95 );		// adapt depth to [0..0.95]		// ***
	gl_FragColor = vec4( N * 0.5 + 0.5, 1.0 - abs(dist)/abs(prof1.x) * abs(prof1.w) * 5. );

	gl_FragDepth =  d * 10.;		// to improve depth test accuracy
}