#pragma once

#include <GL/glew.h>
#include <string>

namespace Bengine
{
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();

		void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void linkShaders();
		void addAttribute(const std::string& attributeName);

		GLint getUniformLocation(const std::string uniformName);

		void use();
		void unuse();

	private:
		void compileShader(const std::string& filePath, GLuint id);

		int m_numAttributes = 0;
		GLuint m_programID = 0;

		GLuint m_vertexShaderID = 0;
		GLuint m_fragmentShaderID = 0;
	};
}
