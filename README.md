# 3D Engine
 This is a Remake of Descent in my 3D Engine in C++

![](https://github.com/Loris-Moreau/3D-Engine-CPP/blob/Descent-Game/Assets/Showcase/Descent-Game.gif "Showcase")

*It's still missing some enemies, proper level design, some UI, some pickups & I need to fix collisions & the ships' roll.*

> [!Warning]
> Camera currently broken, don't ask why.
> 
> I messed around with the Matrix4x4 Class
>
> latest functional comit is [this](https://github.com/Loris-Moreau/3D-Engine-CPP/tree/867a849eccb32e17849df294d83c01c4a8f62776)
> 
> oops

### Controls : 

- **Esc** = Unlock Mouse

- **ZQSD** = Movement
  - ***QD*** = Strafe
- **Space** = Slide Up
- **Ctrl** = Slide Down

- **A** = roll left
- **E** = roll right

- **Mouse** = Pitch Up/Down & Yaw left/right

- **LClick** = Shoot Primary *(Lasers - Infinite Shots)*
- **RClick** = Shoot Secondary *(Missiles - Limited to 10 shots)*

---

## How It Works  
#### Game System

* Entity System
  * Object-Oriented
  * Dynamic Creation and Release of custom Entities through templating features: createEntity<MyEntity>()
  * Event-based Logic support : onCreate(), onUpdate(float deltaTime), ...
  * Default Entity classes available out-of-the-box : LightEntity, MeshEntity, CameraEntity,...

#### Graphics Engine

* Graphics APIs : DirectX 11
* Forward Rendering
* Phong-Based Lighting
* Static Meshes Support: .obj *(all formats supported by tinyobjloader)*
* Textures Support: .jpg,.png,.bmp,.tga etc. *(all formats supported by DirectXTex)*
* Materials Support: HLSL Shading Language, Textures, Custom Data Passage

#### Input Manager

* Mouse Input Events Support
* Keyboard Input Events Support

#### Windowing System

* Native windowing and event support under Windows.
