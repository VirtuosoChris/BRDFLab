#version 120																				
#extension GL_EXT_geometry_shader4 : enable		
 
// a passthrough geometry shader for color and position 
void main() 
{ 

	vec3 v0 = gl_TexCoordIn[0][0].xyz;
	vec3 v1 = gl_TexCoordIn[1][0].xyz;
	vec3 v2 = gl_TexCoordIn[2][0].xyz;

	vec3 v01 = normalize(v1-v0);
	vec3 v02 = normalize(v2-v0);

	vec3 normal = cross(v01,v02);

	vec3 color = vec3(0.,0.,1.);

	gl_Position = gl_PositionIn[0];
	gl_TexCoord[0] = gl_TexCoordIn[0][0];
	gl_TexCoord[1] = vec4(normal,1.);
	EmitVertex(); 

	gl_Position = gl_PositionIn[1];       
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	gl_TexCoord[1] = vec4(normal,1.);
	EmitVertex(); 

	gl_Position = gl_PositionIn[2];
	gl_TexCoord[0] = gl_TexCoordIn[2][0];
	gl_TexCoord[1] = vec4(normal,1.);
	EmitVertex(); 

/*
	for(int i=0; i<gl_VerticesIn; i++){
		gl_Position = gl_PositionIn[i];
		gl_TexCoord[2] = vec4(1.,0.,0.,1.);
		EmitVertex(); 
	}
*/
	EndPrimitive();	
} 
