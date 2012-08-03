#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <sys/time.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include "sort.h"

extern void glutMainLoopEvent(void);

#define T 1024

static void render();
static int make_resources();
static GLuint make_texture(GLbyte* buf);
static GLuint make_lut();
static GLuint make_buffer(
	GLenum target,
	const void *buffer_data,
	GLsizei buffer_size );
static GLuint make_shader(GLenum type, const char *filename);
static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);
static void show_info_log(
		GLuint object,
		PFNGLGETSHADERIVPROC glGet__iv,
		PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);
void *file_contents(const char *filename, GLint *length);

unsigned short t = 0;
GLbyte buffer[T*N*3];

static struct {
	GLuint vertex_buffer, element_buffer;
	GLuint textures[1];
	GLuint lut;
	GLuint vertex_shader, fragment_shader, program;

	GLbyte buffer[T*N*3];

	struct {
		GLint textures[1];
		GLint lut;
		GLfloat offset;
	} uniforms;

	struct {
		GLint position;
	} attributes;

	float offset;
} g_resources;

static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	-1.0f,  1.0f,
	 1.0f,  1.0f
};
static const GLushort g_element_buffer_data[] = {0, 1, 2, 3};

buf *b; 
int main(int argc, char **argv) {
	b = malloc(sizeof(buf));
	b->data = malloc(sizeof(uint16_t) * 512);
	b->length = 512;
	memset(b->data, 0, b->length);

	memset(buffer, T*N*3, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(T, N);
	glutCreateWindow("Sorting");
	glutDisplayFunc(&render);
	glewInit();
	
	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "OpenGL 2.0 needed\n");
		return 1;
	}

	if (!make_resources()) {
		fprintf(stderr, "Failed to load resources\n");
		return 1;
	}

	run_sorts(b);
	//glutMainLoop();
	printf("Done.\n");

	return 0;
}

// Add a row with the current array.
void sample(float cost) {
	static int samples = 0;
	samples++;


	if (samples % 4 != 0) { return; };

	for (int i = 0; i != b->length; i++) {
		// New index = (3 * i) + (3 * T)
		//int index = 3 * ((t+1)%T) + 3 * i * b->length;
		int index = 3 * ((t+1)%T) + 3 * i * T;
		float scale = (float)256/(b->length);
		// Copy each short to an RGB triple.
		buffer[index] = b->data[i] * scale;
		buffer[index+1] = b->data[i] * scale;
		buffer[index+2] = b->data[i] * scale;
	}

	if (++t == T) {
		t = 0;
	}

	glutMainLoopEvent();
	glutPostRedisplay();
}

static int make_resources() {

	g_resources.vertex_buffer = make_buffer(
		GL_ARRAY_BUFFER,
		g_vertex_buffer_data,
		sizeof(g_vertex_buffer_data));
	g_resources.element_buffer = make_buffer(
		GL_ELEMENT_ARRAY_BUFFER,
		g_element_buffer_data,
		sizeof(g_element_buffer_data));

	g_resources.textures[0] = make_texture(buffer);
	if (g_resources.textures[0] == 0 ) {
		return 0;
	}

	g_resources.lut = make_lut();
	if (g_resources.lut == 0 ) {
		return 0;
	}

	g_resources.vertex_shader = make_shader(
		GL_VERTEX_SHADER,
		"gl.v.glsl");
	if (g_resources.vertex_shader == 0) return 0;

	g_resources.fragment_shader = make_shader(
		GL_FRAGMENT_SHADER,
		"gl.f.glsl");
	if (g_resources.fragment_shader == 0) return 0;

	g_resources.program = make_program(
		g_resources.vertex_shader,
		g_resources.fragment_shader);
	if (g_resources.program == 0) return 0;

	g_resources.uniforms.textures[0] =
		glGetUniformLocation(g_resources.program, "textures[0]");

	g_resources.uniforms.lut =
		glGetUniformLocation(g_resources.program, "lut");

	g_resources.attributes.position =
		glGetAttribLocation(g_resources.program, "position");

	g_resources.uniforms.offset =
		glGetUniformLocation(g_resources.program, "offset");
	
	return 1;
}

