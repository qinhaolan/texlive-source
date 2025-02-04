// shader handling
// Author: Supakorn "Jamie" Rassameemasmuang

#include "common.h"

#ifdef HAVE_GL

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

#include "shaders.h"

GLuint compileAndLinkShader(std::vector<ShaderfileModePair> const& shaders, 
                            size_t NLights, size_t NMaterials,
                            std::vector<std::string> const& defineflags)
{
  GLuint mainShader = glCreateProgram();
  std::vector<GLuint> compiledShaders;

  for (auto const& shaderInfo : shaders) {
    GLint newshader=createShaderFile(shaderInfo.first,shaderInfo.second,
                                     NLights,NMaterials,defineflags);
    glAttachShader(mainShader,newshader);
    compiledShaders.push_back(newshader);
  }

  glLinkProgram(mainShader);

  for (auto const& compiledShader : compiledShaders) {
    glDetachShader(mainShader,compiledShader);
    glDeleteShader(compiledShader);
  }

  return mainShader;
}

GLuint createShaders(GLchar const* src, int shaderType,
                     std::string const& filename)
{
  GLuint shader=glCreateShader(shaderType);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
    
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if(status != GL_TRUE) {
    GLint length; 

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    std::vector<GLchar> msg(length);

    glGetShaderInfoLog(shader, length, &length, msg.data());

    size_t n=msg.size();
    for(size_t i=0; i < n; ++i) {
      std::cerr << msg[i];
    }

    std::cerr << std::endl << "GL Compile error" << std::endl;
    std::cerr << src << std::endl;
    throw 1; 
  }
  return shader;
}

GLuint createShaderFile(std::string file, int shaderType, size_t Nlights,
                        size_t Nmaterials,
                        std::vector<std::string> const& defineflags)
{
  std::ifstream shaderFile;
  shaderFile.open(file.c_str());
  std::stringstream shaderSrc;

#ifdef __APPLE__
#define GLSL_VERSION "410"
#else
#define GLSL_VERSION "130"
#endif
  
  shaderSrc << "#version " << GLSL_VERSION << "\r\n";
  shaderSrc << "#extension GL_ARB_uniform_buffer_object : enable"
            << "\r\n";
  shaderSrc << "#extension GL_ARB_shading_language_packing : enable"
            << "\r\n";

  for (std::string const& flag : defineflags) {
    shaderSrc << "#define " << flag << "\r\n";
  }

  shaderSrc << "const int Nlights=" << Nlights << ";\r\n";
  shaderSrc << "const int Nmaterials=" << Nmaterials << ";\r\n";

  if(shaderFile) {
    shaderSrc << shaderFile.rdbuf();
    shaderFile.close();
  } else {
    throw 1;
  }

  return createShaders(shaderSrc.str().data(), shaderType, file);
}
#endif
