#include "framebuffer.h"

namespace Fondue {	namespace graphics {

	FrameBuffer::FrameBuffer()
	{
		glGenFramebuffers(1, &mBufferID);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteBuffers(1, &mBufferID);
	}

    void FrameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
    }

    void FrameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint FrameBuffer::attachColorBuffer(unsigned int width, unsigned int height, GLuint internalFormat, GLuint format, GLuint type, bool clamp)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

		GLuint colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if(clamp) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + mAttachments.size(), GL_TEXTURE_2D, colorBufferID, 0);

        mAttachments.push_back(GL_COLOR_ATTACHMENT0 + mAttachments.size());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return colorBufferID;
	}

    GLuint FrameBuffer::attachDepthBuffer(unsigned int width, unsigned int height, GLuint type, bool clamp)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

        GLuint depthMapID;
        glGenTextures(1, &depthMapID);



        glBindTexture(GL_TEXTURE_2D, depthMapID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if (clamp) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapID, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return depthMapID;
    }

    GLuint FrameBuffer::attachDepthRenderBuffer(unsigned int width, unsigned int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

        GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return rbo;
	}

	void FrameBuffer::initAttachedBuffers()
	{
        glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
        glDrawBuffers(mAttachments.size(), mAttachments.data());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::checkCompleteness()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	void FrameBuffer::bindColorBuffers()
	{
        for (int i = 0; i < mAttachments.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mAttachments[i]);
		}
	}

} }
