#include "vertexarray.h"

namespace Fondue {	namespace graphics {

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &mArrayID);
	}

	VertexArray::~VertexArray()
	{
		for (int i = 0; i < mBuffers.size(); i++)
			delete mBuffers[i];

		glDeleteVertexArrays(1, &mArrayID);
	}

	void VertexArray::addBuffers(Buffer* buffer, GLuint index)
	{
		bind();
		buffer->bind();

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, buffer->getNumComponents(), GL_FLOAT, GL_FALSE, 0, 0);

		buffer->bind();
		unbind();

		mBuffers.push_back(buffer);
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(mArrayID);
	}

	void VertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

}}
