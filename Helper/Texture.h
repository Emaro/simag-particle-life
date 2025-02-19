#pragma once

#include "Image.h"

class Texture
{
	int m_width;
	int m_height;

	unsigned int m_texId;

	void create(Image const& image);
	void destroy();

public:

	Texture(Image const& image);
	virtual ~Texture();

	int width() const { return m_width; }
	int height() const { return m_height; }

	void update(Image const& image);

	void bind() const;
	void unbind() const;

};

