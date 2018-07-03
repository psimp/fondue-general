#pragma once

#include "../utils/fileutils.h"
#include "../maths/maths.h"
#include "shader.const"

#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <map>
#include <type_traits>

namespace Fondue {	namespace graphics {

    template<typename T>
    class CountedType
    {
      public:
      static long int id()
      {
        static int v;
        return (long int)&v;
      }
    };

    static inline unsigned int uniformBindingLocation = 10;

    class EMPTY {};

    const static std::unordered_map<std::string, std::string> shader_constants
    {
            {"NUM_SAMPLES", std::to_string(NUM_SAMPLES)},
            {"MAX_VIEWSPACE_MESHES", std::to_string(MAX_VIEWSPACE_MESHES)},
            {"MAX_WORK_GROUP_SIZE", std::to_string(MAX_WORK_GROUP_SIZE)},
            {"TILE_SIZE", std::to_string(MAX_WORK_GROUP_SIZE)},
            {"SCREEN_WIDTH", std::to_string(SCREEN_WIDTH)},
            {"SCREEN_HEIGHT", std::to_string(SCREEN_HEIGHT)},
            {"MAX_GLOBAL_MESHES", std::to_string(MAX_GLOBAL_MESHES)},
            {"MAX_LIGHTS", std::to_string(MAX_LIGHTS)}
    };

    template<class T, class S>
	class Shader
	{

    public:

        T* InitUniformStruct;
        S* RunUniformStruct;

	public:

        Shader(const char* vertPath, const char* fragPath)
            : mVertPath(vertPath), mFragPath(fragPath)
        {
            mShaderID = load();
            if (!std::is_same<T, EMPTY>::value) { createInitUniformBlock(uniformBindingLocation); }
            if (!std::is_same<S, EMPTY>::value) { createRunUniformBlock(uniformBindingLocation); }
        }

        Shader(const char* computePath)
            : mComputePath(computePath)
        {
            mShaderID = loadCompute();
            if (!std::is_same<T, EMPTY>::value) { createInitUniformBlock(uniformBindingLocation); }
            if (!std::is_same<S, EMPTY>::value) { createRunUniformBlock(uniformBindingLocation); }
        }

        ~Shader()
        {
            glDeleteProgram(mShaderID);
        }

