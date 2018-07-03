#pragma once

//#include <GL/glew.h>
#include <glad/glad.h>

namespace Fondue { namespace graphics {

		class IndexBuffer
		{
		private:

			GLuint mBufferID;
			GLuint mNumIndicies;

		public:

			IndexBuffer(GLushort* data, GLsizei count);
			~IndexBuffer();

			void bind() const;
			void unbind() const;

			inline GLuint getNumIndices() const { return mNumIndicies; }

		};

}}
