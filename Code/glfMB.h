GLchar* glfShaderCode =
"#version 130\n"
"uniform vec2 cntr;"
"uniform float scal;"

"void main() {"

	"vec2 c;"
	"c.x = 1.7777 * (gl_TexCoord[0].x - 0.5) * scal - cntr.x;"
	"c.y = (gl_TexCoord[0].y - 0.5) * scal - cntr.y;"

	// It is worth noting new graphic cards may support doubles for better results.
	"highp vec2 z = c;"
	"highp vec2 z2;"

	"int i;"

	// Iterate to 250; The limitations of 24-bit FP would render higher iterations less useful.
	"for(i = 0; i < 250; i++) {"

		"z2 = z * z;"

		"if (z2.x + z2.y > 4.0)"
			"break;"

		"z.y = z.x * z.y;"
		"z.y += z.y + c.y;"
		"z.x = z2.x - z2.y + c.x;"

	"}"

	// A bit of a Hack, but I really like the colors
	"gl_FragColor = vec4(0.5 + cos(3.0 + float(i)/4 + vec3(0.0, 0.5, 1.0))/2, 1.0);"

"}";
