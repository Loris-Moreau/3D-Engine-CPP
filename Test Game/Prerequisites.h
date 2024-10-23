#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

class  Window;
class  Display;

class  GraphicsEngine;
class  Shader;
class  SwapChain;
class  VertexBuffer;
class  IndexBuffer;
class  ConstantBuffer;
class  Texture2D;

class InputManager;

class  ResourceManager;
class  Resource;
class  Texture;
class  Mesh;
class  Material;

class  Game;
class  Entity;
class  MeshEntity;
class  LightEntity;
class  CameraEntity;

typedef std::shared_ptr<SwapChain> SwapChainPtr;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr<ConstantBuffer> ConstantBufferPtr;
typedef std::shared_ptr<Resource> ResourcePtr;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Mesh> MeshPtr;
typedef std::shared_ptr<Material> MaterialPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<Texture2D> Texture2DPtr;

typedef unsigned int ui32;
typedef int i32;
typedef float f32;
typedef double d64;

enum class  Key
{
	Escape = 0,
	Shift, Space,
	A, B, C, D, E, F, G, H, I, J, K, L, M,
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
};

enum class  TextureType
{
	Normal = 0,
	RenderTarget,
	DepthStencil
};

enum class  MouseButton
{
	Left = 0,
	Middle,
	Right
};

enum class  CullMode
{
	Front = 0,
	Back
};

struct MaterialSlot
{
	size_t start_index = 0;
	size_t num_indices = 0;
	size_t material_id = 0;
};

struct ConstantBufferDesc
{
	void* buffer = nullptr;
	ui32 bufferSize = 0;
};

struct VertexBufferDesc
{
	void* verticesList = nullptr;
	ui32 vertexSize = 0;
	ui32 listSize = 0;
};

struct IndexBufferDesc
{
	void* indicesList = nullptr;
	ui32 listSize = 0;
};

struct ShaderDesc
{
	const wchar_t* vertexShaderFilePath;
	const char* vertexShaderEntryPointName;
	
	const wchar_t* pixelShaderFilePath;
	const char* pixelShaderEntryPointName;
};

#define ERROR_3D(message)\
{\
std::stringstream m;\
m << "3D Error: " << (message) << std::endl;\
throw std::runtime_error(m.str());\
}

#define WARNING_3D(message)\
std::wclog << "3D Warning: " << (message) << std::endl;


#define INFO_3D(message)\
std::wclog << "3D Info: " << (message) << std::endl;
