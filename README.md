# 3D Engine
 This is a Remake of Descent in my 3D Engine in C++

![](https://github.com/Loris-Moreau/3D-Engine-CPP/blob/Descent-Game/Assets/Showcase/Descent-Game.gif "Showcase")


### Controls : 

- Esc = Unlock Mouse

- ZQSD = Movement
  - QD = Strafe
- Space = Slide Up
- Ctrl = Slide Down

- A = roll left
- E = roll right

- Mouse = Pitch Up/Down & Yaw left/right

- LClick = Shoot Primary (Lasers)
- RClick = Shoot Secondary (Missiles)

## How It Works  
#### Game System

* Entity System
  * Object-Oriented
  * Dynamic Creation and Release of custom Entities through templating features: createEntity<MyEntity>()
  * Event-based Logic support: onCreate(), onUpdate(f32 deltaTime), ...
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
