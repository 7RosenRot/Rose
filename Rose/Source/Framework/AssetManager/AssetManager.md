# AssetManager module

## AssetManger

### General Inforamtion

#### Назначение:
  - Организация загрузки, хранения и удаления ресурсов, и предоставления доступа к ним.
#### Интерфейс:
  - Работа модуля
    - *Инитиализация* `void Initialize(const std::shared_ptr<IRenderer>& pRenderer) {...}`
    - *Завершение* `void Shutdown() {...}`
  - Работа с лоадером
    - *Регистрация лоадера* `void RegistryLoader() {...}`
  - Работа с ресурсом
    - *Загрузка* `ResourceDesc UploadResource(const std::string& Path) {...}`
    - *Удаление* `void DeleteResource(ResourceDesc& Desc) {...}`
    - *Доступ* `T* LoadResource(ResourceDesc& Desc) {...}`

#### Architecture
```text
├──Framework/
│  ╰── AssetManager/
│      ├── AssetManager.hpp
│      ├── AssetManager.cpp
│      ├── Loaders/
│      │   ├── IResourceLoader.hpp
│      │   ├── ObjLoader.hpp
│      │   ╰── PngLoader.hpp
│      ╰── Resources/
│          ├── IResource.hpp
│          ├── IMesh.hpp
│          ╰── ITexture.hpp
│
├── Renderer/
│   ├── IRenderer.hpp
│   ├── ICommandContext.hpp
│   ╰── Backend/
│       ├── D3D12/
│       │   ├── D3D12Renderer.cpp
│       │   ├── D3D12Model.cpp
│       │   ├── D3D12Texture.cpp
│       │   ╰── RHI/
│       ╰── VK3D/
│
╰── main.cpp
```

#### Namespace
1. Пространство имен:
```C++
  namespace Rose::Framework::AssetManager {...}
```

### Storage
1. Хранение ресурсов внутри `AssetManger` 
```C++
  // ↓ Список поддерживаемых расширений ↓
  enum class ResourceType : uint32_t { Mesh, Texture, Audio, Animation, Unknown };
  
  // ↓ Описатель ресурса ↓
  struct ResourceDesc {
    ResourceType Type = ResourceType::Unknown;
    uint32_t Index = -1;
  };

  // ↓ Указатель на используемый рендерер D3D12/VK3D ↓
  std::shared_ptr<IRenderer> m_pRenderer;

  // ↓ Список реализованных загрузчиков ↓
  std::vector<std::shared_ptr<IResourceLoader>> m_pResourceLoaders;
  
  // ↓ Мапа для доступа через Path ↓
  std::unordered_map<std::string, ResourceDesc> m_ResourceMap;
  
  // ↓ Вектора когда-либо загруженных ассетов ↓
  std::vector<std::shared_ptr<IMesh>> m_pMeshes;
  std::vector<std::shared_ptr<ITextrue>> m_pTextures;

  // ↓ Очереди свободных индексов ↓
  std::queue<uint32_t> m_FreeMeshIndices;
  std::queue<uint32_t> m_FreeTextureIndices;
```

### Logic

#### *Подключение AssetManager*
```C++
  #include <AssetManager>

  std::shared_ptr<AssetManager> m_pAssetManager;
```

#### *Инициализация AssetManger*
```C++
  m_pAssetManger = std::make_shared<AssetManger>();

  m_pAssetManger->Initialize(m_pRenderer);
```

Внутри `void Initialize(const std::shared_ptr<IRenderer>& pRenderer) {...}`

```C++
  m_pRenderer = pRenderer;

  RegistryLoader<ObjLoader>();
  RegistryLoader<PngLoader>();
```

#### *Обработка вызова загрузки*
```C++
  m_pAssetManager->UploadResource("Path");
```

Внутри `ResourceDesc UploadResource(const std::string& Path) {...}`

1. Подготовка.
```C++
  // ↓ *Проверка реестра* ↓
  if (auto it = m_ResourceMap.find(Path); it != m_ResourceMap.end()) {
    return it->second;
  }

  // ↓ *Базовый дескриптор* ↓
  ResourceDesc Resource = {};

  // ↓ *Расширение файла* ↓
  std::string FileExtension = std::filesystem::path(Path).extension().string();
```
2. Обход загрузчиков и вызов нужного.
```C++
  // ↓ *Общий интерфейс* ↓
  std::shared_ptr<IResource> pResource = nullptr;
  
  // ↓ *Обход загрузчиков* ↓
  for (const auto& pLoader : m_pResourceLoaders) {
    if (pLoader->GetPattern() == FileExtension) {
      pResource = pLoader->LoadResource(Path);

      break;
    }
  }
  
  // ↓ *Сбой загрузки* ↓
  if (pResource == nullptr) {
    return Resource;
  }
```
3. Сохранение ресурса в реестр.
```C++
  // ↓ *Запись типа* ↓
  Resource.Type = pResource->GetType();
  
  switch (Resource.Type) {
    // ↓ *Тип: Меш* ↓
    case (ResourceType::Mesh): {
      // ↓ *Апкаст до нужного указателя* ↓
      std::shared_ptr<IMesh> pMesh = std::static_pointer_cast<IMesh>(pResource);

      uint32_t Index = 0;

      // ↓ *Запись в ранее очищенную ячейку* ↓
      if (!m_FreeMeshIndices.empty()) {
        // ↓ *Забираем индекс* ↓
        Index = m_FreeMeshIndices.front();
        m_FreeMeshIndices.pop();

        // ↓ *Пишем* ↓
        m_pMeshes[Index] = pMesh;
      }
      // ↓ *Запись в новую ячейку* ↓
      else {
        // ↓ *Запись в вектор* ↓
        m_pMeshes.push_back(pMesh);
  
        // ↓ *Запись индекса* ↓
        Index = static_cast<uint32_t>(m_pMeshes.size() - 1);
      }

      Resource.Index = Index;

      break;
    }

    // ↓ *Тип: Текстура* ↓
    case (ResourceType::Texture): {...}

    // ↓ *Тип: Анимация/Аудио/...* ↓
    case (...): {...}

    // ↓ *Вернуть пустой* ↓
    default: {
      return { ResourceType::Unknown, -1 };
    }
  }
  
  // ↓ *Запись в реестр* ↓
  m_ResourceMap.emplace(Path, Resource)
  
  return Resource;
```

