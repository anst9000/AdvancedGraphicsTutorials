#version 130

// ------------------------------------------ //
// The fragment shader operates on each pixel //
// in a given polygon						  //
// ------------------------------------------ //

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

// A 4-component float vector that is being output
// to the screen for each pixel.
out vec4 color;

uniform sampler2D playerSampler;

void main() {
	vec4 textureColor = texture(playerSampler, fragmentUV);

	color = fragmentColor * textureColor;
}