struct timeval start, end;
int numFrames = 0;

static void render(void)
{
/*	if (numFrames++ == 100) {
		gettimeofday(&end, NULL);
		printf("FPS: %f\n", 1000000.0 * 100.0 / ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) ) ;
		numFrames = 0;
	}
	if (numFrames == 0) {
		gettimeofday(&start, NULL);
	}
*/
	glUseProgram(g_resources.program);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, g_resources.lut);
	glUniform1i(g_resources.uniforms.lut, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_resources.textures[0]);
	glUniform1i(g_resources.uniforms.textures[0], 0);

	g_resources.offset = ((float) t) / T;
	glUniform1f(g_resources.uniforms.offset, g_resources.offset);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, T, N, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	
	glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
	glVertexAttribPointer(
		g_resources.attributes.position,
		2, GL_FLOAT, GL_FALSE,
		sizeof(GLfloat)*2, (void*) 0);
	glEnableVertexAttribArray(g_resources.attributes.position);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
	glDrawElements(
		GL_TRIANGLE_STRIP,
		4,
		GL_UNSIGNED_SHORT,
		(void*) 0);

	glDisableVertexAttribArray(g_resources.attributes.position);

	glutSwapBuffers();
}

static GLuint make_buffer(
		GLenum target,
		const void *buffer_data,
		GLsizei buffer_size ) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(target, buffer);
	glBufferData(target, buffer_size, buffer_data, GL_STREAM_DRAW);

	return buffer;
}

static GLuint make_texture(GLbyte* buf) {
	GLuint texture;
	int width = T, height = N;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		GL_RGB8, width, height, 0,
		GL_BGR, GL_UNSIGNED_BYTE, buf);

	return texture;
}

static GLuint make_shader(GLenum type, const char *filename) {
	GLint length;
	GLchar *source = file_contents(filename, &length);
	GLuint shader;
	GLint shader_ok;
	if (!source) return 0;
	
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**) &source, &length);
	free(source);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
//	if (!shader_ok) {
//		fprintf(stderr, "Failed to compile %s:\n", filename);
		show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
//		glDeleteShader(shader);
//		return 0;
//	}

	return shader;
}

static void show_info_log(
		GLuint object,
		PFNGLGETSHADERIVPROC glGet__iv,
		PFNGLGETSHADERINFOLOGPROC glGet__InfoLog) {
	GLint log_length;
	char *log;

	glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	log = malloc(log_length);
	glGet__InfoLog(object, log_length, NULL, log);
	fprintf(stderr, "%s", log);
	free(log);
}

static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader) {
	GLint program_ok;
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
	if (!program_ok) {
		fprintf(stderr, "Failed to link shader program:\n");
		show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

void *file_contents(const char *filename, GLint *length) {
	FILE *f = fopen(filename, "r");
	void *buffer;

	if (!f) {
		fprintf(stderr, "Unable to open %s for reading\n", filename);
	}

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(*length+1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';

	return buffer;
}

static GLuint make_lut()
{
	unsigned char *lutData = malloc(1024);
	FILE *f;
	switch (rand_from(0,3)) {
	case 0:
		f = fopen("hotiron.bin", "rb");
		break;
	case 1:
		f = fopen("rainbow.bin", "rb");
		break;
	case 2:
		f = fopen("geographic.bin", "rb");
		break;
	case 3:
		f = fopen("ge.bin", "rb");
		break;
	}
	unsigned char c;
	int i = 0;
	while(i < 256) {
		c = (char) fgetc(f);
		lutData[i++*4] = c;
	}
	i = 0;
	while(i < 256) {
		c = (char) fgetc(f);
		lutData[i++*4+1] = c;
	}
	i = 0;
	while(i < 256) {
		c = (char) fgetc(f);
		lutData[i++*4+2] = c;
	}
	while (i < 256) {
		// alpha: 1
		lutData[i++*4+3] = 255;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_1D, texture);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, lutData);
	return texture;
}