#### *Обработка вызова доступа*
```C++
  m_pAssetManager->GetResource(Desc);
```

1. Метод должен быть шаблонным, так как возвращает один из типов ресурсов: `IMesh`, `ITexture`, `IAnimation`, ...

```C++
  template<typename T>
  T* GetResource(const ResourceDesc& Desc) {
    switch (Desc.Type) {
      case (ResourceType::Mesh): {
        // ↓ *Возврат указателя* ↓
        if (!(Desc.Index < m_pMeshes.size())) {
          assert(
            (typeid(T) == typeid(Internal::IMesh))
             &&
            ("Type mismatch! Requested Mesh, but template param is other")
          );
          
          return static_cast<T*>(m_pMeshes[Desc.Index].get());
        }

        break;
      }

      case (ResourceType::Texture): {...}

      case (...): {...}

      default: {
        return nullptr;
      }
    }

    return nullptr;
  }
```

#### *Обработка вызова удаления*
```C++
  m_pAssetManager->DeleteResource(Desc);
```

1. Необходимо осовободить память, удалить запись в реестре и сохранить индекс очищенной ячейки.

```C++
  void DeleteResource(const std::string& Path) {
    // ↓ *Проверить в реестр* ↓
    auto it = m_ResourceMap.find(Path);
    if (it == m_ResourceMap.end()) {
      return;
    }

    ResourceDesc Desc = it->second;

    switch (Desc.Type) {
      case (ResourceType::Mesh): {
        // ↓ *Освобождаем ОЗУ/VRAM* ↓
        m_pMeshes[Desc.Index].reset();

        // ↓ *Сохраняем индекс очищенной ячейки* ↓
        m_FreeMeshIndices.push(Desc.Index);

        break;
      }

      case (ResourceType::Texture): {...}

      case (...) {...}
    }

    m_ResourceMap.erase(it);
  }
```

## Resource Loader

### General Inforamtion

#### Назначение:
- Группа специализированных лоадеров, каждый из которых поддерживает один тип ресурса - *"Obj, Png, ..."*
#### Интерфейс:
  - Работа загрузчика:
    - Инициализация: `void Initialize(const IRenderer* pRenderer) {...}`
    - Завершение работы: `void Shutdown() {...}`
  - Работа с ресурсом:
    - Узнать подерживаемый тип: `const std::string& GetPattern() const {...}`
    - Загрузить поддерживаемый ресурс: `const std::shared_ptr<IResource> LoadResource(const std::string& Path) {...}`

#### Architecture
```text
Loaders/
├── IResourceLoader.hpp
├── ObjLoader.hpp
╰── PngLoader.hpp
```

#### Namespace
1. Пространство имен:
```C++
  namespace Rose::Framework::Internal {...}
```

### Base Class
1. Базовый класс `IResourceLoader`:
```C++
  class IResourceLoader
  {
   public:
    virtual IResourceLoader() = default;
    virtual ~IResourceLoader() = default;
    
    void Initialize(const Renderer::IRenderer* pRenderer)
    {
      m_pRenderer = pRenderer;
    }
   
    virtual std::string_view GetPattern() const = 0;
    virtual std::shared_ptr<IResource> LoadResource(const std::string& Path) = 0;
   
   protected:
    Renderer::IRednerer* m_pRenderer = nullptr;
  };
```

### Logic
1. Инициализация загрузчика
```C++
  void Initialize(Rose::Renderer::IRenderer* pRenderer)
  {
    m_pRenderer = pRenderer;
  }
```

## Resource Interface

### Information
#### Назначение:
#### Интерфейс:

#### Architecture
```text
Resources/
├── IResource.hpp
├── IMesh.hpp
╰── ITexture.hpp
```

1. Базовый класс ресурса `IResource`
```C++
  class IResource {
   public:
    virtual ~IResource() = default;
    
    // ↓ Метод для определения типа ресурса класса ↓
    virtual ResourceType GetType() const = 0;
  };
```