#pragma once

//#include <GL/glew.h>
#include <glad/glad.h>

namespace Fondue { namespace graphics {

		class Buffer
		{
		private:
			GLuint mBufferID;
			GLuint mNumComponents;
		public:
			Buffer(GLfloat* data, GLsizei count, GLint numComponents);
			~Buffer();

			void bind() const;
			void unbind() const;

			inline GLuint getNumComponents() const { return mNumComponents; }
		};

}}
