/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Shader.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 27/09/2016
\brief
  Contains the interface for the Shader class which is responsible for
  reading, compiling, linking, and activating shader programs.
*/
/*****************************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL\glew.h>

/*****************************************************************************/
/*!
\class Shader
\brief
  This is responsible for managing everything to do with shaders.
  Given the files that the vertex shader and fragment shader are located in, the
  shader class will compile, link, and use those shaders to create a shader
  program. That shader can then be managed with this object. Contact me if you 
  want to know about how to write and manipulate shaders.
*/
/*****************************************************************************/
class Shader
{
  public:
    Shader(const std::string & vertex_file, const std::string & fragment_file);
    bool Compiled();
    GLuint GetAttribLocation(const std::string & name);
    GLuint GetUniformLocation(const std::string & name);
    GLuint ID() const;
    virtual void Use() const;
    void Purge() const;
    virtual void EnableAttributes();
    virtual void DisableAttributes();
  protected:
    //! The ID of the program created after linking the shaders.
    GLuint _programID;
    //! Identifies whether the program successfully compiled or not.
    bool _compiled;
    //! The name of the vertex shader file.
    std::string _vertexFile;
    //! The name of the fragment shader file.
    std::string _fragmentFile;
  private:
    GLuint CompileShader(const std::string & filename, GLenum type) const;
    std::string ReadShaderFile(const std::string & shader_file) const;
    void CreateProgram(GLuint vshader, GLuint fshader);
};

#endif // SHADER_H
