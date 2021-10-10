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

uniform float time;
uniform sampler2D playerSampler;

void main() {
	vec4 textureColor = texture(playerSampler, fragmentUV);

	color = vec4(
		fragmentColor.r * (cos(fragmentPosition.x * 4.0 + time) + 1.0) * 0.5,
		fragmentColor.g * (cos(fragmentPosition.y * 8.0 + time) + 1.0) * 0.5,
		fragmentColor.b * (cos(fragmentPosition.x * 0.4 + time) + 1.0) * 0.5,
		fragmentColor.a) * textureColor;
}