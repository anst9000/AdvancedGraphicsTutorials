#include "GLSLProgram.h"
#include "BengineErrors.h"

#include <fstream>
#include <vector>
#include <algorithm>

namespace Bengine
{

	GLSLProgram::GLSLProgram()
	{
		// Empty
	}

	GLSLProgram::~GLSLProgram()
	{
		// Empty
	}

	void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
	{
		// Vertex and fragment shaders are successfully compiled
		// Link the together into a program, get a program object.
		m_programID = glCreateProgram();

		m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (m_vertexShaderID == 0)
		{
			fatalError("Vertex shader failed to be created.");
		}

		m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (m_fragmentShaderID == 0)
		{
			fatalError("Fragment shader failed to be created.");
		}

		compileShader(vertexShaderFilePath, m_vertexShaderID);
		compileShader(fragmentShaderFilePath, m_fragmentShaderID);
	}

	void GLSLProgram::linkShaders()
	{
		// Attach our shaders to our program
		glAttachShader(m_programID, m_vertexShaderID);
		glAttachShader(m_programID, m_fragmentShaderID);

		// Link our program
		glLinkProgram(m_programID);

		// Note the different functions here: glGetProgram* instead of glGetShader*
		GLint isLinked = 0;
		glGetProgramiv(m_programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(m_programID, maxLength, &maxLength, &errorLog[0]);

			// We don't need the program anymore
			glDeleteProgram(m_programID);
			// Don't leak shaders either
			glDeleteShader(m_vertexShaderID);
			glDeleteShader(m_fragmentShaderID);

			std::printf("%s\n", &(errorLog[0]));
			fatalError("Shaders failed to link");
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_programID, m_vertexShaderID);
		glDetachShader(m_programID, m_fragmentShaderID);

		glDeleteShader(m_vertexShaderID);
		glDeleteShader(m_fragmentShaderID);
	}

	// Adds an attribute to our shader. Should be called between compiling and linking.
	void GLSLProgram::addAttribute(const std::string& attributeName)
	{
		glBindAttribLocation(m_programID, m_numAttributes++, attributeName.c_str());
	}

	GLint GLSLProgram::getUniformLocation(const std::string uniformName)
	{
		GLint location = glGetUniformLocation(m_programID, uniformName.c_str());

		if (location == GL_INVALID_INDEX)
		{
			fatalError("Uniform " + uniformName + " not found in shader");
		}

		return location;
	}

	// Enable the shader, and all its attributes
	void GLSLProgram::use()
	{
		glUseProgram(m_programID);

		for (int i = 0; i < m_numAttributes; i++)
		{
			glEnableVertexAttribArray(i);
		}
	}

	// Disable the shader
	void GLSLProgram::unuse()
	{
		glUseProgram(0);

		for (int i = 0; i < m_numAttributes; i++)
		{
			glDisableVertexAttribArray(i);
		}
	}

	// Compiles a single shader file
	void GLSLProgram::compileShader(const std::string& filePath, GLuint id)
	{
		std::ifstream shaderFile(filePath);
		if (shaderFile.fail())
		{
			perror(filePath.c_str());
			fatalError("Failed to open " + filePath);
		}

		std::string fileContents;
		std::string line;

		while (std::getline(shaderFile, line))
		{
			fileContents += line + "\n";
		}

		shaderFile.close();

		// Get a pointer to our file contents c string
		const char* contentsPtr = fileContents.c_str();
		// Tell OpenGL that we want to use fileContents as the contents of the shader file
		glShaderSource(id, 1, &contentsPtr, nullptr);

		// Compile the shader
		glCompileShader(id);

		// Check for errors
		GLint success = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure
			glDeleteShader(id);	// Don't leak the shader

			std::printf("%s\n", &(errorLog[0]));
			fatalError("Shader " + filePath + " failed to compile");
		}
	}

}