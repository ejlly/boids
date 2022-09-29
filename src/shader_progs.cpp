#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "shader_progs.hpp"

void Program::use(){
	glUseProgram(programID);
}

//floats
void Program::uniform(const char* name, GLfloat v0){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1f(nameLoc, v0);
}

void Program::uniform(const char* name, GLfloat v0, GLfloat v1){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2f(nameLoc, v0, v1);
}

void Program::uniform(const char* name, GLfloat v0, GLfloat v1, GLfloat v2){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3f(nameLoc, v0, v1, v2);
}

void Program::uniform(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4f(nameLoc, v0, v1, v2, v3);
}

//ints
void Program::uniform(const char* name, GLint v0){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1i(nameLoc, v0);
}

void Program::uniform(const char* name, GLint v0, GLint v1){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2i(nameLoc, v0, v1);
}

void Program::uniform(const char* name, GLint v0, GLint v1, GLint v2){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3i(nameLoc, v0, v1, v2);
}

void Program::uniform(const char* name, GLint v0, GLint v1, GLint v2, GLint v3){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4i(nameLoc, v0, v1, v2, v3);
}

//uints
void Program::uniform(const char* name, GLuint v0){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform1ui(nameLoc, v0);
}

void Program::uniform(const char* name, GLuint v0, GLuint v1){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform2ui(nameLoc, v0, v1);
}

void Program::uniform(const char* name, GLuint v0, GLuint v1, GLuint v2){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform3ui(nameLoc, v0, v1, v2);
}

void Program::uniform(const char* name, GLuint v0, GLuint v1, GLuint v2, GLuint v3){
	GLint nameLoc = glGetUniformLocation(programID, name);

	glUniform4ui(nameLoc, v0, v1, v2, v3);
}

//tabs
void Program::uniform(const char *name, int dim, GLsizei count, const GLfloat* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	switch(dim){
		case 1:
			glUniform1fv(nameLoc, count, value);
			break;
		case 2:
			glUniform2fv(nameLoc, count, value);
			break;
		case 3:
			glUniform3fv(nameLoc, count, value);
			break;
		case 4:
			glUniform4fv(nameLoc, count, value);
			break;
		default:
			throw std::invalid_argument("Invalid dimension in uniform float tab (glUniform_fv)");
	}
}

void Program::uniform(const char *name, int dim, GLsizei count, const GLint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	switch(dim){
		case 1:
			glUniform1iv(nameLoc, count, value);
			break;
		case 2:
			glUniform2iv(nameLoc, count, value);
			break;
		case 3:
			glUniform3iv(nameLoc, count, value);
			break;
		case 4:
			glUniform4iv(nameLoc, count, value);
			break;
		default:
			throw std::invalid_argument("Invalid dimension in uniform int tab (glUniform_iv)");
	}
}

void Program::uniform(const char *name, int dim, GLsizei count, const GLuint* value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	switch(dim){
		case 1:
			glUniform1uiv(nameLoc, count, value);
			break;
		case 2:
			glUniform2uiv(nameLoc, count, value);
			break;
		case 3:
			glUniform3uiv(nameLoc, count, value);
			break;
		case 4:
			glUniform4uiv(nameLoc, count, value);
			break;
		default:
			throw std::invalid_argument("Invalid dimension in uniform int tab (glUniform_uiv)");
	}
}

//square matrix
void Program::uniform(const char *name, int dim, GLsizei count, GL_Boolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	switch(dim){
		case 2:
			glUniformMatrix2fv(nameLoc, count, transpose, value);
			break;
		case 3:
			glUniformMatrix3fv(nameLoc, count, transpose, value);
			break;
		case 4:
			glUniformMatrix4fv(nameLoc, count, transpose, value);
			break;
		default:
			throw std::invalid_argument("Invalid dimension in uniform square matrix (glUniformMatrix_fv)");
	}
}

//rectangular matrix
void Program::uniform(const char *name, int d1, int d2, GLsizei count, GL_Boolean transpose, const GLfloat *value){
	GLint nameLoc = glGetUniformLocation(programID, name);

	switch(d1){
		case 2:
			switch(d2){
				case(3):
					glUniformMatrix2x3fv(nameLoc, count, transpose, value);
					break;
				case(4):
					glUniformMatrix2x4fv(nameLoc, count, transpose, value);
					break;
				default:
					throw std::invalid_argument("Invalid dimension in uniform compi matrix (glUniformMatrix2x_fv)");
			}
			break;
		case 3:
			switch(d2){
				case(3):
					glUniformMatrix3x2fv(nameLoc, count, transpose, value);
					break;
				case(4):
					glUniformMatrix3x4fv(nameLoc, count, transpose, value);
					break;
				default:
					throw std::invalid_argument("Invalid dimension in uniform compi matrix (glUniformMatrix3x_fv)");
			}
			break;
		case 4:
			switch(d2){
				case(2):
					glUniformMatrix4x2fv(nameLoc, count, transpose, value);
					break;
				case(3):
					glUniformMatrix4x3fv(nameLoc, count, transpose, value);
					break;
				default:
					throw std::invalid_argument("Invalid dimension in uniform compi matrix (glUniformMatrix4x_fv)");
			}
			break;
		default:
			throw std::invalid_argument("Invalid dimension in uniform simple matrix (glUniformMatrix_x{2-3-4}fv)");
	}
}



DrawingProgram::loadShader(char const *shader_file_path, GLenum shaderType){
	GLuint shaderID = glCreateShader(shaderType);
	
	std::string shaderCodeString;
	std::ifstream shaderFile(shader_file_path, std::ios::in);
	if(shaderFile.is_open()){
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderCodeString = shaderStream.str();
		vsFile.close();
	}
	else{
		throw std::invalid_argument("Impossible to open file : %s\n", vs);
	}
	
	char const shaderCode[] = shaderCodeString.c_str();

	std::cout << "Compiling shader : " << shader_file_path << std::endl;

	glShaderSrouce(shaderID, 1, &shaderCode, nullptr);
	glCompileShader(shaderID);

	GLint success = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

		std::cout << errorLog << endl;
		glDeleteShader(shaderID);
		throw std::runtime_error("Compile shader error");
	}

	return shaderID;
}

DrawingProgram::initialize(char const *vs, char const *fs){
	GLuint vsID = loadShader(vs, GL_VERTEX_SHADER);
	GLuint fsID = loadShader(fs, GL_FRAGMENT_SHADER);

	std::cout << "Linking program\n";

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vsID);
	glAttachShader(programID, fsID);
	glLinkProgram(programID);


	GLint success = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if(success == GL_FALSE){
		GLint logSize = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<GLchar> errorLog(logSize);
		glGetProgramInfoLog(programID, maxLength, &maxLength, &errorLog[0]);

		std::cout << errorLog << endl;
		glDeleteProgram(programID);
		throw std::runtime_error("Compile shader error");
	}

	glDetachShader(programID, vsID);
	glDetachShader(programID, fsID);

	glDeleteShader(vsID);
	glDeleteShader(fsID);

	return programID;
}
