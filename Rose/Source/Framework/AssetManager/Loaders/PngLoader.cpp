#include "PngLoader.hpp"

#include <stb_image.h>

namespace Rose::Framework::Internal
{
  std::shared_ptr<IResource> PngLoader::LoadResource(_In_ const std::string& Path)
  {
    if (Path.empty())
    {
      // CreateTexture(Default)

      return false;
    }

    int Width = 1;
    int Height = 1;
    int Chanels = 3;

    unsigned char* pData = stbi_load(Path.c_str(), &Width, &Height, &Chanels, 4);

    // std::shared_ptr<ITexture> pTexture = CreateTexture(pData);

    stbi_image_free(pData);
    // return pTexture;
  }

  const std::vector<uint32_t> Default(_In_ const uint16_t Resolution)
  {
    uint16_t Width = Resolution;
    uint16_t Height = Resolution;
    uint16_t Tile = Resolution / 8;
    
    for (uint16_t y = 0; y < Height; y += 1)
    {
      for (uint16_t x = 0; x < Width; x += 1)
      {
        bool White = ((x / Tile) ^ (y / Tile)) & 1U;


      }
    }
  }
}