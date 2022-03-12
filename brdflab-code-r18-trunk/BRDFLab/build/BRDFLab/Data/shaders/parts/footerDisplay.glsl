void main()
{
	// Transform the vertex (ModelViewProj matrix)
//	gl_Position = ftransform();
	
	// Incident and exitant directions
	vec3 u = normalize(incidentVector); // incident position // light
	vec3 v = vertex.xyz; // exitant position // viewer

	// Transform the vertex position to spherical coordinates
	vec2 uv = cart2spherical(v);

	vec3 vPlusU = spherical2cart(uv + vec2(0.01,0.));
	vec3 vPlusV = spherical2cart(uv + vec2(0.,0.01));
	
	// Compute the intensities at each position
	float mainIntensity = transformReflectionValue( computeBRDF(u, v).w );
	float uIntensity = transformReflectionValue( computeBRDF(u, vPlusU).w );
	float vIntensity = transformReflectionValue( computeBRDF(u, vPlusV).w );
	
	vec3 mainPosition = v * mainIntensity;
	vPlusU = vPlusU * uIntensity;
	vPlusV = vPlusV * vIntensity;
	
	// Compute local vectors
	vec3 vectorU = normalize(vPlusU-mainPosition);
	vec3 vectorV = normalize(vPlusV-mainPosition);
	
	vec3 vnormal = cross(vectorV,vectorU);
	vectorU = cross(vnormal,vectorV);
	vnormal = cross(vectorV,vectorU);

	// Pass the position to the fragment shader
	gl_TexCoord[0] = vertex;

	// Pass the normal to the fragment shader
	gl_TexCoord[1] = vec4(normalize(vnormal),1.);//vec4(cross(vectorV,vectorU),1.);
	
	// Displace point
	// TODO: pow 0.33 in scatmech
	vec4 dpos = vec4(vertex.xyz * max(mainIntensity,0.), 1);

	// Output point in clip space
	gl_Position = gl_ModelViewProjectionMatrix*dpos;
}
