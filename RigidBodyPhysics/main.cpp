
#include "DxUtInclude.h"
#include "DxUtD3DApp.h"
#include "DxUtFPCamera.h"
#include "DxUtMesh.h"
#include "DxUtRigidBodyWorld.h"
#include "DxUtShaders.h"

#include <fstream>

enum Scenes {
	TestVE,
	TestFE1,
	TestFE2,
	TestEE,
	Waffle,
	BoxPyramid,
	BoxStack,
	NutBolt,
	Funnel,
	TankScene,
};
//https://gist.github.com/badboy/6267743	

Scenes g_Scene = Scenes::TankScene;

bool g_bUseHierarchicalLevelSet=0;

bool g_bDrawCollisionGraphics = 0;

void CreateObj();
void CreateTestVE();
void CreateTestFE1();
void CreateTestFE2();
void CreateTestEE();
void CreateWaffleScene();
void CreateBoxPyramidScene();
void CreateBoxStackScene();
void CreateNutBoltScene();
void CreateFunnelScene();
void CreateTankScene();
void CreateBall();

void Render();
void OnWindowResize();
void ShutDown();

using DxUt::g_pSwapChain;
using DxUt::g_pD3DDevice;
using DxUt::g_pRenderTargetView;
using DxUt::g_pDepthStencilView;
using DxUt::g_App;
using DxUt::Vector3F;

DxUt::CApp * g_App = 0;

DxUt::SLightDir g_Light = {
		D3DXCOLOR(.7f, .7f, .7f, 1.f), 
		D3DXCOLOR(.5f, .5f, .5f, 1.f), 
		D3DXCOLOR(.8f, .8f, .8f, 1.f), 
		DxUt::Vector3F(0, -.707f, .707f)
};

DxUt::CFPCamera g_Camera;

DxUt::CRigidBodyWorld g_RBWorld;

DxUt::CMeshPNT g_Box;
DxUt::CMeshPNT g_Waffle;
DxUt::CMeshPNT g_Platform;
DxUt::CMeshPNT g_Sphere;
DxUt::CMeshPNT g_Nut;
DxUt::CMeshPNT g_Bolt;
DxUt::CMeshPNT g_Tread;
DxUt::CMeshPNT g_Wheel;
DxUt::CMeshPNT g_Connector;
DxUt::CMeshPNT g_SquareFunnel;
DxUt::CMeshPNT g_Ground;
DxUt::CMeshPNT g_Ramp;

DxUt::CPNTPhongFx g_Effect;

const UINT g_uiScreenWidth = 800;
const UINT g_uiScreenHeight = 600;

DWORD g_dwBallId=0;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, PSTR line, int show)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(165);

	g_App = new DxUt::CD3DApp(hInst, L"ContactNormal", L"ContactNormal", 0, 0, g_uiScreenWidth, g_uiScreenHeight, OnWindowResize);

	CreateObj();
	switch (g_Scene) {
		case Scenes::TestVE:		CreateTestVE(); break;
		case Scenes::TestFE1:		CreateTestFE1(); break;
		case Scenes::TestFE2:		CreateTestFE2(); break;
		case Scenes::TestEE:		CreateTestEE(); break;
		case Scenes::Waffle:		CreateWaffleScene(); break;
		case Scenes::BoxPyramid:	CreateBoxPyramidScene(); break;
		case Scenes::BoxStack:		CreateBoxStackScene(); break;
		case Scenes::NutBolt:		CreateNutBoltScene(); break;
		case Scenes::Funnel:		CreateFunnelScene(); break;
		case Scenes::TankScene:		CreateTankScene(); break;
	}
	//CreateGlass();
 	CreateBall();
	//g_RayTracer.AddAllRigidBodyWorldMeshes();
	g_RBWorld.SetCameraForCollisionGraphics(&g_Camera);

	g_App->Loop(Render);

	ShutDown();
	g_App->Destroy();
	delete g_App;

	return 0;
}

