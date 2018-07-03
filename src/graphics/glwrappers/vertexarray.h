#pragma once

#include <glad/glad.h>
#include "buffer.h"
#include <vector>

namespace Fondue { namespace graphics {

	class VertexArray
	{
	private:
		GLuint mArrayID;
		std::vector<Buffer*> mBuffers;
	public:
		VertexArray();
		~VertexArray();

		void addBuffers(Buffer* buffer, GLuint index);
		void bind() const;
		void unbind() const;
	};

}}
