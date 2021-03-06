#pragma once
#include<d3d11.h>
#include<D3DX11.h>
#include<xnamath.h>

class Vertex
{
public:
	Vertex()
	{
		position = { .0f, .0f, .0f };
		normal = { .0f, .0f, .0f }; 
		texture = {.0f,.0f}; 
	}
	Vertex(XMFLOAT3 pos,XMFLOAT3 nor,XMFLOAT2 tex):position(pos),
		normal(nor),
		texture(tex){}
	~Vertex(){}
	Vertex& operator = (const Vertex& other)
	{
		position = other.position;
		normal = other.normal;
		texture = other.texture;
		return *this;
	}
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texture;
};

class SkinVertex 
{
public:
	SkinVertex()
	{
		memset(boneIndices, 0, sizeof(boneIndices));
		//boneIndices = 0;
		weights = { 0, 0, 0, 0 };
	}

	void setBoneIndex(int slot,unsigned char value)
	{
		//value <<= (slot * 8);
		//switch (slot)
		//{
		//	case 0:boneIndices &= 0x00ffffff; break;
		//	case 1:boneIndices &= 0xff00ffff; break;
		//	case 2:boneIndices &= 0xffff00ff; break;
		//	case 3:boneIndices &= 0xffffff00; break;
		//}
		//boneIndices |= value;
		boneIndices[slot] = value;
	}
	unsigned int boneIndices[4];
	XMFLOAT4 weights;
	XMFLOAT4 position;
	XMFLOAT4 normal;
	XMFLOAT4 texture;
};