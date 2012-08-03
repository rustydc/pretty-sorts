#version 110

uniform sampler2D textures[1];
uniform sampler1D lut;
uniform float offset;

varying vec2 texcoord;

void main() {
	vec2 history = vec2(mod(texcoord.x + offset + 0.1, 1.0), texcoord.y);
	vec2 current = vec2(mod(offset * 1.0, 1.0), texcoord.y);

	vec4 tmp = texcoord.x > 0.9 ? (texcoord.x < 0.905 ? vec4(0,0,0,1) : texture2D(textures[0], current) ) : texture2D(textures[0], history);
	gl_FragColor = texture1D(lut, tmp.r);
}
