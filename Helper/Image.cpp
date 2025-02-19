#include "Image.h"

#include <iostream>

#include <stb_image.h>

Image::Image(std::string const& filename)
    : m_width(0)
    , m_height(0)
    , m_channels(3)
{

    int sx=0,sy=0,n=0;
    const int noOfComponents = 3;

    // replace noOfComponents=0 with '1'..'4' to force that many components per pixel
    // 'n' will always be the number that it would have been if you said 0

    unsigned char *data = stbi_load(filename.c_str(), &sx, &sy, &n, noOfComponents);

    if (data)
    {
        m_width = sx;
        m_height = sy;
        m_channels = n;
        
        m_data.resize((size_t)m_width * m_height);
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                int idxDst = (m_height - 1 - y) * m_width + x;
                int idxSrc = y * m_width + x;
                m_data[idxDst] = glm::vec3(
                    (float)data[idxSrc * 3 + 0] / 255.0f,
                    (float)data[idxSrc * 3 + 1] / 255.0f,
                    (float)data[idxSrc * 3 + 2] / 255.0f
                );
            }
        }

        stbi_image_free(data);
    }
}

void Image::print() const
{
    const int w = m_width;
    //const int c = m_channels;

    for (int c = 0; c < 3; c++)
    {
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < w; x++)
            {
                std::cerr << ((at(x, y)[c] < 0.5f) ? " " : "#");
            }
            std::cerr << "\n";
        }
        std::cerr << "=============================\n";
    }
}
