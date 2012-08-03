#version 110

uniform sampler2D textures[1];
uniform float offset;

varying vec2 texcoord;

void main() {
	vec2 history = vec2(mod(texcoord.x + offset + 0.2, 1.0), texcoord.y);
	vec2 current = vec2(mod(offset * 1.0, 1.0), texcoord.y);
	gl_FragColor = texcoord.x > 0.8 ? texture2D(textures[0], current) : texture2D(textures[0], history);
}
