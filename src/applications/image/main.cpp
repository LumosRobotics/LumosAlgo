// #include <duoplot/duoplot.h>

// #include <glm/glm.hpp>
#include <iostream>
#include <vector>
// #include <nlohmann/json.hpp>

#include "vo/vo.h"

#include "math/math.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Example function to load an image using stb_image
bool loadImage(const std::string &filename, std::vector<uint8_t> &imageData, int &width, int &height, int &channels)
{
  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
  if (!data)
  {
    std::cerr << "Failed to load image: " << filename << std::endl;
    return false;
  }
  imageData.assign(data, data + (width * height * channels));
  stbi_image_free(data);
  return true;
}

ImageRGB<uint8_t> createImageFromData(const std::vector<uint8_t> &data, int width, int height)
{
  ImageRGB<uint8_t> image(height, width);
  for (int r = 0; r < height; ++r)
  {
    for (int c = 0; c < width; ++c)
    {
      for (int ch = 0; ch < 3; ++ch) // Assuming RGB
      {
        image(r, c, ch) = data[(r * width + c) * 3 + ch];
      }
    }
  }
  return image;
}

using namespace lumos;

int main()
{
  std::vector<uint8_t> imageData;
  int width, height, channels;
  if (loadImage("/Users/danielpi/work/LumosAlgo/src/applications/simple/img.png", imageData, width, height, channels))
  {
    std::cout << "Image loaded successfully: " << width << "x" << height << " channels: " << channels << std::endl;
  }
  else
  {
    std::cerr << "Image loading failed." << std::endl;
  }
  return 0;
}
