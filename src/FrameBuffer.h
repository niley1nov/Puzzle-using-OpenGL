#pragma once

#include"Renderer.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
public:
	unsigned int TexColor, RenderBuffer;
	FrameBuffer(const unsigned int& width,const unsigned int& height,unsigned int attachNum)
	{
		GLCall(glGenFramebuffers(1, &m_RendererID));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

		GLCall(glGenTextures(1, &TexColor));
		GLCall(glBindTexture(GL_TEXTURE_2D, TexColor));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachNum, GL_TEXTURE_2D, TexColor, 0));
		
		GLCall(glGenRenderbuffers(1, &RenderBuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));

		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	}
	FrameBuffer(const unsigned int& width, const unsigned int& height, const unsigned int& samples, const int attachNum)
	{
		GLCall(glGenFramebuffers(1, &m_RendererID));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
		
		GLCall(glGenTextures(1, &TexColor));
		GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TexColor));
		GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE));
		
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachNum, GL_TEXTURE_2D_MULTISAMPLE, TexColor, NULL));
		
		GLCall(glGenRenderbuffers(1, &RenderBuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer));
		GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height));
		
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer));
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	~FrameBuffer()
	{
		GLCall(glDeleteFramebuffers(1, &m_RendererID));
	}
	unsigned int GetID()
	{
		return m_RendererID;
	}
	void Bind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
	}
	void Unbind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	void BindTex(unsigned int num = 0)
	{
		glActiveTexture(GL_TEXTURE0 + num);
		glBindTexture(GL_TEXTURE_2D, TexColor);
	}
	void UnbindTex()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
