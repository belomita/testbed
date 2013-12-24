#ifndef CDMask_h__
#define CDMask_h__

#include "d3dUtil.h"

class CDMask
{
public:
	CDMask(float width, bool loop, float time, DWORD color);
	~CDMask();

	void update(float dt);
	void render();

private:
	IDirect3DVertexBuffer9* mVB;
	IDirect3DIndexBuffer9*  mIB;

	float mWidth;
	bool  mLoop;
	float mCDTime;
	DWORD mColor;
	float mAngle;
	DWORD mStartIndex;

public:
	DWORD mVertexNum;
	DWORD mPrimNum;
};
#endif // CDMask_h__