void CreateObj()
{ 
	//Create camera
	g_Camera.CreateFPCameraLH(.5*D3DX_PI, g_uiScreenWidth, g_uiScreenHeight, .01f, 100.f, DxUt::Vector3F(.475f, .475f, .475f), .01f);
	g_Camera.SetFPCamera(DxUt::Vector3F(0, 5.f, -5.5f), .5*D3DX_PI, 0);

	//Create the mesh
	g_Box.LoadMeshFromFile("box.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_SubdividedBox.LoadMeshFromFile("subdivided_box.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_BigBox.LoadMeshFromFile("Box.txt", D3DX10_MESH_32_BIT, Vector3F(4.f));
	//g_Waffle.LoadMeshFromFile("Frame.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_Lattice.LoadMeshFromFile("lattice.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	///g_BoxInflated.LoadMeshFromFile("Box.txt", D3DX10_MESH_32_BIT, Vector3F(1.1f));
	g_Platform.LoadMeshFromFile("platform.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Sphere.LoadMeshFromFile("sphere.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	/*g_SphereHR.LoadMeshFromFile("sphere_hr.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Torus.LoadMeshFromFile("torus.txt", D3DX10_MESH_32_BIT, Vector3F(1.5));
	g_TorusHR.LoadMeshFromFile("torus_hr.txt", D3DX10_MESH_32_BIT, Vector3F(1.5));
	g_Bowl.LoadMeshFromFile("bowl.txt", D3DX10_MESH_32_BIT, Vector3F(2.5));
	g_Bunny.LoadMeshFromFile("bunny.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Bunny_slr.LoadMeshFromFile("bunny_slr.txt", D3DX10_MESH_32_BIT, Vector3F(.6));
	g_Glass.LoadMeshFromFile("glass_lr.txt", D3DX10_MESH_32_BIT, Vector3F(4.f));
	g_SmoothCube.LoadMeshFromFile("smooth_cube.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_BunnyHR.LoadMeshFromFile("bunny_hr.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Dragon.LoadMeshFromFile("dragon.txt", D3DX10_MESH_32_BIT, Vector3F(2.));
	g_Nut.LoadMeshFromFile("nut.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Bolt.LoadMeshFromFile("bolt.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Funnel.LoadMeshFromFile("funnel.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_rgPuzzlePieces[0].LoadMeshFromFile("puzzle_piece1.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_rgGear[0].LoadMeshFromFile("gear1.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_rgGear[1].LoadMeshFromFile("gear2.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));*/
	g_Tread.LoadMeshFromFile("tread.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Wheel.LoadMeshFromFile("wheel2.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	/*
	g_Rod.LoadMeshFromFile("Rod.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Plank.LoadMeshFromFile("Plank.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Connector.LoadMeshFromFile("Connector.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_SquareFunnel.LoadMeshFromFile("funnel.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Bottle.LoadMeshFromFile("bottle.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Cap.LoadMeshFromFile("cap.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Ground.LoadMeshFromFile("ground.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Ramp.LoadMeshFromFile("ramp.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_FrictionBlock.LoadMeshFromFile("friction_block.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_FrictionBox.LoadMeshFromFile("friction_box.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Homer.LoadMeshFromFile("homer.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Dilo.LoadMeshFromFile("dilo.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Horse.LoadMeshFromFile("horse.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));*/

	/*char file[256] = {"lion_00.txt"};
	for (UINT i=0; i<g_nLionParts; i++) {
		file[strlen(file)-5] = '0' + i;
		g_rgLionPart[i].LoadMeshFromFile(file, D3DX10_MESH_32_BIT, Vector3F(1.f));
	}*/
	
	//g_Convex[1].LoadMeshFromFile("C:\\Users\\Aric\\Desktop\\Research\\Convex\\convex_2.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_Convex[2].LoadMeshFromFile("C:\\Users\\Aric\\Desktop\\Research\\Convex\\convex_3.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_Convex[3].LoadMeshFromFile("C:\\Users\\Aric\\Desktop\\Research\\Convex\\convex_4.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));


	/*g_RayTracer.CreateRayTracer("Ouput.png", 860, 640, 1, DIK_9, 100, &g_RBWorld);
	DxUt::SLightSource light = {
		DxUt::Vector3F(.4, .4f, .4f),
		DxUt::Vector3F(.8, .8f, .8f),
		DxUt::Vector3F(.6, .6f, .6f),
		//DxUt::Vector3F(0, 0, 1.f)
		DxUt::Vector3F(0, -.707f, .707f)
	};
	g_RayTracer.AddLight(light);*/
}

void CreateTestVE()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.25*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal*rot1;
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.8, 3.9, 0);
}

void CreateTestFE1()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.25*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal;
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 3.9, 0);//DxUt::Vector3F(5.99, 3.9, 0);
}

