//=============================================================================
// Vertex.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "Vertex.h"
#include "d3dUtil.h"

// Initialize static variables.
//IDirect3DVertexDeclaration9* ColorVertex::Decl = 0;
DWORD ColorVertex::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE);
DWORD TexVertex::FVF = (D3DFVF_XYZ | D3DFVF_TEX1);

void InitAllVertexDeclarations()
{
	//===============================================================
	// VertexPos

	D3DVERTEXELEMENT9 VertexPosElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};	
	//HR(gd3dDevice->CreateVertexDeclaration(VertexPosElements, &ColorVertex::Decl));
}

void DestroyAllVertexDeclarations()
{
	//ReleaseCOM(ColorVertex::Decl);
}