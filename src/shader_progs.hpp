#ifndef MY_SHADER_PROGS_OPENGL
#define MY_SHADER_PROGS_OPENGL

#define GLEW_STATIC
#include <GL/glew.h>

class Program{
	protected:
		GLuint programID;
		GLuint loadShader(char const *shader_file_path, GLenum shaderType);

	public:
		void use();
		//floats
		void uniform(const char *name, GLfloat v0);
		void uniform(const char *name, GLfloat v0, GLfloat v1);
		void uniform(const char *name, GLfloat v0, GLfloat v1, GLfloat v2);
		void uniform(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		//ints
		void uniform(const char *name, GLint v0);
		void uniform(const char *name, GLint v0, GLint v1);
		void uniform(const char *name, GLint v0, GLint v1, GLint v2);
		void uniform(const char *name, GLint v0, GLint v1, GLint v2, GLint v3);
		//uints
		void uniform(const char *name, GLuint v0);
		void uniform(const char *name, GLuint v0, GLuint v1);
		void uniform(const char *name, GLuint v0, GLuint v1, GLuint v2);
		void uniform(const char *name, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
		//tabs
		void uniform(const char *name, int dim, GLsizei count, const GLfloat* value);
		void uniform(const char *name, int dim, GLsizei count, const GLint* value);
		void uniform(const char *name, int dim, GLsizei count, const GLuint* value);
		//square matrix
		void uniform(const char *name, int dim, GLsizei count, GLboolean transpose, const GLfloat *value);
		//rectangular matrix
		void uniform(const char *name, int d1, int d2, GLsizei count, GLboolean transpose, const GLfloat *value);

};

class DrawingProgram : public Program{
	public:
		DrawingProgram(char const *vs, char const *fs);
};

class ComputeProgram : public Program{
	public:
		ComputeProgram(char const *cs);
};

#endif

