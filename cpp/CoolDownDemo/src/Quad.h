#ifndef Quad_h__
#define Quad_h__


#include "d3dUtil.h"

class Quad
{
public:
	Quad(float width, DWORD color, const char* imagename);
	~Quad();

	void update(float dt);
	void render();

private:
	IDirect3DVertexBuffer9* mVB;
	IDirect3DIndexBuffer9*  mIB;
	IDirect3DTexture9*      mTexture;

	DWORD mColor;

	float mWidth;

public:
	DWORD mVertexNum;
	DWORD mPrimNum;
};
#endif // Quad_h__
