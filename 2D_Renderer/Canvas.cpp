#include "Canvas.h"
#include "Image.h"

SDL_Color PixelToColor(const pixel_t& pixel)
{
    SDL_Color color;
    color.r = pixel >> 24;
    color.g = (pixel >> 16) & 0xff;
    color.b = (pixel >> 8) & 0xff;
    color.a = pixel & 0xff;

    return color;

}

pixel_t ColorToPixel(const SDL_Color& color)
{
    return (color.r << 24 | color.g << 16 | color.b << 8 | color.a);
}

Canvas::Canvas(SDL_Renderer* renderer, int width, int height)
{
    m_width = width;
    m_height = height;

    m_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        width, height);

    m_buffer.resize(width * height);
}

Canvas::~Canvas()
{
    SDL_DestroyTexture(m_texture);
}

void Canvas::Update()
{
    SDL_UpdateTexture(m_texture, nullptr, m_buffer.data(), m_width * sizeof(pixel_t));
}

void Canvas::Draw(SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer, m_texture, nullptr, nullptr);
}

void Canvas::Clear(const SDL_Color& color)
{
    std::fill(m_buffer.begin(), m_buffer.end(), (color.r << 24 | color.g << 16 | color.b << 8 | color.a));
}

void Canvas::DrawPoint(const SDL_Point& point, const SDL_Color& color)
{
    if (point.x < 0 || point.x >= m_width || point.y < 0 || point.y >= m_height) return;

    m_buffer[point.x + (point.y * m_width)] = (color.r << 24 | color.g << 16 | color.b << 8 | color.a);
}

void Canvas::DrawLine(const SDL_Point& v1, const SDL_Point& v2, const SDL_Color& color)
{
    SDL_Point p1 = v1;
    SDL_Point p2 = v2;

    int dx = p2.x - p1.x; //Run
    int dy = p2.y - p1.y; //Rise
    if (dx == 0)
    {
        if (p1.y > p2.y) std::swap(p1.y, p2.y);
        for (int y = p1.y; y < p2.y; y++) 
        {
            DrawPoint({ p1.x, y }, color);
        }
    }
    else
    {
        float m = dy / static_cast<float>(dx);
        float b = p1.y - (m * p1.x);

        if (m <= 1.0f && m >= -1.0f)
        {
            if (p1.x > p2.x) std::swap(p1.x, p2.x);
            for (int x = p1.x; x < p2.x; x++)
            {
                int y = static_cast<int>(round((m * x) + b));
                DrawPoint({ x, y }, color);
            }
        }
        else
        {
            if (p1.y > p2.y) std::swap(p1.y, p2.y);
            for (int y = p1.y; y < p2.y; y++)
            {
                int x = static_cast<int>(round((y-b)/m));
                DrawPoint({ x, y }, color);
            }
        }

    }


}

void Canvas::DrawCircle(const SDL_Point& center, int radius, const SDL_Color& color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * (radius);
    DrawCirclePoint(center, { x,y }, color);
    while (y >= x)
    {
        x++;
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }
        DrawCirclePoint(center, { x, y }, color);
    }

}

void Canvas::DrawRectangle(const SDL_Rect& rect, const SDL_Color& color)
{
    for (int y = rect.y ; y < rect.h ; y++)
    {
        for (int x = rect.x ; x < rect.w ; x++)
        {
            DrawPoint({ x , y }, color);
        }
    }

}

void Canvas::DrawTriangle(const SDL_Point& p1, const SDL_Point& p2, const SDL_Point& p3, const SDL_Color& color)
{
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p1, color);
}

void Canvas::DrawLineList(const std::vector<SDL_Point> points, const SDL_Color& color)
{
    for (int i = 0; i < points.size()-1; i++)
    {
        DrawLine(points[i], points[(i + 1)], color);
    }
}

void Canvas::DrawImage(const Image* image, const SDL_Point& point)
{
    for (int y = 0; y < image->m_height; y++)
    {
        int sy = point.y + y;
        for (int x = 0; x < image->m_width; x++)
        {
            int sx = point.x + x;
            if (sx < 0 || sx >= m_width || sy < 0 || sy >= m_height) continue;



            m_buffer[sx + (sy * m_width)] = image->m_buffer[x + (y * image->m_width)];
        }
    }
}



void Canvas::ProcessInvert()
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);

        color.r = 255 - color.r;
        color.g = 255 - color.g;
        color.b = 255 - color.b;

        pixel = ColorToPixel(color);
    }
}

void Canvas::ProcessMono()
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);

        int sum = (color.r + color.g + color.b)/3;

        SDL_Color c = { sum, sum, sum };
        pixel = ColorToPixel(c);
    }
}

void Canvas::ProcessNight()
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);

        int sum = (color.r + color.g + color.b) / 3;

        SDL_Color c = { 0, sum, 0 };
        pixel = ColorToPixel(c);
    }
}

void Canvas::ProcessThreshhold(const SDL_Color& threshold)
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);
        
        if (color.r >= threshold.r)
        {
            color.r = color.r;
        }
        else
        {
            color.r = 0;
        }
        if (color.g >= threshold.g)
        {
            color.g = color.g;
        }
        else
        {
            color.g = 0;
        }if (color.b >= threshold.b)
        {
            color.b = color.b;
        }
        else
        {
            color.b = 0;
        }

        pixel = ColorToPixel(color);
    }
}

void Canvas::ProcessBrightness(int brightness)
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);

        color.r = std::min(std::max((int)color.r + brightness, 0), 255);
        color.g = std::min(std::max((int)color.g + brightness, 0), 255);
        color.b = std::min(std::max((int)color.b + brightness, 0), 255);
        

        pixel = ColorToPixel(color);
    }
}

void Canvas::ProcessColor(int r, int g, int b)
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);

        color.r = std::min(std::max((int)color.r + r, 0), 255);
        color.g = std::min(std::max((int)color.g + g, 0), 255);
        color.b = std::min(std::max((int)color.b + b, 0), 255);


        pixel = ColorToPixel(color);
    }
}

void Canvas::ProcessRBSwap()
{
    for (pixel_t& pixel : m_buffer)
    {
        SDL_Color color = PixelToColor(pixel);

        color.r = 255 - color.b;
        color.g = 255 - color.g;
        color.b = 255 - color.r;

        pixel = ColorToPixel(color);
    }
}

void Canvas::DrawCirclePoint(const SDL_Point& center, const SDL_Point& point, const SDL_Color& color)
{
    DrawPoint({ center.x + point.x, center.y + point.y }, color);
    DrawPoint({ center.x + point.x, center.y - point.y }, color);
    DrawPoint({ center.x - point.x, center.y + point.y }, color);
    DrawPoint({ center.x - point.x, center.y - point.y }, color);

    DrawPoint({ center.x + point.y, center.y + point.x }, color);
    DrawPoint({ center.x + point.y, center.y - point.x }, color);
    DrawPoint({ center.x - point.y, center.y + point.x }, color);
    DrawPoint({ center.x - point.y, center.y - point.x }, color);
}
