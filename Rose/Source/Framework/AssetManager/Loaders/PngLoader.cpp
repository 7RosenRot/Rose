#include "PngLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"

namespace Rose::Framework::Internal
{
  std::shared_ptr<IResource> PngLoader::LoadResource(_In_ const std::string& Path)
  {
    if (Path.empty())
    {
      // LOGGER expected

      return nullptr;
    }

    int Width = 0;
    int Height = 0;
    int Chanels = 3;

    unsigned char* pData = stbi_load(Path.c_str(), &Width, &Height, &Chanels, STBI_rgb_alpha);

    if (!pData)
    {
      // LOGGER expected

      return nullptr;
    }

    std::shared_ptr<ITexture> pTexture = m_pRenderer->CreateTexture(Width, Height, pData);

    stbi_image_free(pData);
    
    return pTexture;
  }

  const std::vector<uint32_t> Default(_In_ const uint16_t Resolution)
  {
    uint16_t Tile = Resolution / 8;

    std::vector<uint32_t> Default(Resolution * Resolution);
    
    for (uint16_t y = 0; y < Resolution; y += 1)
    {
      for (uint16_t x = 0; x < Resolution; x += 1)
      {
        bool White = ((x / Tile) ^ (y / Tile)) & 1U;

        Default[y * Resolution + x] = White ? 0xFFFFFFFF : 0xFF000000;
      }
    }

    return Default;
  }
}