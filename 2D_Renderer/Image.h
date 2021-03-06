#pragma once
#include "Canvas.h"
#include <vector>

class Image
{
public:
	Image() {}
	~Image() {}

	bool Load(const std::string& filename);

	friend class Canvas;
private:
	std::vector<pixel_t> m_buffer;
	int m_width = 0;
	int m_height = 0;
};