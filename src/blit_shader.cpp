#include "blit_shader.h"

const char *const blit_vertex_shader =
		"#version 330\n"
		"\n"
		"layout (location=0) in vec2 coords;"
		"out vec2 T;"
		"\n"
		"void main(void)\n"
		"{\n"
		"T = coords;\n"
		"gl_Position = vec4((coords * 2.0) - 1.0, 0.0, 1.0);\n"
		"}";

const char *const blit_fragment_shader =
		"#version 330\n"
		"\n"
		"uniform sampler2D hdr_texture;\n"
		"\n"
		"in vec2 T;\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"color = texture(hdr_texture, T);\n"
		"}";

float blit_vertice_data[12] = {
	0.0, 1.0,
	1.0, 0.0,
	0.0, 0.0,
	0.0, 1.0,
	1.0, 1.0,
	1.0, 0.0
};

GLuint blit_shader_program = 0;
GLuint blit_shader_vao = 0;
GLuint blit_shader_vbo = 0;

void blit_shader_compile_shader_src(GLuint shader, const char *src) {
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint status;
	GLint length;
	char log[4096] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(shader, 4096, &length, log);
	if (status == GL_FALSE) {
		printf("Compile failed %s\n", log);
	};
};

void blit_shader_link_shader() {
	printf("Compiling blit shader\n");

	// Create the handels
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	blit_shader_program = glCreateProgram();

	// Attach the shaders to a program handel.
	glAttachShader(blit_shader_program, vertexShader);
	glAttachShader(blit_shader_program, fragmentShader);

	// Load and compile the Vertex Shader
	blit_shader_compile_shader_src(vertexShader, blit_vertex_shader);

	// Load and compile the Fragment Shader
	blit_shader_compile_shader_src(fragmentShader, blit_fragment_shader);

	printf("Linking blit shaders\n");
	glLinkProgram(blit_shader_program);

	// The shader objects are not needed any more,
	// the shader_program is the complete shader to be used.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint status;
	GLint length;
	char log[4096] = { 0 };

	glGetProgramiv(blit_shader_program, GL_LINK_STATUS, &status);
	glGetProgramInfoLog(blit_shader_program, 4096, &length, log);
	if (status == GL_FALSE) {
		printf("Link failed %s\n", log);

		glDeleteProgram(blit_shader_program);
		blit_shader_program = 0;
		return;
	};

	// and set some properties that never change
	glUniform1i(glGetUniformLocation(blit_shader_program, "hdr_texture"), 0);
	glUseProgram(0);
};

void blit_render(GLuint p_texture_id) {
	GLuint was_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&was_program);

	if (blit_shader_program != 0) {
		// set our shader up
		glUseProgram(blit_shader_program);

		// set our texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_texture_id);

		// bind our vao to restore our state
		glBindVertexArray(blit_shader_vao);

		// render our rectangle
		glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

		// and unbind
		glBindVertexArray(0);
		glUseProgram(was_program);
	}
};

void blit_shader_init() {
	// Create our shader program
	blit_shader_link_shader();

	// Need a Vertex Array Object
	glGenVertexArrays(1, &blit_shader_vao);

	// Bind our VAO, all relevant state changes are bound to our VAO, will be unset when we unbind, and reset when we bind...
	glBindVertexArray(blit_shader_vao);

	// Need a Vertex Buffer Object
	glGenBuffers(1, &blit_shader_vbo);

	// Now bind our Vertex Buffer Object and load up some data!
	glBindBuffer(GL_ARRAY_BUFFER, blit_shader_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, blit_vertice_data, GL_STATIC_DRAW);

	// And setup our attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid *)0);

	// and unbind our vao to return back to our old state
	glBindVertexArray(0);
};

void blit_shader_cleanup() {
	if (blit_shader_program != 0) {
		glDeleteProgram(blit_shader_program);
		blit_shader_program = 0;
	};

	glDeleteVertexArrays(1, &blit_shader_vao);
	glDeleteBuffers(1, &blit_shader_vbo);
};