void CreateTestFE2()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset",  100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.25*D3DX_PI);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal;
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.5, 2.45, .5);//-1 - (1/.707)+.6);
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.9, 1.8, 0);//-1 - (1/.707)+.6);
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 2.6, .5);//-1 - (1/.707)+.6);
	//g_RBWorld.GetRigidBody(1)->GetLinVel() = DxUt::Vector3F(0, 0, -.3f);
	//g_RBWorld.GetRigidBody(1)->GetAngVel() = DxUt::Vector3F(0, 10., 0);
}

void CreateTestEE()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.0*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal*rot1;
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.5, 2.45, .5);//-1 - (1/.707)+.6);
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.9, 1.8, 0);//-1 - (1/.707)+.6);
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(1.2, 1.9, 0);//-1 - (1/.707)+.6);
	//g_RBWorld.GetRigidBody(1)->GetLinVel() = DxUt::Vector3F(0, 0, -.3f);
	//g_RBWorld.GetRigidBody(1)->GetAngVel() = DxUt::Vector3F(0, 10., 0);

	//Edge-edge
	rotHorizontal.MRotationXLH(-.25*D3DX_PI);
	rot1.MRotationZLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 2.f, 0.);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal*rot1;
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.1, 2.45, -1.5 - (1/.707)+1.);//DxUt::Vector3F(-.447, 2.45, -1.5 - (1/.707)+1.);
}

void CreateWaffleScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Waffle, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "lattice.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Waffle, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "lattice.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot, rot2;
	rot.MRotationXLH(D3DX_PI);
	rot2.MRotationYLH(.5f*D3DX_PI);
	g_RBWorld.GetRigidBody(1)->GetRot() = rot2*rot;
	g_RBWorld.GetRigidBody(1)->GetPos() = Vector3F(0.f, 1.25f, -0.f);
}

void CreateBoxPyramidScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, DxUt::Vector3F(0, -3.8f, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.02f, .0f, .0f, 1.f);
	mat.dif = D3DXCOLOR(.02f, .0f, .0f, 1.f);

	g_RBWorld.AddRigidBody(&g_Platform, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, .5f, zero, zero, "platform.lvset", 100, DxUt::CRigidBody::GT_OBBOX);

	mat.amb = D3DXCOLOR(.1f, .1f, .2f, 1.f);
	mat.dif = D3DXCOLOR(.2f, .2f, .6f, 1.f);

	DxUt::Matrix4x4F rot1, rotHorizontal, rot2;
	//Face-face
	rotHorizontal.MRotationXLH(-.0*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	//g_RBWorld.GetRigidBody(0)->GetRot() = rot2*rotHorizontal;
	
	srand(0);

	float mass = 1;
	int nBoxesWide = 25;
	int nBoxesHigh = 25;
	float fBoxCenter = 0.f;
	float fBoxWidth = 2.0f;
	float fBoxHeight = 2.01f;
	float fBoxLeft = -nBoxesWide*fBoxWidth/2.f; UINT idx=1;
	for (int j=0; j<=nBoxesHigh; j++) {
		//if (j==0) mass = -10.f;
		//else mass = 10.f;
		fBoxLeft = -(nBoxesWide - j)*fBoxWidth/2.f;
		for (int i=0; i<=nBoxesWide - j; i++) {
			DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*(fBoxWidth+.2) + (rand() % 256)*.000, 2 + fBoxHeight*j+.0001, (rand() % 256)*.000 )); 
			//boxPos = Vector3F(0, 4.f, -4.7f);
			//DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*fBoxWidth + .1*(i%2), 2 + fBoxWidth*j, .2f*(j%2))); 
			//DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*fBoxWidth, 2 + fBoxWidth*j, 0)); 
			mat.dif = D3DXCOLOR((rand()%256)/256.f, (rand()%256)/256.f, (rand()%256)/256.f, 1.f);
			g_RBWorld.AddRigidBody(&g_Box, 1.f, mass, boxPos, idenity, zero,
				zero, 1.f, 0.0f, zero, zero, "box.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);

			//g_RBWorld.GetRigidBody(1+i*(j+1))->GetPos() = DxUt::Vector3F(fBoxLeft + i*fBoxWidth, 2 + fBoxWidth*j, 0);
			//g_RBWorld.GetRigidBody(1+i*(j+1))->GetLinVel() = DxUt::Vector3F(0, 0, 1.f);
			//g_RBWorld.GetRigidBody(idx++)->GetAngVel() = DxUt::Vector3F(0, 1.f, 0);
		}
	}
}


void CreateBoxStackScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, DxUt::Vector3F(0, -3.8f, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.amb = D3DXCOLOR(.4f, .4f, .4f, 1.f);
	mat.dif = D3DXCOLOR(.8f, .0f, .8f, 1.f);
	mat.pow = 16.f;

	float mass = 1;
	int nBoxesHigh = 20;
	float fBoxCenter = 0.f;
	float fBoxHeight = 2.01f;
	for (int j=0; j<=nBoxesHigh; j++) {
		if (j==0) mass = -1;
		else mass = 1;
		mat.dif = D3DXCOLOR((rand()%256)/256.f, (rand()%256)/256.f, (rand()%256)/256.f, 1.f);
		DxUt::Vector3F boxPos(DxUt::Vector3F(0, 2 + fBoxHeight*j+.0001, 0)); 
		//if (j==1)
		//	boxPos = (DxUt::Vector3F(-.22, 2 + fBoxHeight*j+.0001, -.2)); 
		g_RBWorld.AddRigidBody(&g_Box, 1.f, mass, boxPos, idenity, zero,
			zero, 1.f, 0.0f, zero, zero, "box.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
	}
}

void CreateBall()
{
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F pos(0, 0, -1000), zero(0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);
	g_dwBallId = g_RBWorld.AddRigidBody(&g_Sphere, 1.f, 100.f, pos, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "sphere.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
}

void CreateNutBoltScene()
{
	g_RBWorld.CreateRigidBodyWorld(2, 32);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);

	DxUt::Matrix4x4F rot;
	rot.MRotationZLH(0.*3.141);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.4f, .4f, .4f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.6f, .3f, .3f, 1.f);
	mat.dif = D3DXCOLOR(1.f, .6f, .6f, 1.f);
	
	g_RBWorld.AddRigidBody(&g_Nut, 1.f, 1.f, Vector3F(0, 0.f, 0), rot, zero,
		Vector3F(0, 1.1f, 0), 1.f, 0.f, zero, zero, "nut.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	mat.amb = D3DXCOLOR(.3f, .3f, .6f, 1.f);
	mat.dif = D3DXCOLOR(.6f, .6f, 1.f, 1.f);

	g_RBWorld.AddRigidBody(&g_Bolt, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "bolt.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
}

void CreateFunnelScene()
{
	/*g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet, &Vector3F(0, -3.8f, 0), .03, 14.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);

	DxUt::Matrix4x4F rot;
	rot.MRotationZLH(0.*3.141);
	DxUt::Matrix4x4F rot2;
	rot2.MRotationZLH(.4*3.141);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.8f, .8f, .8f, 1.f);
	mat.pow = 16.f;

	//mat.amb = D3DXCOLOR(.9f, .3f, .3f, 1.f);
	//mat.dif = D3DXCOLOR(.9f, .2f, .2f, 1.f);
	
	mat.amb = D3DXCOLOR(.6f, .1f, .1f, 1.f);
	mat.dif = D3DXCOLOR(.75f, .1f, .1f, 1.f);

	int nPoly = 0;
	
	std::fstream file("specs.txt");
	UINT uiTriPerVolume;
	int nObjects;
	file >> nObjects;
	file >> uiTriPerVolume;
	file.close();
	
	g_RBWorld.AddRigidBody(&g_Funnel, 1.f, -1.f, zero, rot, zero,
		zero, 1.f, 0.f, zero, zero, "funnel.lvset", uiTriPerVolume, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	nPoly += g_Funnel.GetNumTriangles();

	g_RBWorld.AddRigidBody(&g_Sphere, 1.f, -1.f, Vector3F(0, -14.f, 0), rot, zero,
				zero, 1.f, 0.f, zero, zero, "sphere_lr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	nPoly += g_Sphere.GetNumTriangles();

	//g_RBWorld.AddRigidBody(&g_Box, 1.f, -1.f, Vector3F(0, -10, 0), idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "Box.lvset", .05, 1, DxUt::CRigidBody::GT_OBBOX, &mat);

	/*mat.amb = D3DXCOLOR((rand() % 255)/255.f, (rand() % 255)/255.f, (rand() % 255)/255.f, 1.f);//D3DXCOLOR(.6f, .3f, .3f, 1.f);
	g_RBWorld.AddRigidBody(&g_Bolt, 1.f, 1.f, DxUt::Vector3F(-6,6, -.5), rot2, zero,
				zero, 1.f, 0.f, zero, zero, "bolt.lvset", .08, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	float objRadius = 12.5f;
	Vector3F objPos(8.f, -2.5f, 0.f);
	srand(90);
	for (int i=0; i<nObjects; i++) {
		Vector3F offset(0, i*objRadius+2.5f, 0);
		int choice = i % 2;
		mat.amb = D3DXCOLOR((rand() % 255)/255.f, (rand() % 255)/255.f, (rand() % 255)/255.f, 1.f);//D3DXCOLOR(.6f, .3f, .3f, 1.f);
		mat.dif = 2.f*mat.amb;
		choice = 3;//i%3;
		switch (choice) {
		case 0:
			g_RBWorld.AddRigidBody(&g_Homer, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "homer.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Homer.GetNumTriangles();
			break;
		case 1:
			g_RBWorld.AddRigidBody(&g_Dilo, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "dilo.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Dilo.GetNumTriangles();
			break;
		case 2:
			g_RBWorld.AddRigidBody(&g_Horse, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "horse.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Horse.GetNumTriangles();
			break;
		case 3:
			g_RBWorld.AddRigidBody(&g_Sphere, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "sphere_lr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Sphere.GetNumTriangles();
			break; 
		case 4:
			g_RBWorld.AddRigidBody(&g_Bunny, 1.f, 1.f, -objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "bunny.lvset", uiTriPerVolume, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Bunny.GetNumTriangles();
			break;
		case 5:
			g_RBWorld.AddRigidBody(&g_BunnyHR, 1.f, 1.,  -objPos+offset, idenity, zero, 
				zero, 1.f, 0.f, zero, zero, "bunny_hr.lvset", uiTriPerVolume, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			nPoly += g_BunnyHR.GetNumTriangles();
			break;
		}
			
	}
	g_App->Print((int)nPoly);*/
}

UINT g_uiWheelPos[2];

void CreateTankScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(20, 1000, g_bUseHierarchicalLevelSet, DxUt::Vector3F(0, -3.8, 0));
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	UINT uiStride = sizeof(DxUt::SVertexPNT);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.02f, .0f, .0f, 1.f);
	mat.dif = D3DXCOLOR(.02f, .0f, .0f, 1.f);

	mat.amb = D3DXCOLOR(.1f, .1f, .2f, 1.f);
	mat.dif = D3DXCOLOR(.2f, .2f, .6f, 1.f);

	DxUt::Matrix4x4F rot1, rotHorizontal, rot2;
	//Face-face
	rotHorizontal.MRotationXLH(-.0*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	rot2.MRotationZLH(.12*D3DX_PI);

	g_RBWorld.AddRigidBody(&g_Platform, 1.f, -200.f, Vector3F(0, -17.f, 0), idenity, zero,
		zero, 1.f, .5f, zero, zero, "platform.lvset", 100, DxUt::CRigidBody::GT_OBBOX);

	std::fstream stream("tank_info.txt");
	char name[256];
	UINT uiRod = 0;
	int count=0;
	while ((stream >> name)) {
		float mass = 1.f;
		Vector3F pos;
		stream >> pos.x;
		stream >> pos.y;
		stream >> pos.z;
		D3DXQUATERNION quat;
		stream >> quat.x;
		stream >> quat.y;
		stream >> quat.z;
		stream >> quat.w;
		DxUt::Matrix4x4F rot;
		D3DXMatrixRotationQuaternion((D3DXMATRIX*)&rot, &quat);
		float offset = 16.f;
		if (strstr(name, "poly") != NULL) {
			g_RBWorld.AddRigidBody(&g_Tread, 1.f, .1f*mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "tread.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		} else if (strstr(name, "wheel") != NULL) {
			UINT uiRBIndex = g_RBWorld.AddRigidBody(&g_Wheel, 1.f, mass, pos, rot, zero,
				Vector3F(0, 0, 0), 1.f, 0.f, zero, Vector3F(0, 0, 0), "wheel2.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
			//g_RBWorld.AddCenterOfMassPositionConstraint(uiRBIndex, pos);
			g_uiWheelPos[count++] = uiRBIndex;
		}
	}
}

void Render()
{
	//float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	//float color[] = {0.2f, 0.2f, .5f, 1.0f};
	float color[] = {1.f, 1.f, 1.f, 1.0f};
	//float color[] = {.5f, 0.4f, 0.9f, 1.0f};
	g_pD3DDevice->ClearRenderTargetView(g_pRenderTargetView, color);
	g_pD3DDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	//static int counter = 0;
	//if (counter < 18) {
	//	g_RBWorld.UpdateRigidBodies(.03, DxUt::Vector3F(0, -3.8f, 0));
	//} counter++;
	static int count = 0;
	float timeStep = .015f;
	static bool bInitialGo = 0;
	static float totalTime = 0;
	//if (count++ > 100) timeStep = .01f;
	static int counter = 0;
	if (DxUt::g_MouseState.rgbButtons[1] || bInitialGo) {
		counter++;
		g_RBWorld.UpdateRigidBodies(timeStep, DxUt::Vector3F(0, -4.8, 0));
		//Sleep(100);
	}
	if (totalTime > 3)
		bInitialGo = 0;
	totalTime += timeStep;
	//else g_RBWorld.UpdateRigidBodies(0, DxUt::Vector3F(0, -3.8f, 0));
	
	if (DxUt::g_KeysState[DIK_G]) {
		g_bDrawCollisionGraphics = !g_bDrawCollisionGraphics;
		Sleep(100);
	}
	if (g_bDrawCollisionGraphics)
		g_RBWorld.DrawCollisionGraphics(&g_Camera);

	if (DxUt::g_KeysState[DIK_D]) {
		g_RBWorld.SetCenterOfMassPositionConstraint(g_uiWheelPos[0], Vector3F(-.001f, 0, 0));
		g_RBWorld.SetCenterOfMassPositionConstraint(g_uiWheelPos[1], Vector3F(.001f, 0, 0));
		//g_RBWorld.GetRigidBody(g_uiWheelPos[0])->GetAngVel() += DxUt::Vector3F(0, 0, .05f);
		//g_RBWorld.GetRigidBody(g_uiWheelPos[1])->GetAngVel() += DxUt::Vector3F(0, 0, .05f);
	}

	if (DxUt::g_KeysState[DIK_E]) {
		g_RBWorld.GetRigidBody(g_uiWheelPos[0])->GetAngVel() -= DxUt::Vector3F(0, 0, .05f);
		g_RBWorld.GetRigidBody(g_uiWheelPos[1])->GetAngVel() -= DxUt::Vector3F(0, 0, .05f);
	}

	static bool once = 1;
	if (!once || DxUt::g_KeysState[DIK_SPACE] || counter == -1) {
		//g_RBWorld.GetRigidBody(g_dwBallId)->GetPos() = Vector3F(0, 6.f, 0);
		//g_RBWorld.GetRigidBody(g_dwBallId)->GetLinVel() = Vector3F(0, 0.f, 0);
		g_RBWorld.GetRigidBody(g_dwBallId)->GetPos() = g_Camera.GetPosition();
		g_RBWorld.GetRigidBody(g_dwBallId)->GetLinVel() = 20.f*g_Camera.GetForwardVector();once = 1;
	}

	g_Camera.UpdateFPCamera(50.f*DxUt::g_SPFrame);

	g_RBWorld.DrawRigidBodies(&g_Camera, g_Light, 0);


	/*if (g_Scene == Tank) {
		if (DxUt::g_KeysState[DIK_A]) {
			DxUt::Matrix4x4F rot = g_RBWorld.GetRigidBody(g_uiRod[0])->GetRot();
			Vector3F v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_uiRod[0])->GetAngVel() += 10.f*v3;
			rot = g_RBWorld.GetRigidBody(g_uiRod[1])->GetRot();
			v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_uiRod[1])->GetAngVel() += -10.f*v3;
		}
		if (DxUt::g_KeysState[DIK_D]) {
			DxUt::Matrix4x4F rot = g_RBWorld.GetRigidBody(g_uiRod[0])->GetRot();
			Vector3F v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_uiRod[0])->GetAngVel() += -10.f*v3;
			rot = g_RBWorld.GetRigidBody(g_uiRod[1])->GetRot();
			v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_uiRod[1])->GetAngVel() += -10.f*v3;
		}
	}*/

	//g_CGraphics.SetupTriangleDraw(Vector3F(1.f, 0, 0));
	//g_CGraphics.DrawTriangle(DxUt::Vector3F(5.f, 0, 0), DxUt::Vector3F(0, 5.f, 0), DxUt::Vector3F(0, 0, 0.f));

	//g_RayTracer.SetNumSamples(16);
	//g_RayTracer.UpdateRaytracer(&g_Camera);
	//g_RayTracer.DrawRays(&g_CGraphics);


	/*ID3D10Texture2D * pBuffer;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBuffer);
	static ID3D10Texture2D * texture_to_save = NULL;
	if(texture_to_save == NULL) {
		D3D10_TEXTURE2D_DESC td;
		pBuffer->GetDesc(&td);
		g_pD3DDevice->CreateTexture2D(&td, NULL, &texture_to_save);
	}
	g_pD3DDevice->CopyResource(texture_to_save, pBuffer);
	char filename[256];
	static int counter  =0;
	sprintf(filename, "Images/Rendering %.6d.jpg", counter++); 
	D3DX10SaveTextureToFileA(texture_to_save, D3DX10_IFF_JPG, filename);*/
	
	
	g_pSwapChain->Present(0, 0);
}

void OnWindowResize()
{
	g_Camera.OnSize();
}

void ShutDown()
{
	g_RBWorld.DestroyRigidBodyWorld();
}


