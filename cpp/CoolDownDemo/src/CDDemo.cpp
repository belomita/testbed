//=============================================================================
// CubeDemo.cpp by Frank Luna (C) 2005 All Rights Reserved.
//
// Demonstrates how to create the geometry of a cube and draw it.
//
// Controls: Use mouse to orbit and zoom; use the 'W' and 'S' keys to 
//           alter the height of the camera.
//=============================================================================

#include "d3dApp.h"
#include "DirectInput.h"
#include <crtdbg.h>
#include "GfxStats.h"
#include "Vertex.h"
#include <vector>
#include "Quad.h"
#include "CDMask.h"

static const float s_Length = 30.0f;
static const DWORD s_color = 0x80123456; //0xFFFF0000;

class CDDemo : public D3DApp
{
public:
	CDDemo(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~CDDemo();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void drawScene();

	// Helper methods
	void buildViewMtx();
	void buildProjMtx();

private:
	GfxStats* mGfxStats;
	
	float mCameraRotationY;
	float mCameraRadius;
	float mCameraHeight;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	Quad*    mQuad;
	CDMask*  mMask;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	CDDemo app(hInstance, "CD Demo", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gd3dApp = &app;

	DirectInput di(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	gDInput = &di;

	return gd3dApp->run();
}

CDDemo::CDDemo(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP)
: D3DApp(hInstance, winCaption, devType, requestedVP)
{
	if(!checkDeviceCaps())
	{
		MessageBox(0, "checkDeviceCaps() Failed", 0, 0);
		PostQuitMessage(0);
	}

	mGfxStats = new GfxStats();

	mCameraRadius    = 10.0f;
	mCameraRotationY = 1.2 * D3DX_PI;
	mCameraHeight    = 5.0f;


	onResetDevice();
	mQuad = new Quad(s_Length, 0xFFFF0089,  "textest.png");
	mMask = new CDMask(s_Length, false, 5.0f, 0x80666666);
}

CDDemo::~CDDemo()
{
	delete mGfxStats;
	SafeDelete(mQuad);
	SafeDelete(mMask);

	DestroyAllVertexDeclarations();

}

bool CDDemo::checkDeviceCaps()
{
	return true;
}

void CDDemo::onLostDevice()
{
	mGfxStats->onLostDevice();
}

void CDDemo::onResetDevice()
{
	mGfxStats->onResetDevice();

	// The aspect ratio depends on the back buffer dimensions, which can 
	// possibly change after a reset.  So rebuild the projection matrix.
	buildProjMtx();
}


void CDDemo::updateScene(float dt)
{
	mQuad->update(dt);
	mMask->update(dt);

	mGfxStats->setVertexCount(mQuad->mVertexNum + mMask->mVertexNum);
	mGfxStats->setTriCount(mQuad->mPrimNum + mMask->mPrimNum);
	mGfxStats->update(dt);

	// Get snapshot of input devices.
	gDInput->poll();

	// Check input.
	if( gDInput->keyDown(DIK_W) )	 
		mCameraHeight   += 25.0f * dt;
	if( gDInput->keyDown(DIK_S) )	 
		mCameraHeight   -= 25.0f * dt;

	// Divide by 50 to make mouse less sensitive. 
	mCameraRotationY += gDInput->mouseDX() / 100.0f;
	mCameraRadius    += gDInput->mouseDY() / 25.0f;

	// If we rotate over 360 degrees, just roll back to 0
	if( fabsf(mCameraRotationY) >= 2.0f * D3DX_PI ) 
		mCameraRotationY = 0.0f;

	// Don't let radius get too small.
	if( mCameraRadius < 5.0f )
		mCameraRadius = 5.0f;

	// The camera position/orientation relative to world space can 
	// change every frame based on input, so we need to rebuild the
	// view matrix every frame with the latest changes.
	buildViewMtx();
}


void CDDemo::drawScene()
{
	// Clear the back buffer and depth buffer.
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFEEEEEE, 1.0f, 0));

	HR(gd3dDevice->BeginScene());
	gd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	// World matrix is identity.
	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	HR(gd3dDevice->SetTransform(D3DTS_WORLD, &W));
	HR(gd3dDevice->SetTransform(D3DTS_VIEW, &mView));
	HR(gd3dDevice->SetTransform(D3DTS_PROJECTION, &mProj));

	//HR(gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME));
	mQuad->render();
	mMask->render();
		
	mGfxStats->display();

	HR(gd3dDevice->EndScene());

	// Present the back buffer.
	HR(gd3dDevice->Present(0, 0, 0, 0));
	
}

void CDDemo::buildViewMtx()
{
	float x = mCameraRadius * cosf(mCameraRotationY);
	float z = mCameraRadius * sinf(mCameraRotationY);
	//D3DXVECTOR3 pos(0, 0, -150);
	D3DXVECTOR3 pos(x, mCameraHeight, z);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void CDDemo::buildProjMtx()
{
	float w = (float)md3dPP.BackBufferWidth;
	float h = (float)md3dPP.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI * 0.25f, w/h, 1.0f, 5000.0f);
}

#if 0

void CDDemo::buildRectangle2()
{
	std::vector<ColorVertex> points;
	points.push_back(ColorVertex(0.0f, 0.0f, 0.0f, s_color));

	for(int i = 1; i <= 20; i++)
	{
		float y = 10.0f;
		float x = -10.0f + i * 1.0f;
		float z = 0.0f;
		points.push_back(ColorVertex(x, y, z, s_color));
	}
	for(int i = 1; i <= 20; i++)
	{
		float x = 10.0f;
		float y = 10.0f - i * 1.0f;
		float z = 0.0f;
		points.push_back(ColorVertex(x, y, z, s_color));
	}
	for(int i = 1; i <= 20; i++)
	{
		float y = -10.0f;
		float x = 10.0f - i * 1.0f;
		float z = 0.0f;
		points.push_back(ColorVertex(x, y, z, s_color));
	}
	for(int i = 1; i <= 20; i++)
	{
		float x = -10.0f;
		float y = -10.0f + i * 1.0f;
		float z = 0.0f;
		points.push_back(ColorVertex(x, y, z, s_color));
	}

	// Obtain a pointer to a new vertex buffer.
	HR(gd3dDevice->CreateVertexBuffer(points.size() * sizeof(ColorVertex), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mVB, 0));

	// Now lock it to obtain a pointer to its internal data, and write the
	// cube's vertex data.

	ColorVertex* v = 0;
	HR(mVB->Lock(0, 0, (void**)&v, 0));

	for (int i = 0; i < points.size(); i++)
	{
		v[i] = points[i];
	}

	HR(mVB->Unlock());

	mVertexNum = points.size();
	mVertexNum = mVertexNum;
	mPrimNum = mVertexNum - 1;
	mPrimNum = mPrimNum;

	// Obtain a pointer to a new index buffer.
	HR(gd3dDevice->CreateIndexBuffer(mPrimNum * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIB, 0));

	// Now lock it to obtain a pointer to its internal data, and write the
	// cube's index data.
	WORD* k = 0;

	HR(mIB->Lock(0, 0, (void**)&k, 0));
	int j = 0;
	for (; j < 79; j++)
	{
		k[j * 3]   = 0;
		k[j * 3+1] = j+1;
		k[j * 3+2] = j+2;
	}
	k[j * 3]   = 0;
	k[j * 3+1] = j+1;
	k[j * 3+2] = 1;

	HR(mIB->Unlock());
}

#endif