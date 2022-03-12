/**

Input parameters from vertex shader
	gl_TexCoord[0] = vertex
	gl_TexCoord[1] = normal
	gl_TexCoord[2] = tangent

*/


void main()
{
	vec2 keyGamma = vec2(key,gamma);

	// Local frame
	vec3 vertexWorld = normalize(gl_TexCoord[0].xyz);
	vec3 normal = normalize(gl_TexCoord[1].xyz);
	vec3 tangent = normalize(gl_TexCoord[2].xyz);
	
	// Compute TBN matrix
	vec3 binormal = cross(normal,tangent);
	mat3 tangentspace = mat3(tangent,binormal,normal);
	tangentspace = transpose(tangentspace);
	
	// Pass eye and light vectors to tangent space
	vec3 eyeVector = normalize(eyePos.xyz - vertexWorld.xyz);
	vec3 lightVector = normalize(lightPos.xyz - vertexWorld.xyz);
	
	vec3 eyeVectorTangentSpace = tangentspace*eyeVector;
	vec3 lightVectorTangentSpace = tangentspace*lightVector;	
	
	vec3 intensity = computeBRDF(lightVectorTangentSpace, eyeVectorTangentSpace).xyz;
											
	if(channels == 1.)
		gl_FragColor = vec4(colorCorrect(vec3(1.,1.,1.)*intensity.x*max(0.,dot(lightVector, normal.xyz)), keyGamma),1.);
	else
		gl_FragColor = vec4(colorCorrect(intensity.xyz*max(0.,dot(lightVectorTangentSpace, vec3(0.,0.,1.))), keyGamma),1.);
		//gl_FragColor = vec4(colorCorrect(intensity.xyz*max(0.,dot(lightVector, normal.xyz)), keyGamma),1.);
}
