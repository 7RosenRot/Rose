#include "Renderer/IRenderer.hpp"
#include "Renderer/D3D12/Backend/D3D12Renderer.hpp"
#include "Framework/AssetManager/AssetManager.hpp"

#include <iostream>

int WinMain()
{
  return 0;
}

int main(int argc, char** argv)
{
  std::shared_ptr<Rose::Renderer::IRenderer> pRenderer;
  pRenderer = std::make_shared<Rose::Renderer::D3D12::D3D12Renderer>();

  std::shared_ptr<Rose::Framework::AssetManager> pAssetManager;
  pAssetManager = std::make_shared<Rose::Framework::AssetManager>();

  pAssetManager->Initialize(pRenderer);

  Rose::Framework::Internal::RESOURCE_HANDLE Handle{};

  Handle = pAssetManager->UploadResource("C:/Microsoft Visual Studio/Visual Studio Code/Rose/MshkFrede.obj");

  if (Handle.GetType() != Rose::Framework::Internal::Type::Unknown)
  {
    std::cout << "Success: ID - " << Handle.GetID() << " / Type - ";
    if (Handle.GetType() == Rose::Framework::Internal::Type::Mesh)
    {
      std::cout << "Mesh";
    }
  }
  else
  {
    std::cout << "Failure";
  }
  
  pAssetManager->DeleteResource("C:/Microsoft Visual Studio/Visual Studio Code/Rose/MshkFrede.obj");

  pAssetManager->Shutdown();

  return 0;
}