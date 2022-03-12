	
	vec3 finalColor = diffuse*gl_TexCoord[3].rgb+color;

	// set the final color as the output, applying the tone map
	gl_FragColor = vec4(colorCorrect(finalColor,keyGamma),1.);
}




