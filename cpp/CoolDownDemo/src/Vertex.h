//=============================================================================
// Vertex.h by Frank Luna (C) 2005 All Rights Reserved.
//
// This file contains all of our vertex structures.  We will add to it
// as needed throughout the book.
//=============================================================================

#ifndef VERTEX_H
#define VERTEX_H

#include <d3dx9.h>


// Call in constructor and destructor, respectively, of derived application class.
void InitAllVertexDeclarations();
void DestroyAllVertexDeclarations();


//===============================================================
struct ColorVertex
{
	ColorVertex():pos(0.0f, 0.0f, 0.0f), color(0xFF00FF00){}
	ColorVertex(float x, float y, float z, DWORD c):pos(x,y,z), color(c){}
	ColorVertex(const D3DXVECTOR3& v):pos(v), color(0xFF00FF00){}

	D3DXVECTOR3 pos;
	DWORD       color;
	static DWORD FVF;
};

struct TexVertex
{
	TexVertex(float x, float y, float z, DWORD c, float u, float v)
		: pos(x,y,z)
		, tu(u), tv(v)
	{}

	D3DXVECTOR3  pos;
	float        tu, tv;
	static DWORD FVF;
};

#endif // VERTEX_H