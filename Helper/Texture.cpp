#include "Texture.h"

#include <glad/gl.h>

Texture::Texture(Image const& image)
{
	create(image);
}

Texture::~Texture()
{
	destroy();
}

void Texture::create(Image const& image)
{
	m_width = image.width();
	m_height = image.height();

	glGenTextures(1, &m_texId);
	
	bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, image.data());
	unbind();
}

void Texture::update(Image const& image)
{
	if ((m_width != image.width()) || (m_height != image.height()))
	{
		destroy();
		create(image);
		return;
	}

	bind();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, image.data());
	unbind();
}

void Texture::destroy()
{
	glDeleteTextures(1, &m_texId);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_texId);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
