#include "CDMask.h"
#include "Vertex.h"

CDMask::CDMask(float width, bool loop, float time, DWORD color)
	: mWidth(width)
	, mLoop(loop)
	, mCDTime(time)
	, mColor(color)
	, mAngle(D3DX_PI / 2)
	, mStartIndex(0)
{
	mVertexNum = 7;
	mPrimNum = 5;

	// Obtain a pointer to a new vertex buffer.
	HR(gd3dDevice->CreateVertexBuffer(mVertexNum * sizeof(ColorVertex), D3DUSAGE_WRITEONLY,
		ColorVertex::FVF, D3DPOOL_MANAGED, &mVB, 0));

	ColorVertex* v = 0;
	HR(mVB->Lock(0, 0, (void**)&v, 0));
	v[0] = ColorVertex(0, 0, 0, mColor);
	v[1] = ColorVertex(mWidth/2, mWidth/2, 0, mColor);
	v[2] = ColorVertex(mWidth/2, mWidth/2, 0.0f, mColor);
	v[3] = ColorVertex(mWidth/2, -mWidth/2, 0.0f, mColor);
	v[4] = ColorVertex(-mWidth/2, -mWidth/2, 0.0f, mColor);
	v[5] = ColorVertex(-mWidth/2, mWidth/2, 0.0f, mColor);
	v[6] = ColorVertex(0, mWidth/2, 0, mColor);
	HR(mVB->Unlock());

	// Obtain a pointer to a new index buffer.
	HR(gd3dDevice->CreateIndexBuffer(mPrimNum * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIB, 0));

	// Now lock it to obtain a pointer to its internal data, and write the
	// cube's index data.
	WORD* k = 0;
	HR(mIB->Lock(0, 0, (void**)&k, 0));
	WORD j = 0;
	for (; j < mPrimNum; j++)
	{
		k[j * 3]   = 0;
		k[j * 3+1] = j+1;
		k[j * 3+2] = j+2;
	}

	HR(mIB->Unlock());
}

CDMask::~CDMask()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
static bool brender = true;
void CDMask::update( float dt )
{
	static float runTime = 0.0f;
	static float waitTime = 0.0f;

	runTime += dt;
	if (runTime >= mCDTime)
	{
		waitTime += dt;
		if (waitTime <= 1.0f)
		{
			brender = false;
			return;
		}
		else
		{
			runTime = waitTime = 0;
			mAngle = D3DX_PI / 2;
			brender = true;
		}
	}

	float fSpeed = 2 * D3DX_PI / mCDTime;

	mAngle = fmodf(mAngle - fSpeed * dt + 2 * D3DX_PI, 2 * D3DX_PI);
	if (mAngle >= D3DX_PI / 4 && mAngle < D3DX_PI / 2)
	{
		mStartIndex = 0;

		ColorVertex* v = 0;
		HR(mVB->Lock(0, 0, (void**)&v, 0));
		v[1] = ColorVertex(mWidth/2 / tanf(mAngle), mWidth/2, 0, mColor);
		v[2] = ColorVertex(mWidth/2, mWidth/2, 0.0f, mColor);
		v[3] = ColorVertex(mWidth/2, -mWidth/2, 0.0f, mColor);
		v[4] = ColorVertex(-mWidth/2, -mWidth/2, 0.0f, mColor);
		v[5] = ColorVertex(-mWidth/2, mWidth/2, 0.0f, mColor);
		v[6] = ColorVertex(0, mWidth/2, 0, mColor);
		HR(mVB->Unlock());
	}
	else if (mAngle >= 0 && mAngle < D3DX_PI / 4 || mAngle >= 7 * D3DX_PI / 4 && mAngle <= D3DX_PI * 2)
	{
		mStartIndex = 3;

		ColorVertex* v = 0;
		HR(mVB->Lock(0, 0, (void**)&v, 0));
		v[1] = ColorVertex(0, mWidth/2, 0, mColor);
		v[2] = ColorVertex(mWidth/2, mWidth/2 * tanf(mAngle), 0.0f, mColor);
		v[3] = ColorVertex(mWidth/2, -mWidth/2, 0.0f, mColor);
		v[4] = ColorVertex(-mWidth/2, -mWidth/2, 0.0f, mColor);
		v[5] = ColorVertex(-mWidth/2, mWidth/2, 0.0f, mColor);
		v[6] = ColorVertex(0, mWidth/2, 0, mColor);
		HR(mVB->Unlock());
	} 
	else if (mAngle >= 5 * D3DX_PI / 4 &&  mAngle < 7 * D3DX_PI / 4)
	{
		mStartIndex = 6;

		ColorVertex* v = 0;
		HR(mVB->Lock(0, 0, (void**)&v, 0));
		v[1] = ColorVertex(0, mWidth/2, 0, mColor);
		v[2] = ColorVertex(mWidth/2, mWidth/2, 0.0f, mColor);
		v[3] = ColorVertex(mWidth/2 / tanf(D3DX_PI - mAngle), -mWidth/2, 0.0f, mColor);
		v[4] = ColorVertex(-mWidth/2, -mWidth/2, 0.0f, mColor);
		v[5] = ColorVertex(-mWidth/2, mWidth/2, 0.0f, mColor);
		v[6] = ColorVertex(0, mWidth/2, 0, mColor);
		HR(mVB->Unlock());
	}
	else if (mAngle >= 3 * D3DX_PI / 4 &&  mAngle < 5 * D3DX_PI / 4)
	{
		mStartIndex = 9;

		ColorVertex* v = 0;
		HR(mVB->Lock(0, 0, (void**)&v, 0));
		v[1] = ColorVertex(0, mWidth/2, 0, mColor);
		v[2] = ColorVertex(mWidth/2, mWidth/2, 0.0f, mColor);
		v[3] = ColorVertex(mWidth/2, -mWidth/2, 0.0f, mColor);
		v[4] = ColorVertex(-mWidth/2, -mWidth/2 * tanf(mAngle), 0.0f, mColor);
		v[5] = ColorVertex(-mWidth/2, mWidth/2, 0.0f, mColor);
		v[6] = ColorVertex(0, mWidth/2, 0, mColor);
		HR(mVB->Unlock());
	}
	else if (mAngle >=  D3DX_PI / 2 &&  mAngle < 3 * D3DX_PI / 4)
	{
		mStartIndex = 12;

		ColorVertex* v = 0;
		HR(mVB->Lock(0, 0, (void**)&v, 0));
		v[1] = ColorVertex(0, mWidth/2, 0, mColor);
		v[2] = ColorVertex(mWidth/2, mWidth/2, 0.0f, mColor);
		v[3] = ColorVertex(mWidth/2, -mWidth/2, 0.0f, mColor);
		v[4] = ColorVertex(-mWidth/2, -mWidth/2, 0.0f, mColor);
		v[5] = ColorVertex(-mWidth/2 / tanf(D3DX_PI - mAngle), mWidth/2, 0.0f, mColor);
		v[6] = ColorVertex(0, mWidth/2, 0, mColor);
		HR(mVB->Unlock());
	}
}

void CDMask::render()
{
	if (!brender)
		return;

	HR(gd3dDevice->SetStreamSource(0, mVB, 0, sizeof(ColorVertex)));
	HR(gd3dDevice->SetFVF(ColorVertex::FVF));

	HR(gd3dDevice->SetIndices(mIB));

	gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	gd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	gd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertexNum, mStartIndex, mPrimNum));

	gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

