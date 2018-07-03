#pragma once

#include <vector>
#include <iostream>

//#include <GL/glew.h>
#include <glad/glad.h>

namespace Fondue { namespace graphics {

#define MAX_ATTACHMENTS 15    // 16 inc. 0

	class FrameBuffer
	{

    public:

        void bind();
        void unbind();

	private:

		GLuint mBufferID;
		std::vector<GLuint> mAttachments;

	public:

		FrameBuffer();
		~FrameBuffer();

        GLuint attachColorBuffer(unsigned int width, unsigned int height, GLuint internalFormat, GLuint format, GLuint type, bool clamp = false);
        GLuint attachDepthBuffer(unsigned int width, unsigned int height, GLuint type, bool clamp = true);
        GLuint attachDepthRenderBuffer(unsigned int width, unsigned int height);
		void initAttachedBuffers();
		void checkCompleteness();
		void bindColorBuffers();

        GLuint getBufferID()  { return mBufferID; }

	};

} }
