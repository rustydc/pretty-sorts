#version 110

uniform float offset;

attribute vec2 position;
varying vec2 texcoord;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
/*	vec2 tmp= position * vec2(0.5) + vec2(0.5);
	texcoord = vec2(mod(tmp.x + offset, 1.0), tmp.y);*/
	texcoord = vec2(position.x * 0.5 + 0.5, position.y * 0.5 + 0.5);
}
