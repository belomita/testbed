#include "Quad.h"
#include "Vertex.h"

Quad::Quad(float width, DWORD color, const char* imagename)
	: mWidth(width)
	, mColor(color)
	, mTexture(0)
{
	mVertexNum = 4;
	mPrimNum   = 2;
	// Obtain a pointer to a new vertex buffer.
	HR(gd3dDevice->CreateVertexBuffer(mVertexNum * sizeof(TexVertex), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mVB, 0));

	// Now lock it to obtain a pointer to its internal data, and write the
	// cube's vertex data.

	TexVertex* v = 0;
	HR(mVB->Lock(0, 0, (void**)&v, 0));

	v[0] = TexVertex(-mWidth / 2, mWidth / 2, 0, mColor, 0, 0);
	v[1] = TexVertex(mWidth / 2, mWidth / 2, 0, mColor, 1, 0);
	v[2] = TexVertex(mWidth / 2, -mWidth / 2, 0, mColor, 1, 1);
	v[3] = TexVertex(-mWidth / 2, -mWidth / 2, 0, mColor, 0, 1);

	HR(mVB->Unlock());

	// Obtain a pointer to a new index buffer.
	HR(gd3dDevice->CreateIndexBuffer(mPrimNum * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIB, 0));

	// Now lock it to obtain a pointer to its internal data, and write the
	// cube's index data.

	WORD* k = 0;
	HR(mIB->Lock(0, 0, (void**)&k, 0));
	k[0] = 0; k[1] = 1; k[2] = 2;
	k[3] = 0; k[4] = 2; k[5] = 3;

	HR(mIB->Unlock());

	HR(D3DXCreateTextureFromFile( gd3dDevice, imagename, &mTexture ));
}

void Quad::update( float dt )
{

}

void Quad::render()
{
	gd3dDevice->SetTexture( 0, mTexture );
	gd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );//D3DTOP_SELECTARG2
	//gd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	gd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	gd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	HR(gd3dDevice->SetStreamSource(0, mVB, 0, sizeof(TexVertex)));
	HR(gd3dDevice->SetFVF(TexVertex::FVF));

	HR(gd3dDevice->SetIndices(mIB));

	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertexNum, 0, mPrimNum));
}

Quad::~Quad()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mTexture);
}

