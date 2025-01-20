# 3D Engine
 This is a *test* code for a 3D Game Engine in C++

> This will create a window that will let you visualize & modify your game in 3D.

![](https://github.com/Loris-Moreau/3D-Engine-CPP/blob/test-23/Assets/2024-10-24%2012-04-56.gif "Showcase")


### Controls : 

- ZQSD for movement
- G to unlock the mouse
- F to fullscreen
- O & P for the point light radius


## How It Works  
#### Game System

* Entity System
  * Object-Oriented
  * Dynamic Creation and Release of custom Entities through templating features: createEntity<MyEntity>()
  * Event-based Logic support: onCreate(), onUpdate(float deltaTime), ...
  * Default Entity classes available out-of-the-box: LightEntity,MeshEntity,CameraEntity,...

#### Graphics Engine

* Graphics APIs: DirectX 11
* Forward Rendering
* Phong-Based Lighting
* Static Meshes Support: .obj (all formats supported by tinyobjloader)
* Textures Support: .jpg,.png,.bmp,.tga etc. (all formats supported by DirectXTex)
* Materials Support: HLSL Shading Language, Textures, Custom Data Passage

#### Input Manager

* Mouse Input Events Support
* Keyboard Input Events Support

#### Windowing System

* Native windowing and event support under Windows.
