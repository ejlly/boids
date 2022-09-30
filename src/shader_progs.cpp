#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "shader_progs.hpp"


GLuint Program::loadShader(char const *shader_file_path, GLenum shaderType){
	GLuint shaderID = glCreateShader(shaderType);
	
	std::string shaderCodeString;
	std::ifstream shaderFile(shader_file_path, std::ios::in);
	if(shaderFile.is_open()){
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderCodeString = shaderStream.str();
		shaderFile.close();
	}
	else{
		std::string error;
		error = std::string("Impossible to open file : ") + shader_file_path + "\n";
		throw std::invalid_argument(error);
	}
	
	char const *shaderCode = shaderCodeString.c_str();

	std::cout << "Compiling shader : " << shader_file_path << std::endl;

	glShaderSource(shaderID, 1, &shaderCode, nullptr);
	glCompileShader(shaderID);

	GLint success = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(shaderID, logSize, &logSize, &errorLog[0]);

		std::cout << &errorLog[0] << std::endl;
		glDeleteShader(shaderID);
		throw std::runtime_error("Compile shader error");
	}

	return shaderID;
}

Program::~Program(){
	glDeleteProgram(programID);
}

void Program::use(){
	glUseProgram(programID);
}

//floats
void Program::uniformf(const char* name, GLfloat v0){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1f(nameLoc, v0);
}

void Program::uniformf(const char* name, GLfloat v0, GLfloat v1){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2f(nameLoc, v0, v1);
}

void Program::uniformf(const char* name, GLfloat v0, GLfloat v1, GLfloat v2){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3f(nameLoc, v0, v1, v2);
}

void Program::uniformf(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4f(nameLoc, v0, v1, v2, v3);
}

//ints
void Program::uniformi(const char* name, GLint v0){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1i(nameLoc, v0);
}

void Program::uniformi(const char* name, GLint v0, GLint v1){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2i(nameLoc, v0, v1);
}

void Program::uniformi(const char* name, GLint v0, GLint v1, GLint v2){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3i(nameLoc, v0, v1, v2);
}

void Program::uniformi(const char* name, GLint v0, GLint v1, GLint v2, GLint v3){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4i(nameLoc, v0, v1, v2, v3);
}

//uints
void Program::uniformui(const char* name, GLuint v0){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1ui(nameLoc, v0);
}

void Program::uniformui(const char* name, GLuint v0, GLuint v1){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2ui(nameLoc, v0, v1);
}

void Program::uniformui(const char* name, GLuint v0, GLuint v1, GLuint v2){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3ui(nameLoc, v0, v1, v2);
}

void Program::uniformui(const char* name, GLuint v0, GLuint v1, GLuint v2, GLuint v3){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4ui(nameLoc, v0, v1, v2, v3);
}

//tabs
void Program::uniform_1f(const char *name, GLsizei count, const GLfloat* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1fv(nameLoc, count, value);
}

void Program::uniform_2f(const char *name, GLsizei count, const GLfloat* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2fv(nameLoc, count, value);
}

void Program::uniform_3f(const char *name, GLsizei count, const GLfloat* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3fv(nameLoc, count, value);
}

void Program::uniform_4f(const char *name, GLsizei count, const GLfloat* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4fv(nameLoc, count, value);
}

void Program::uniform_1i(const char *name, GLsizei count, const GLint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1iv(nameLoc, count, value);
}

void Program::uniform_2i(const char *name, GLsizei count, const GLint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2iv(nameLoc, count, value);
}

void Program::uniform_3i(const char *name, GLsizei count, const GLint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3iv(nameLoc, count, value);
}

void Program::uniform_4i(const char *name, GLsizei count, const GLint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4iv(nameLoc, count, value);
}

void Program::uniform_1ui(const char *name, GLsizei count, const GLuint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1uiv(nameLoc, count, value);
}

void Program::uniform_2ui(const char *name, GLsizei count, const GLuint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2uiv(nameLoc, count, value);
}

void Program::uniform_3ui(const char *name, GLsizei count, const GLuint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3uiv(nameLoc, count, value);
}

void Program::uniform_4ui(const char *name, GLsizei count, const GLuint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4uiv(nameLoc, count, value);
}

//square matrix
void Program::uniform_2x2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix2fv(nameLoc, count, transpose, value);
}

void Program::uniform_3x3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix3fv(nameLoc, count, transpose, value);
}

void Program::uniform_4x4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix4fv(nameLoc, count, transpose, value);
}

//rectangular matrix
void Program::uniform_2x3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix2x3fv(nameLoc, count, transpose, value);
}

void Program::uniform_2x4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix2x4fv(nameLoc, count, transpose, value);
}

void Program::uniform_3x2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix3x2fv(nameLoc, count, transpose, value);
}

void Program::uniform_3x4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix3x4fv(nameLoc, count, transpose, value);
}

void Program::uniform_4x2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix4x2fv(nameLoc, count, transpose, value);
}

void Program::uniform_4x3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniformMatrix4x3fv(nameLoc, count, transpose, value);
}



DrawingProgram::DrawingProgram(char const *vs, char const *fs){
	GLuint vsID = loadShader(vs, GL_VERTEX_SHADER);
	GLuint fsID = loadShader(fs, GL_FRAGMENT_SHADER);

	std::cout << "Linking drawing program\n";

	programID = glCreateProgram();
	glAttachShader(programID, vsID);
	glAttachShader(programID, fsID);
	glLinkProgram(programID);


	GLint success = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<GLchar> errorLog(logSize);
		glGetProgramInfoLog(programID, logSize, &logSize, &errorLog[0]);

		std::cout << &errorLog[0] << std::endl;
		glDeleteProgram(programID);
		throw std::runtime_error("Compile shader error");
	}

	glDetachShader(programID, vsID);
	glDetachShader(programID, fsID);

	glDeleteShader(vsID);
	glDeleteShader(fsID);
}

ComputeProgram::ComputeProgram(char const *cs){
	GLuint csID = loadShader(cs, GL_COMPUTE_SHADER);

	std::cout << "Linking compute program\n";

	programID = glCreateProgram();
	glAttachShader(programID, csID);
	glLinkProgram(programID);


	GLint success = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<GLchar> errorLog(logSize);
		glGetProgramInfoLog(programID, logSize, &logSize, &errorLog[0]);

		std::cout << &errorLog[0] << std::endl;
		glDeleteProgram(programID);
		throw std::runtime_error("Compile shader error");
	}

	glDetachShader(programID, csID);
	glDeleteShader(csID);
}
