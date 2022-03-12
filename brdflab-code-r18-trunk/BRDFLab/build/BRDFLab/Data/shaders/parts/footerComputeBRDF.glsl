	return vec4(color.xyz, max((color.r + color.g + color.b), 0.)); // RGB + current value
}
