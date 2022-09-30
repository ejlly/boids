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
		~Program();
		//floats
		void uniformf(const char *name, GLfloat v0);
		void uniformf(const char *name, GLfloat v0, GLfloat v1);
		void uniformf(const char *name, GLfloat v0, GLfloat v1, GLfloat v2);
		void uniformf(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		//ints
		void uniformi(const char *name, GLint v0);
		void uniformi(const char *name, GLint v0, GLint v1);
		void uniformi(const char *name, GLint v0, GLint v1, GLint v2);
		void uniformi(const char *name, GLint v0, GLint v1, GLint v2, GLint v3);
		//uints
		void uniformui(const char *name, GLuint v0);
		void uniformui(const char *name, GLuint v0, GLuint v1);
		void uniformui(const char *name, GLuint v0, GLuint v1, GLuint v2);
		void uniformui(const char *name, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
		//tabs
		void uniform_1f(const char *name, GLsizei count, const GLfloat* value);
		void uniform_2f(const char *name, GLsizei count, const GLfloat* value);
		void uniform_3f(const char *name, GLsizei count, const GLfloat* value);
		void uniform_4f(const char *name, GLsizei count, const GLfloat* value);

		void uniform_1i(const char *name, GLsizei count, const GLint* value);
		void uniform_2i(const char *name, GLsizei count, const GLint* value);
		void uniform_3i(const char *name, GLsizei count, const GLint* value);
		void uniform_4i(const char *name, GLsizei count, const GLint* value);

		void uniform_1ui(const char *name, GLsizei count, const GLuint* value);
		void uniform_2ui(const char *name, GLsizei count, const GLuint* value);
		void uniform_3ui(const char *name, GLsizei count, const GLuint* value);
		void uniform_4ui(const char *name, GLsizei count, const GLuint* value);
		//square matrix
		void uniform_2x2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_3x3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_4x4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		//rectangular matrix
		void uniform_2x3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_2x4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_3x2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_3x4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_4x2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
		void uniform_4x3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);

};

class DrawingProgram : public Program{
	public:
		DrawingProgram(char const *vs, char const *fs);
};

class ComputeProgram : public Program{
	public:
		ComputeProgram(char const *cs);

		void compute(int g_x, int g_y, int g_z);
};

#endif