        void createInitUniformBlock(GLuint binding)
        {
            GLuint &bind = uniformIDs[CountedType<T>::id()];
            if (bind) { setUniformBlockBinding("InitUniforms", bind); return; }
            else bind = binding;
            setUniformBlockBinding("InitUniforms", bind);
            uniformBindingLocation++;

            glGenBuffers(1, &mInitUniformBlock);
            glBindBuffer(GL_UNIFORM_BUFFER, mInitUniformBlock);
            glBufferStorage(GL_UNIFORM_BUFFER, sizeof(T), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            glBindBufferBase(GL_UNIFORM_BUFFER, binding, mInitUniformBlock);

            InitUniformStruct = reinterpret_cast<T*>
                     (
                         glMapBufferRange(GL_UNIFORM_BUFFER, 0 , sizeof(T),
                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
                     );

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void createRunUniformBlock(GLuint binding)
        {
            GLuint &bind = uniformIDs[CountedType<S>::id()];
            if (bind) { setUniformBlockBinding("RunUniforms", bind); return; }
            else bind = binding;
            setUniformBlockBinding("RunUniforms", bind);
            uniformBindingLocation++;

            glGenBuffers(1, &mRunUniformBlock);
            glBindBuffer(GL_UNIFORM_BUFFER, mRunUniformBlock);
            glBufferStorage(GL_UNIFORM_BUFFER, sizeof(S), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRunUniformBlock);

            RunUniformStruct = reinterpret_cast<S*>
                     (
                         glMapBufferRange(GL_UNIFORM_BUFFER, 0 , sizeof(S),
                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
                     );

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void getConstants(std::string& shaderData)
        {
            std::size_t endlc; std::string endstr; std::string::size_type startline;
            size_t t0 = shaderData.find("//external\r\n");
            if( t0 != std::string::npos) { endstr = "\r\n"; endlc = 2; startline=t0+12; }
            else{
                size_t t1 = shaderData.find("//external\n"  );
                if( t1 != std::string::npos) { endstr = "\n"; endlc = 1; startline=t1+11;   }
            else{
                size_t t2 = shaderData.find("//external\r"  );
                if( t2 != std::string::npos) { endstr = "\r"; endlc = 1; startline=t2+11;   }
            } }
            std::size_t foundEnd = shaderData.find("//external end", startline);

            if (foundEnd == std::string::npos) return;

            while(startline < foundEnd)
            {
                std::size_t endline = shaderData.find(endstr, startline);
                std::string shaderConst = shaderData.substr(startline, endline - startline);
                std::string value = shader_constants.at(shaderConst);

                shaderData.replace(startline, endline - startline, "#define " + shaderConst + " " + value);

                startline = endline + 9 + endlc + value.size();
                foundEnd += 8 + value.size();
            }
        }

        void enable() const
        {
            glUseProgram(mShaderID);
        }

        void disable() const
        {
            glUseProgram(0);
        }

        void setUniformMat4(const GLchar* name, const maths::mat4& mat)
        {
            glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat.elements);
        }

        void setUniform1f(const GLchar* name, const float val)
        {
            glUniform1f(getUniformLocation(name), val);
        }

        void setUniform1fv(const GLchar* name, int count, float* val)
        {
            glUniform1fv(getUniformLocation(name), count,  val);
        }

        void setUniform1i(const GLchar* name, const int val)
        {
            glUniform1i(getUniformLocation(name), val);
        }

        void setUniform1u(const GLchar* name, const uint val)
        {
            glUniform1ui(getUniformLocation(name), val);
        }

        void setUniform1iv(const GLchar* name, int count, int* val)
        {
            glUniform1iv(getUniformLocation(name), count, val);
        }

        void setUniform2f(const GLchar* name, const maths::vec2& vect)
        {
            glUniform2f(getUniformLocation(name), vect.x, vect.y);
        }

        void setUniform3f(const GLchar* name, const maths::vec3& vect)
        {
            glUniform3f(getUniformLocation(name), vect.x, vect.y, vect.z);
        }

        void setUniform4f(const GLchar* name, const maths::vec4& vect)
        {
            glUniform4f(getUniformLocation(name), vect.x, vect.y, vect.z, vect.w);
        }

        void setUniformBlockBinding(const GLchar* name, int bindID)
        {
            unsigned int blockIndex = glGetUniformBlockIndex(mShaderID, name);
            glUniformBlockBinding(mShaderID, blockIndex, bindID);
        }

        void setTextureUnit(const GLchar* name, const GLuint tid, const GLuint binding)
        {
            setUniform1i(name, binding);
            glBindTextureUnit(binding, tid);
        }

        int getAttribLocation(const GLchar* name)
        {
            return glGetAttribLocation(mShaderID, name);
        }

    private:

        int getUniformLocation(const GLchar* name)
        {
            return glGetUniformLocation(mShaderID, name);
        }

        unsigned int load()
        {
            GLuint program = glCreateProgram();
            GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
            GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

            std::string vertSourceString = read_file(mVertPath);
            std::string fragSourceString = read_file(mFragPath);

            getConstants(vertSourceString);
            getConstants(fragSourceString);

            const char* vertSource = vertSourceString.c_str();
            const char* fragSource = fragSourceString.c_str();

            glShaderSource(vertex, 1, &vertSource, NULL);
            glCompileShader(vertex);
            GLint result;
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
            if (result == GL_FALSE)
            {
                GLint length;
                glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> error(length);
                glGetShaderInfoLog(vertex, length, &length, &error[0]);
                std::cout << "VERTEX SHADER COMPILATION ERROR: " << &error[0] << std::endl;
                glDeleteShader(vertex);
                return 0;
            }

            glShaderSource(fragment, 1, &fragSource, NULL);
            glCompileShader(fragment);
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
            if (result == GL_FALSE)
            {
                GLint length;
                glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> error(length);
                glGetShaderInfoLog(fragment, length, &length, &error[0]);
                std::cout << "FRAGMENT SHADER COMPILATION ERROR: " << &error[0] << std::endl;
                glDeleteShader(fragment);
                return 0;
            }

            glAttachShader(program, vertex);
            glAttachShader(program, fragment);

            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &result);
            if (result == GL_FALSE)
            {
                GLint length;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> error(length);
                glGetShaderInfoLog(program, length, &length, &error[0]);
                std::cout << "PROGRAM LINK ERROR: " << &error[0] << std::endl;
                return 0;
            }

//            glValidateProgram(program);
//            glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
//            if (result == GL_FALSE)
//            {
//                GLint length;
//                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
//                std::vector<char> error(length);
//                glGetShaderInfoLog(program, length, &length, &error[0]);
//                std::cout << "PROGRAM VALIDATION ERROR: " << &error[0] << std::endl;
//                return 0;
//            }

            glDeleteShader(vertex);
            glDeleteShader(fragment);

            return program;
        }

        unsigned int loadCompute()
        {
            GLuint program = glCreateProgram();
            GLuint compute = glCreateShader(GL_COMPUTE_SHADER);

            std::string compSourceString = read_file(mComputePath);
            getConstants(compSourceString);

            const char* compSource = compSourceString.c_str();

            glShaderSource(compute, 1, &compSource, NULL);
            glCompileShader(compute);
            GLint result;
            glGetShaderiv(compute, GL_COMPILE_STATUS, &result);
            if (result == GL_FALSE)
            {
                GLint length;
                glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> error(length);
                glGetShaderInfoLog(compute, length, &length, &error[0]);
                std::cout << "COMPUTE SHADER COMPILATION ERROR: " << &error[0] << std::endl;
                glDeleteShader(compute);
                return 0;
            }

            glAttachShader(program, compute);

            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &result);
            if (result == GL_FALSE)
            {
                GLint length;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> error(length);
                glGetShaderInfoLog(program, length, &length, &error[0]);
                std::cout << "PROGRAM LINK ERROR: " << &error[0] << std::endl;
                return 0;
            }

            glValidateProgram(program);
            glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
            if (result == GL_FALSE)
            {
                GLint length;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> error(length);
                glGetShaderInfoLog(program, length, &length, &error[0]);
                std::cout << "PROGRAM VALIDATION ERROR: " << &error[0] << std::endl;
                return 0;
            }

            glDeleteShader(compute);

            return program;
        }


    private:

        static inline std::map<long int, GLuint> uniformIDs; // For uniform id lookup pre shader compile-time

        GLuint mInitUniformBlock;
        GLuint mRunUniformBlock;
        GLushort mUniformBinding;

        GLuint mShaderID;

        const char * mVertPath, * mFragPath, * mComputePath;

	};

}}
