
/**
Parameters from the Vertex Shader:

	//parameters to fragment shader
	gl_TexCoord[0] // vertex in world coordinates
	gl_TexCoord[1] // normal in world coordinates
	gl_TexCoord[2] // tangent in world coordinates

	//diffuse component computed with spherical harmonics
	gl_TexCoord[3].rgb //diffuse component computed with spherical harmonics
*/



void main()																					
{	
	// tone mapping
	vec2 keyGamma = vec2(key,gamma);

	vec3 vertexWorld = gl_TexCoord[0].xyz;
	vec3 normal = normalize(gl_TexCoord[1].xyz);
	vec3 tangent = normalize(gl_TexCoord[2].xyz);
//	vec3 binormal = normalize(gl_TexCoord[4].xyz);
	
	// compute tbn matrix
	vec3 binormal = cross(normal,tangent);
	mat3 localToWorld = mat3(tangent,binormal,normal);
	mat3 worldToLocal = transpose(localToWorld);
	
	// outgoing vector
	vec3 eyeVectorWorld = normalize(eyePos.xyz - vertexWorld.xyz);
	vec3 eyeVectorTangent = worldToLocal*eyeVectorWorld;
	
	eyeVectorTangent.z = max(eyeVectorTangent.z,0.01);
	eyeVectorTangent = normalize(eyeVectorTangent);

	float preLoD = 0.5*log2(512.*512./float(nSamplesPerLobe)/(2.*3.14159))+1.;

	// final pixel color
	vec3 color = vec3(0.,0.,0.);
	vec3 dir = vec3(0.,0.,0.);
	vec3 sfactor = vec3(0.,0.,0.);
	vec4 lobeColor = vec4(0.,0.,0.,0.);
	
	// index to access the right BRDF parameter
	int count = 0;

	//if it is not halfangle ...	
	// matrix to transform from local space to tangent space.
	mat3 specularToTangent = mat3(0.,0.,0.,0.,0.,0.,0.,0.,0.);	
	
	