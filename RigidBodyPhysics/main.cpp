
#include "DxUtInclude.h"
#include "DxUtD3DApp.h"
#include "DxUtFPCamera.h"
#include "DxUtMesh.h"
#include "DxUtRigidBodyWorld.h"
#include "DxUtShaders.h"
#include "DxUtRayTracer.h"

#include <fstream>


//#define HAVOK_GO
#ifndef HAVOK_GO
#define HAVOK_CONVERT(x) (x)
#else
#define HAVOK_CONVERT(x) x ##_Havok
#endif

#ifdef HAVOK_GO

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Fwd/hkcstdio.h>

// Physics
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

// Platform specific initialization
#include <Common/Base/System/Init/PlatformInit.cxx>

#include <Common/Base/KeyCode.cxx>
#include <Common/Base/Config/hkProductFeatures.cxx>

static void HK_CALL errorReport(const char* msg, void* userContext)
{
	using namespace std;
	printf("%s", msg);
}

#endif

enum Scenes {
	Test_VE,
	Test_FE1,
	Box_Box,
	Friction,
	Denting,
	Mod_Box,
	BoxEdge,
	EdgeOnBox,
	Edge_Edge,
	Waffle,
	Platform,
	Bowl,
	BoxStack,
	BoxPyramid,
	BoxOnCorner,
	BunnyScene,
	NutBolt,
	Funnel,
	Puzzle,
	Gears,
	Tank,
	Bottle,
	Lion,
	Chain
};
//https://gist.github.com/badboy/6267743	

Scenes g_Scene = Scenes::Denting;

bool g_bUseHierarchicalLevelSet=0;

bool g_bDrawCollisionGraphics = 0;

void CreateObj();
void CreateFrictionScene();
void CreateTestVE();
void CreateTestFE1();
void CreateDentingScene();
void CreateBoxScene();
void CreateBoxEdgeScene();
void CreateEdgeOnBoxScene();
void CreateEdgeEdgeScene();
void CreateWaffle();
void CreateBowlScene();
void CreatePlatformScene();
void CreateBoxStackScene();
void CreateBoxPyramidScene();
void CreateBoxPyramidScene_Havok();
void CreateBoxOnCornerScene();
void CreateBunnyScene();
void CreateNutBoltScene();
void CreateFunnelScene();
void CreatePuzzleScene();
void CreateGearsScene();
void CreateTankScene();
void CreateBottleScene();
void CreateLionScene();
void CreateBall();
void CreateGlass();
void CreateChainScene();
void Render();
void OnWindowResize();
void ShutDown();

using DxUt::g_pSwapChain;
using DxUt::g_pD3DDevice;
using DxUt::g_pRenderTargetView;
using DxUt::g_pDepthStencilView;
using DxUt::g_D3DApp;
using DxUt::Vector3F;

DxUt::CD3DApp * g_D3DApp = 0;

DxUt::SLightDir g_Light = {
		D3DXCOLOR(.7f, .7f, .7f, 1.f), 
		D3DXCOLOR(.5f, .5f, .5f, 1.f), 
		D3DXCOLOR(.8f, .8f, .8f, 1.f), 
		DxUt::Vector3F(0, -.707f, .707f)
};

DxUt::CFPCamera g_Camera;

DxUt::CRigidBodyWorld g_RBWorld;

DxUt::CMeshPNT g_Box;
DxUt::CMeshPNT g_SubdividedBox;
DxUt::CMeshPNT g_BigBox;
DxUt::CMeshPNT g_BoxInflated;
DxUt::CMeshPNT g_Waffle;
DxUt::CMeshPNT g_Lattice;
DxUt::CMeshPNT g_Platform;
DxUt::CMeshPNT g_PlatformLong;
DxUt::CMeshPNT g_Sphere;
DxUt::CMeshPNT g_SphereHR;
DxUt::CMeshPNT g_Torus;
DxUt::CMeshPNT g_TorusHR;
DxUt::CMeshPNT g_Bowl;
DxUt::CMeshPNT g_Bunny;
DxUt::CMeshPNT g_Bunny_slr;
DxUt::CMeshPNT g_Glass;
DxUt::CMeshPNT g_SmoothCube;
DxUt::CMeshPNT g_BunnyHR;
DxUt::CMeshPNT g_Dragon;
DxUt::CMeshPNT g_Nut;
DxUt::CMeshPNT g_Bolt;
DxUt::CMeshPNT g_Funnel;
DxUt::CMeshPNT g_rgGear[4];
DxUt::CMeshPNT g_rgPuzzlePieces[4];
DxUt::CMeshPNT g_Convex[4];
DxUt::CMeshPNT g_Tread;
DxUt::CMeshPNT g_Wheel;
DxUt::CMeshPNT g_Rod;
DxUt::CMeshPNT g_Plank;
DxUt::CMeshPNT g_Connector;
DxUt::CMeshPNT g_SquareFunnel;
DxUt::CMeshPNT g_Bottle;
DxUt::CMeshPNT g_Cap;
DxUt::CMeshPNT g_Ground;
DxUt::CMeshPNT g_Ramp;

DxUt::CMeshPNT g_Cylinder;
DxUt::CMeshPNT g_FrictionBlock;
DxUt::CMeshPNT g_FrictionBox;

DxUt::CMeshPNT g_Homer;
DxUt::CMeshPNT g_Dilo;
DxUt::CMeshPNT g_Horse;

DWORD g_nLionParts = 10;
DxUt::CMeshPNT g_rgLionPart[20];

DxUt::CArray<DxUt::Vector3F> g_rgGearPos;
DWORD g_dwBallId = 0;

DxUt::CRayTracer g_RayTracer;

DxUt::CPNTPhongFx g_Effect;

DxUt::CCollisionGraphics g_CGraphics;

const UINT g_uiScreenWidth = 800;
const UINT g_uiScreenHeight = 600;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, PSTR line, int show)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(165);

	g_D3DApp = new DxUt::CD3DApp(hInst, L"ContactNormal", L"ContactNormal", 0, 0, g_uiScreenWidth, g_uiScreenHeight, OnWindowResize);

	CreateObj();
	switch (g_Scene) {
		case Scenes::Test_VE:		CreateTestVE(); break;
		case Scenes::Test_FE1:		CreateTestFE1(); break;
		case Scenes::Friction:		CreateFrictionScene(); break;
		case Scenes::Denting:		CreateDentingScene(); break;
		case Scenes::Box_Box:		CreateBoxScene(); break;
		case Scenes::BoxEdge:		CreateBoxEdgeScene(); break;
		case Scenes::EdgeOnBox:		CreateEdgeOnBoxScene(); break;
		case Scenes::Edge_Edge:		CreateEdgeEdgeScene(); break;
		case Scenes::Waffle:		CreateWaffle(); break;
		case Scenes::Bowl:			CreateBowlScene(); break;
		case Scenes::Platform:		CreatePlatformScene(); break;
		case Scenes::BoxStack:		CreateBoxStackScene(); break;
		case Scenes::BoxPyramid:	HAVOK_CONVERT(CreateBoxPyramidScene)(); break;
		case Scenes::BoxOnCorner:	CreateBoxOnCornerScene(); break;
		case Scenes::BunnyScene:	CreateBunnyScene(); break;
		case Scenes::NutBolt:		CreateNutBoltScene(); break;
		case Scenes::Funnel:		CreateFunnelScene(); break;
		case Scenes::Puzzle:		CreatePuzzleScene(); break;
		case Scenes::Gears:			CreateGearsScene(); break;
		case Scenes::Tank:			CreateTankScene(); break;
		case Scenes::Bottle:		CreateBottleScene(); break;
		case Scenes::Lion:			CreateLionScene(); break;
		case Scenes::Chain:			CreateChainScene(); break;
	}
	//CreateGlass();
	CreateBall();
	//g_RayTracer.AddAllRigidBodyWorldMeshes();
	g_CGraphics.CreateGraphics();
	g_CGraphics.SetCamera(&g_Camera);
	g_RBWorld.SetCameraForCollisionGraphics(&g_Camera);

	g_D3DApp->Loop(Render);

	ShutDown();
	g_D3DApp->DestroyD3DApp();
	delete g_D3DApp;

	return 0;
}

void CreateObj()
{ 
	//Create camera
	g_Camera.CreateFPCameraLH(.5*D3DX_PI, g_uiScreenWidth, g_uiScreenHeight, .01f, 100.f, DxUt::Vector3F(.075f, .075f, .075f), .01f);
	g_Camera.SetFPCamera(DxUt::Vector3F(0, 5.f, -5.5f), .5*D3DX_PI, 0);

	//Create the mesh
	g_Box.LoadMeshFromFile("/Box.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_SubdividedBox.LoadMeshFromFile("/subdivided_box.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_BigBox.LoadMeshFromFile("/Box.txt", D3DX10_MESH_32_BIT, Vector3F(4.f));
	g_Waffle.LoadMeshFromFile("/Frame.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Lattice.LoadMeshFromFile("/lattice.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_BoxInflated.LoadMeshFromFile("/Box.txt", D3DX10_MESH_32_BIT, Vector3F(1.1f));
	g_Platform.LoadMeshFromFile("/Platform.txt", D3DX10_MESH_32_BIT, Vector3F(2.f, 2.f, 4.f));
	g_Sphere.LoadMeshFromFile("/sphere_lr.txt", D3DX10_MESH_32_BIT, Vector3F(2.4));
	g_SphereHR.LoadMeshFromFile("/sphere_hr.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Torus.LoadMeshFromFile("/torus.txt", D3DX10_MESH_32_BIT, Vector3F(1.5));
	g_TorusHR.LoadMeshFromFile("/torus_hr.txt", D3DX10_MESH_32_BIT, Vector3F(1.5));
	g_Bowl.LoadMeshFromFile("/bowl.txt", D3DX10_MESH_32_BIT, Vector3F(2.5));
	g_Bunny.LoadMeshFromFile("/bunny.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Bunny_slr.LoadMeshFromFile("/bunny_slr.txt", D3DX10_MESH_32_BIT, Vector3F(.6));
	g_Glass.LoadMeshFromFile("/glass_lr.txt", D3DX10_MESH_32_BIT, Vector3F(4.f));
	g_SmoothCube.LoadMeshFromFile("/smooth_cube.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_BunnyHR.LoadMeshFromFile("/bunny_hr.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Dragon.LoadMeshFromFile("/dragon.txt", D3DX10_MESH_32_BIT, Vector3F(2.));
	g_Nut.LoadMeshFromFile("/nut.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Bolt.LoadMeshFromFile("/bolt.txt", D3DX10_MESH_32_BIT, Vector3F(1.));
	g_Funnel.LoadMeshFromFile("/funnel.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_rgPuzzlePieces[0].LoadMeshFromFile("/puzzle_piece1.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_rgGear[0].LoadMeshFromFile("/gear1.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_rgGear[1].LoadMeshFromFile("/gear2.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Tread.LoadMeshFromFile("/tread.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Wheel.LoadMeshFromFile("/Wheel.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Rod.LoadMeshFromFile("/Rod.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Plank.LoadMeshFromFile("/Plank.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Connector.LoadMeshFromFile("/Connector.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_SquareFunnel.LoadMeshFromFile("/funnel.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Bottle.LoadMeshFromFile("/bottle.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Cap.LoadMeshFromFile("/cap.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Ground.LoadMeshFromFile("/ground.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Ramp.LoadMeshFromFile("/ramp.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_FrictionBlock.LoadMeshFromFile("/friction_block.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_FrictionBox.LoadMeshFromFile("/friction_box.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Homer.LoadMeshFromFile("/homer.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Dilo.LoadMeshFromFile("/dilo.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	g_Horse.LoadMeshFromFile("/horse.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));

	char file[256] = {"/lion_00.txt"};
	for (DWORD i=0; i<g_nLionParts; i++) {
		file[strlen(file)-5] = '0' + i;
		g_rgLionPart[i].LoadMeshFromFile(file, D3DX10_MESH_32_BIT, Vector3F(1.f));
	}
	
	//g_Convex[1].LoadMeshFromFile("C:\\Users\\Aric\\Desktop\\Research\\Convex\\convex_2.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_Convex[2].LoadMeshFromFile("C:\\Users\\Aric\\Desktop\\Research\\Convex\\convex_3.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));
	//g_Convex[3].LoadMeshFromFile("C:\\Users\\Aric\\Desktop\\Research\\Convex\\convex_4.txt", D3DX10_MESH_32_BIT, Vector3F(1.f));


	/*g_RayTracer.CreateRayTracer("/Ouput.png", 860, 640, 1, DIK_9, 100, &g_RBWorld);
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
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.25*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal*rot1;
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(5.7, 3.9, 0);
}

void CreateFrictionScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

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

	std::fstream stream("figure14_scene_info.txt");
	char name[256];
	DWORD dwRod = 0;
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
		if (strstr(name, "Cube") != NULL) {
			g_RBWorld.AddRigidBody(&g_FrictionBox, dwStride, 1.f, mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/friction_box.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		} else if (strstr(name, "Cylinder") != NULL) {
			DWORD dwRB = g_RBWorld.AddRigidBody(&g_Cylinder, dwStride, 1.f, mass, pos, rot, zero,
				Vector3F(0, 0, 0), 1.f, 0.f, zero, Vector3F(0, 0, 0), "/cylinder.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		} else {
			DWORD dwRB = g_RBWorld.AddRigidBody(&g_FrictionBlock, dwStride, 1.f, -mass, pos, rot, zero,
				Vector3F(0, 0, 0), 1.f, 0.f, zero, Vector3F(0, 0, 0), "/friction_block.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		}
	}
}

void CreateDentingScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, 0);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	
	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.4f, .1f, .1f, 1.f);
	mat.dif = D3DXCOLOR(.8f, .1f, .1f, 1.f);

	g_RBWorld.AddRigidBody(&g_SubdividedBox, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/subdivided_box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	
	mat.amb = D3DXCOLOR(.1f, .1f, .2f, 1.f);
	mat.dif = D3DXCOLOR(.2f, .2f, .6f, 1.f);
	
	g_RBWorld.AddRigidBody(&g_SphereHR, dwStride, 1.f, 10.f, Vector3F(0, 5.f, 0), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/sphere_hr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	g_RBWorld.AddRigidBody(&g_SphereHR, dwStride, 1.f, 10.f, Vector3F(.4f, 5.f, .2f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/sphere_hr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	g_RBWorld.AddRigidBody(&g_SphereHR, dwStride, 1.f, 10.f, Vector3F(0, 5.f, .5f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/sphere_hr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	g_RBWorld.AddRigidBody(&g_SphereHR, dwStride, 1.f, 10.f, Vector3F(-.4f, 5.f, -.4f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/sphere_hr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 5, 0);
}

void CreateTestFE1()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.25*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal;
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(5.99, 3.9, 0);//DxUt::Vector3F(5.99, 3.9, 0);
}

void CreateBoxScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_BigBox, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/CubeBig.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	/*g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Cube.lvset", .05, 6, DxUt::CRigidBody::GT_TRIANGLE_MESH);

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
	rotHorizontal.MRotationXLH(-.4*D3DX_PI);
	rot1.MRotationZLH(.25*3.141);
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 2.f, 0.);
	//g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal*rot1;
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 3.45, -1.5 - (1/.707)+.6);*/
}

void CreateModBox()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/modCube.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
}

void CreateEdgeOnBoxScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset",  100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

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

void CreateBoxEdgeScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal;
	//Face-face
	rotHorizontal.MRotationZLH(-.25*D3DX_PI);
	g_RBWorld.GetRigidBody(1)->GetRot() = rotHorizontal;
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.5, 2.45, .5);//-1 - (1/.707)+.6);
	//g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(.9, 1.8, 0);//-1 - (1/.707)+.6);
	g_RBWorld.GetRigidBody(0)->GetPos() = DxUt::Vector3F(.2, 2.6, .01);//-1 - (1/.707)+.6);
	//g_RBWorld.GetRigidBody(1)->GetLinVel() = DxUt::Vector3F(0, 0, -.3f);
	//g_RBWorld.GetRigidBody(1)->GetAngVel() = DxUt::Vector3F(0, 10., 0);
}

void CreateEdgeEdgeScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

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
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(0, 1.45, -1.5 - (1/.707)+1.);//DxUt::Vector3F(-.447, 2.45, -1.5 - (1/.707)+1.);
}

void CreateWaffle()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Lattice, dwStride, 1.f, -10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/lattice.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Lattice, dwStride, 1.f, 10.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/lattice.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot, rot2;
	rot.MRotationXLH(D3DX_PI);
	rot2.MRotationYLH(.5f*D3DX_PI);
	g_RBWorld.GetRigidBody(1)->GetRot() = rot2*rot;
	g_RBWorld.GetRigidBody(1)->GetPos() = Vector3F(0.f, 1.25f, -0.f);
}

void CreateBowlScene()
{
	g_RBWorld.CreateRigidBodyWorld(2, 32);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	
	g_RBWorld.AddRigidBody(&g_Bowl, dwStride, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Bowlll.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	//g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/Platform.lvset", .15, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	//g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/Platform_Big.lvset", .15, 1, DxUt::CRigidBody::GT_OBBOX);

	DxUt::Matrix4x4F rot1;
	rot1.MRotationXLH(1.*3.141);

	// Add spheres
	float fBowlRadius = 5.f;
	float fSphereRadius = 7.8f;
	//float fSphereRadius = 2.4f;
	float mass = 10.f;
	//for (DWORD i=g_RBWorld.GetNumBodies(); i<=g_RBWorld.GetNumBodies()+1; i++) {
	for (DWORD i=g_RBWorld.GetNumBodies(); i<=8; i++) {
		//DxUt::Vector3F pos(1, fSphereRadius*(i+1), 3.f);
		DxUt::Vector3F pos(0.f, -0.f+fSphereRadius*(i+1), 0.f);
		//if ((i)%2) {
			g_RBWorld.AddRigidBody(&g_BunnyHR, dwStride, 1.f, mass, pos, idenity, zero, 
				zero, 1.f, 0.f, zero, zero, "/bunnyHR.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			
			//g_RBWorld.GetRigidBody(i)->GetRot() = rot2*rot1;
			//g_RBWorld.GetRigidBody(i)->GetRot() = rot2;
		//} else {
			//g_RBWorld.AddRigidBody(&g_TorusHR, dwStride, 1.f, mass, pos, idenity, zero, 
			//	zero, 1.f, 0.f, zero, zero, "/torusHR.lvset", .25, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			//g_RBWorld.AddRigidBody(&g_Dragon, dwStride, 1.f, mass, pos, idenity, zero, 
			//	zero, 1.f, 0.f, zero, zero, "/dragon.lvset", .125, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			//g_RBWorld.GetRigidBody(i)->GetRot() = rot1;
		//}
		g_RBWorld.GetRigidBody(i)->GetPos() = pos;
		g_RBWorld.GetRigidBody(i)->GetRot() = rot1;
	}
}

void CreatePlatformScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	DxUt::Matrix4x4F rot1, rot2;
	rot1.MRotationXLH(-.02*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, DxUt::Vector3F(40, 5, 0), rot2*rot1, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, 10.f, DxUt::Vector3F(10, 8.f, 0), rot2*rot1, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	/*DxUt::Matrix4x4F rot1, rot2;
	//Face-face
	//rot2.MRotationYLH(.5*D3DX_PI);
	//rot1.MRotationZLH(.05*3.141);
	g_RBWorld.GetRigidBody(0)->GetRot() = rot1*rot2;
	g_RBWorld.GetRigidBody(0)->GetPos() = DxUt::Vector3F(40, 0, 0);
	g_RBWorld.GetRigidBody(1)->GetRot() = rot1;
	g_RBWorld.GetRigidBody(1)->GetPos() = DxUt::Vector3F(8, 4, 0);*/
}

void CreateBoxPyramidScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8f, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.02f, .0f, .0f, 1.f);
	mat.dif = D3DXCOLOR(.02f, .0f, .0f, 1.f);

	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform_Big.lvset", 100, DxUt::CRigidBody::GT_OBBOX);

	mat.amb = D3DXCOLOR(.1f, .1f, .2f, 1.f);
	mat.dif = D3DXCOLOR(.2f, .2f, .6f, 1.f);

	DxUt::Matrix4x4F rot1, rotHorizontal, rot2;
	//Face-face
	rotHorizontal.MRotationXLH(-.0*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(0)->GetRot() = rot2*rotHorizontal;
	
	srand(0);

	float mass = 1;
	int nBoxesWide = 25;
	int nBoxesHigh = 25;
	float fBoxCenter = 0.f;
	float fBoxWidth = 2.0f;
	float fBoxHeight = 2.01f;
	float fBoxLeft = -nBoxesWide*fBoxWidth/2.f; DWORD idx=1;
	for (int j=0; j<=nBoxesHigh; j++) {
		if (j==0) mass = -10.f;
		else mass = 10.f;
		fBoxLeft = -(nBoxesWide - j)*fBoxWidth/2.f;
		for (int i=0; i<=nBoxesWide - j; i++) {
			DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*(fBoxWidth+.2) + (rand() % 256)*.000, 2 + fBoxHeight*j+.0001, (rand() % 256)*.000 )); 
			//boxPos = Vector3F(0, 4.f, -4.7f);
			//DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*fBoxWidth + .1*(i%2), 2 + fBoxWidth*j, .2f*(j%2))); 
			//DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*fBoxWidth, 2 + fBoxWidth*j, 0)); 
			g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, mass, boxPos, idenity, zero,
				zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);

			//g_RBWorld.GetRigidBody(1+i*(j+1))->GetPos() = DxUt::Vector3F(fBoxLeft + i*fBoxWidth, 2 + fBoxWidth*j, 0);
			//g_RBWorld.GetRigidBody(1+i*(j+1))->GetLinVel() = DxUt::Vector3F(0, 0, 1.f);
			//g_RBWorld.GetRigidBody(idx++)->GetAngVel() = DxUt::Vector3F(0, 1.f, 0);
		}
	}
}

#ifdef HAVOK_GO

hkpWorld * world;
hkpRigidBody** rigidBodies;
 
void CreateBoxPyramidScene_Havok()
{
	CreateBoxPyramidScene();

	// Perfrom platform specific initialization for this demo - you should already have something similar in your own code.
	PlatformInit();

	// Need to have memory allocated for the solver. Allocate 1mb for it.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator,
		hkMemorySystem::FrameInfo(1024*10*1024) );
	hkBaseSystem::init( memoryRouter, errorReport );

	{
		hkpWorldCinfo info;

		info.m_gravity.set(0.0f, -9.8f, 0.0f);
		info.m_enableDeactivation = false;

		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_SOFT);
		info.m_contactPointGeneration = info.CONTACT_POINT_REJECT_MANY;
		info.m_collisionTolerance = 0.007f;
		info.setBroadPhaseWorldSize( 400.0f );
		info.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE;

		info.m_contactRestingVelocity = 0;
		//info.m_enableDeactivation = 0;
		info.m_solverIterations = 30;

		world = new hkpWorld( info );
		world->lock();
		// Register all collision agents
		// It's important to register collision agents before adding any entities to the world.
		hkpAgentRegisterUtil::registerAllAgents( world->getCollisionDispatcher() );

		{
			hkVector4 baseSize( 150.0f, 1.f, 150.0f);
			hkpConvexShape* shape = new hkpBoxShape( baseSize , 0 );

			hkpRigidBodyCinfo ci;

			ci.m_shape = shape;
			ci.m_restitution = 0.5f;
			ci.m_friction = 0.3f;
			ci.m_position.set( 0.0f, -.0f, 0.0f );
			ci.m_motionType = hkpMotion::MOTION_FIXED;

			world->addEntity( new hkpRigidBody( ci ) )->removeReference();
			shape->removeReference();
		}

		int nBodies = g_RBWorld.GetNumBodies();
		rigidBodies = new hkpRigidBody*[nBodies+10];
		{
			// Create a box 1 by 2 by 3
			hkVector4 halfExtents; halfExtents.set(1.f, 1.f, 1.f);
			hkpBoxShape* boxShape = new hkpBoxShape(halfExtents);

			hkpRigidBodyCinfo bodyCinfo;
			bodyCinfo.m_shape = boxShape;

			//  Calculate the mass properties for the shape
			const hkReal boxMass = 10.0f;
			hkpMassProperties massProperties;
			hkpInertiaTensorComputer::computeShapeVolumeMassProperties(boxShape, boxMass, massProperties);

			bodyCinfo.setMassProperties(massProperties);

			// Add the boxes
			for (int i=1; i<nBodies; i++) {
				const Vector3F & pos = g_RBWorld.GetRigidBody(i)->GetPos();
				bodyCinfo.m_position = hkVector4(pos.x, pos.y, pos.z, i);

				// Create the rigid body
				rigidBodies[i-1] = new hkpRigidBody(bodyCinfo);
				world->addEntity(rigidBodies[i-1]);
				rigidBodies[i-1]->removeReference();

				//hkpRigidBody * rigidBody = new hkpRigidBody(bodyCinfo);
				//world->addEntity(rigidBody);
				//rigidBody->removeReference();

			}
			boxShape->removeReference();
		}
	}
	world->unlock();
}

#endif 

void CreateBoxStackScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	//g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/Platform_Big.lvset", .25, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal, rot2, rot3;
	//Face-face
	rotHorizontal.MRotationXLH(-.0*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	rot1.MRotationZLH(.25*D3DX_PI);
	rot3.MRotationXLH(.25*D3DX_PI);
	//g_RBWorld.GetRigidBody(0)->GetRot() = rot2*rotHorizontal;
	
	srand(0);

	int nBoxesHigh = 2;
	float fBoxCenter = 0.f;
	float fBoxWidth = 2.2f;
	for (int j=0; j<nBoxesHigh; j++) {
		//DxUt::Vector3F boxPos(DxUt::Vector3F((rand() % 256)*.001, 2 + fBoxWidth*j, (rand() % 256)*.001)); 
		//DxUt::Vector3F boxPos(DxUt::Vector3F(0, 6 + fBoxWidth*j, 0)); 
		DxUt::Vector3F boxPos(DxUt::Vector3F(0, fBoxWidth*j, 0)); 
		float mass = 1.f;
		if (j==0)
			mass = -1.f;
		/*if (j % 2) {//mass = -1.f;
			boxPos.x = -1.1;
			boxPos.z = -1.1;
		}*/
		if (j%2) {
			g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, mass, boxPos, idenity, zero,
				zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			//g_RBWorld.GetRigidBody(j+1)->GetAngVel() = DxUt::Vector3F(0, .8f, 0);
			//g_RBWorld.GetRigidBody(j+1)->GetRot() = rot1*rot3;
			//g_RBWorld.GetRigidBody(j+1)->GetPos() += DxUt::Vector3F(0, 0, .001);
		} else {
			g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, mass, boxPos, idenity, zero,
				zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
		}
		//g_RBWorld.GetRigidBody(j+1)->GetLinVel() = DxUt::Vector3F(-.4, 0, 0);
		//g_RBWorld.GetRigidBody(j+1)->GetAngVel() = DxUt::Vector3F(0, .5f, 0);
		//g_RBWorld.GetRigidBody(j+1)->GetPos() = DxUt::Vector3F(-10.f, 1.45f, 3.f);
		//g_RBWorld.GetRigidBody(j+1)->GetRot() = rot1;
	}
}

void CreateBoxOnCornerScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 32);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

	DxUt::Matrix4x4F rot1, rotHorizontal, rot2;
	//Face-face
	rotHorizontal.MRotationXLH(-.0*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	rot1.MRotationZLH(.04*3.141);
	g_RBWorld.GetRigidBody(0)->GetRot() = rot2*rotHorizontal;
	
	srand(0);

	float mass = 10.f;
	float fBoxWidth = 2.2f;
	//DxUt::Vector3F boxPos(DxUt::Vector3F((rand() % 256)*.001, 2 + fBoxWidth*j, (rand() % 256)*.001)); 
	DxUt::Vector3F boxPos(DxUt::Vector3F(-9.12, 1.3, 2.5)); 
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, mass, boxPos, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	//g_RBWorld.GetRigidBody(j+1)->GetLinVel() = DxUt::Vector3F(-.6f, 0, .6f);
	//g_RBWorld.GetRigidBody(j+1)->GetAngVel() = DxUt::Vector3F(0, .5f, 0);
	//g_RBWorld.GetRigidBody(j+1)->GetPos() = DxUt::Vector3F(-10.f, 1.45f, 3.f);
	g_RBWorld.GetRigidBody(1)->GetRot() = rot1;
}

void CreateGlass()
{
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F pos(-20, 10, 0), zero(0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	//g_dwBallId = g_RBWorld.AddRigidBody(&g_Sphere, dwStride, 1.f, 100.f, pos, idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/sphere_lr.lvset", .05, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	//g_dwBallId = g_RBWorld.AddRigidBody(&g_Glass, dwStride, 1.f, 100.f, pos, idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/glass.lvset", .05, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);
}

void CreateBall()
{
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F pos(0, 0, -1000), zero(0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);
	g_dwBallId = g_RBWorld.AddRigidBody(&g_SphereHR, dwStride, 1.f, 1.f, pos, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/sphere_hr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
	//g_dwBallId = g_RBWorld.AddRigidBody(&g_Bunny, dwStride, 1.f, 100.f, pos, idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/bunny.lvset", .15, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH);
}

void CreateBunnyScene()
{
	g_RBWorld.CreateRigidBodyWorld(2, 32);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

	float fScaling = -12.f;
	float fOffsetX = 12.f;
	DxUt::Matrix4x4F glassRot;
	glassRot.MRotationYLH(.6*3.141);
	float fVerticalAscension = 4.f;
	for (DWORD i=0; i<8; i++) {
		for (DWORD j=0; j<3; j++) {
			float elevation = sqrtf((i+1)*(i+1) + (2+2*j)*(2+2*j));
			g_RBWorld.AddRigidBody(&g_Glass, dwStride, 1.f, -200.f, DxUt::Vector3F((float)i*fScaling + fOffsetX, -1.5*10.5 + fVerticalAscension*elevation, 24*j), glassRot, zero,
				zero, 1.f, 0.f, zero, zero, "/glass.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			DxUt::SSurfaceFresnel * desc =  new DxUt::SSurfaceFresnel;
			desc->amb = DxUt::Vector3F(0);
			if (j==0)
				desc->n = DxUt::Vector4F(.5f, .5f, 1.f, 1.3);
			else if (j == 1)
				desc->n = DxUt::Vector4F(1.f, 1.f, .3f, 1.2);
			else if (j == 2)
				desc->n = DxUt::Vector4F(1.f, .4f, .4f, 1.3);
			desc->bInterpolateNormal = 1;
			g_RayTracer.AddMesh(g_RBWorld.GetNumBodies()-1, desc);
		}
	}

	DxUt::Matrix4x4F platformRot1;
	platformRot1.MRotationYLH(.5*3.141);
	DxUt::Matrix4x4F platformRot2;
	platformRot2.MRotationXLH(.5*3.141);
	DxUt::Matrix4x4F platformRot3;
	platformRot3.MRotationZLH(.0*3.141);
	
	int nCubes = 11;
	float fCubeSize = 8.f;
	float fCubeOffset = - (6/2) * fCubeSize;
	g_PlatformLong.LoadMeshFromFile("/Box.txt", D3DX10_MESH_32_BIT, Vector3F(fCubeSize-2, fCubeSize, fCubeSize-2));
	fCubeSize *= 2;
	for (int i=0; i<nCubes*nCubes; i++) {
		float elevation = sqrtf((i%nCubes)*(i%nCubes) + (i/nCubes)*(i/nCubes));
		g_RBWorld.AddRigidBody(&g_PlatformLong, dwStride, 1.f, -200.f, 
			DxUt::Vector3F(-fCubeOffset - (i/nCubes)*(fCubeSize-4), -1.5f*fCubeSize + fVerticalAscension*elevation, fCubeOffset + (i%nCubes)*(fCubeSize-4)),
			idenity, zero, zero, 1.f, 0.f, zero, zero, "/Cube_big.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

		DxUt::SSurfaceLambert * desc =  new DxUt::SSurfaceLambert;
		desc->amb = DxUt::Vector3F(0, 0, 0);
		desc->dif = DxUt::Vector3F(1.f, 1.f, 1.f);
		desc->spe = DxUt::Vector3F(1.f, 1.f, 1.f);
		if (i%2) {
			desc->dif *= 0.f;
			//desc->spe *= 0;
		}
		desc->sPow = 60.f;
		desc->bInterpolateNormal = 1;
		g_RayTracer.AddMesh(g_RBWorld.GetNumBodies()-1, desc);
	}
	
	float fBunnyOffset = 10.f;
	float fBunnySpacing = 6.0f;
	for (DWORD j=0; j<0; j++) {
		for (DWORD i=0; i<5; i++) {
			for (DWORD k=0; k<6; k++) {
				float elevation = sqrtf((i+1)*(i+1) + (2+2*j)*(2+2*j));
				g_RBWorld.AddRigidBody(&g_Bunny, dwStride, 1.f, 1.f, DxUt::Vector3F((float)i*fScaling + fOffsetX, -1.5*10.5 + 6*elevation + fBunnyOffset + k*fBunnySpacing, j*24),
					idenity, zero, zero, 1.f, 0.f, zero, zero, "/bunny.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH);

				DxUt::SSurfaceFresnel * desc =  new DxUt::SSurfaceFresnel;
				desc->amb = DxUt::Vector3F(0);
				//switch (i%3) {
					desc->n = DxUt::Vector4F(.01f*(rand()%100), .01f*(rand()%100), .01f*(rand()%100), 1.3);
				/*case 0:
					desc->n = DxUt::Vector4F(.5f, .5f, 1.f, 1.3); break;
				case 1:
					desc->n = DxUt::Vector4F(1.f, .5f, .5f, 1.3); break;
				case 2:
					desc->n = DxUt::Vector4F(.5f, 1.f, .5f, 1.3);
				}*/
				desc->bInterpolateNormal = 1;
				g_RayTracer.AddMesh(g_RBWorld.GetNumBodies()-1, desc);
			}
		}
	}
}

void CreateNutBoltScene()
{
	g_RBWorld.CreateRigidBodyWorld(2, 32);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

	DxUt::Matrix4x4F rot;
	rot.MRotationZLH(0.*3.141);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.4f, .4f, .4f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.6f, .3f, .3f, 1.f);
	mat.dif = D3DXCOLOR(1.f, .6f, .6f, 1.f);
	
	g_RBWorld.AddRigidBody(&g_Nut, dwStride, 1.f, 1.f, Vector3F(0, 0.f, 0), rot, zero,
		Vector3F(0, 1.1f, 0), 1.f, 0.f, zero, zero, "/nut.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	mat.amb = D3DXCOLOR(.3f, .3f, .6f, 1.f);
	mat.dif = D3DXCOLOR(.6f, .6f, 1.f, 1.f);

	g_RBWorld.AddRigidBody(&g_Bolt, dwStride, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/bolt.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
}

void CreateFunnelScene()
{
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet, &Vector3F(0, -3.8f, 0), .03, 14.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

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
	DWORD dwTriPerVolume;
	int nObjects;
	file >> nObjects;
	file >> dwTriPerVolume;
	file.close();
	
	g_RBWorld.AddRigidBody(&g_Funnel, dwStride, 1.f, -1.f, zero, rot, zero,
		zero, 1.f, 0.f, zero, zero, "/funnel.lvset", dwTriPerVolume, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	nPoly += g_Funnel->GetFaceCount();

	g_RBWorld.AddRigidBody(&g_Sphere, dwStride, 1.f, -1.f, Vector3F(0, -14.f, 0), rot, zero,
				zero, 1.f, 0.f, zero, zero, "/sphere_lr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	nPoly += g_Sphere->GetFaceCount();

	//g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(0, -10, 0), idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/Box.lvset", .05, 1, DxUt::CRigidBody::GT_OBBOX, &mat);

	/*mat.amb = D3DXCOLOR((rand() % 255)/255.f, (rand() % 255)/255.f, (rand() % 255)/255.f, 1.f);//D3DXCOLOR(.6f, .3f, .3f, 1.f);
	g_RBWorld.AddRigidBody(&g_Bolt, dwStride, 1.f, 1.f, DxUt::Vector3F(-6,6, -.5), rot2, zero,
				zero, 1.f, 0.f, zero, zero, "/bolt.lvset", .08, 1, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);*/
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
			g_RBWorld.AddRigidBody(&g_Homer, dwStride, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/homer.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Homer->GetFaceCount();
			break;
		case 1:
			g_RBWorld.AddRigidBody(&g_Dilo, dwStride, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/dilo.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Dilo->GetFaceCount();
			break;
		case 2:
			g_RBWorld.AddRigidBody(&g_Horse, dwStride, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/horse.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Horse->GetFaceCount();
			break;
		case 3:
			g_RBWorld.AddRigidBody(&g_Sphere, dwStride, 1.f, 1.f, objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/sphere_lr.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Sphere->GetFaceCount();
			break; 
		case 4:
			g_RBWorld.AddRigidBody(&g_Bunny, dwStride, 1.f, 1.f, -objPos+offset, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/bunny.lvset", dwTriPerVolume, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
			nPoly += g_Bunny->GetFaceCount();
			break;
		case 5:
			g_RBWorld.AddRigidBody(&g_BunnyHR, dwStride, 1.f, 1.,  -objPos+offset, idenity, zero, 
				zero, 1.f, 0.f, zero, zero, "/bunny_hr.lvset", dwTriPerVolume, DxUt::CRigidBody::GT_TRIANGLE_MESH);
			nPoly += g_BunnyHR->GetFaceCount();
			break;
		}
			
	}
	g_D3DApp->Print((int)nPoly);
}

void CreatePuzzleScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8f, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.02f, .0f, .0f, 1.f);
	mat.dif = D3DXCOLOR(.02f, .0f, .0f, 1.f);

	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform_Big.lvset", 100, DxUt::CRigidBody::GT_OBBOX);

	mat.amb = D3DXCOLOR(.1f, .1f, .2f, 1.f);
	mat.dif = D3DXCOLOR(.2f, .2f, .6f, 1.f);

	DxUt::Matrix4x4F rot1, rotHorizontal, rot2;
	//Face-face
	rotHorizontal.MRotationXLH(-.0*D3DX_PI);
	rot2.MRotationYLH(.5*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);
	g_RBWorld.GetRigidBody(0)->GetRot() = rot2*rotHorizontal;
	rot2.MRotationZLH(.12*D3DX_PI);

	srand(0);

	float mass = 1;
	int nBoxesWide = 3;
	int nBoxesHigh = 3;
	float fBoxCenter = 0.f;
	float fBoxWidth = 6.01f;
	float fBoxHeight = 6.01f;
	float fBoxLeft = -nBoxesWide*fBoxWidth/2.f; DWORD idx=1;
	for (int j=0; j<=nBoxesHigh; j++) {
		//if (j==0) mass = -10.f;
		//else mass = 10.f;
		fBoxLeft = -(nBoxesWide)*fBoxWidth/2.f;
		for (int i=0; i<=nBoxesWide; i++) {
			DxUt::Vector3F boxPos(DxUt::Vector3F(fBoxLeft + i*(fBoxWidth) + (rand() % 256)*.000, 6 + fBoxHeight*j, (rand() % 256)*.000 )); 

			g_RBWorld.AddRigidBody(&g_rgPuzzlePieces[0], dwStride, 1.f, mass, boxPos, idenity, zero,
				zero, 1.f, 0.f, zero, zero, "/puzzle_piece1.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		}
	}
}

void CreateGearsScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

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
	
	float mass = 1.f;
	g_RBWorld.AddRigidBody(&g_rgGear[0], dwStride, 1.f, -mass, Vector3F(0), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/gear1.lvset", 100, DxUt::CRigidBody::GT_OBBOX);
	DxUt::Matrix4x4F iBody(g_RBWorld.GetRigidBody(0)->GetIBody());
	iBody.m[0][0] = iBody.m[1][0] = iBody.m[2][0] = iBody.m[3][0] = 0;
	iBody.m[0][2] = iBody.m[1][1] = iBody.m[2][1] = iBody.m[3][1] = 0;
	iBody.m[0][0] = 100000; iBody.m[1][1] = 100000;
	g_RBWorld.GetRigidBody(0)->SetIBody(iBody);
	g_RBWorld.GetRigidBody(0)->GetAngVel() = Vector3F(0, 0, .15f);
	g_RBWorld.AddCenterOfMassPositionConstraint(0, Vector3F(0));

	g_RBWorld.AddRigidBody(&g_rgGear[1], dwStride, 1.f, mass, Vector3F(-8.8, 0, 0), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/gear2.lvset", 100, DxUt::CRigidBody::GT_OBBOX);
	DxUt::Matrix4x4F iBody2(g_RBWorld.GetRigidBody(1)->GetIBody());
	iBody2.m[0][1] = 0;
	iBody2.m[0][0] = iBody2.m[1][0] = iBody2.m[2][0] = iBody2.m[3][0] = 0;
	iBody2.m[0][2] = iBody2.m[1][1] = iBody2.m[2][1] = iBody2.m[3][1] = 0;
	iBody2.m[0][0] = 100000; iBody2.m[1][1] = 100000;
	g_RBWorld.GetRigidBody(1)->SetIBody(iBody2);
	g_RBWorld.AddCenterOfMassPositionConstraint(1, g_RBWorld.GetRigidBody(1)->GetPos());
}

DWORD g_dwRod[2];

void CreateTankScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

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

	std::fstream stream("tank_scene_info.txt");
	char name[256];
	DWORD dwRod = 0;
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
		if (strstr(name, "Cube") != NULL) {
			/*g_RBWorld.AddRigidBody(&g_Tread, dwStride, 1.f, .1f*mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/tread.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);

			g_RBWorld.AddRigidBody(&g_Tread, dwStride, 1.f, .1f*mass, pos+Vector3F(0, 0, offset), rot, zero,
				zero, 1.f, 0.f, zero, zero, "/tread.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);*/
		} else if (strstr(name, "Wheel") != NULL) {
			DWORD dwRB = g_RBWorld.AddRigidBody(&g_Wheel, dwStride, 1.f, mass, pos, rot, zero,
				Vector3F(0, 0, 0), 1.f, 0.f, zero, Vector3F(0, 0, 0), "/wheel.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
			dwRB = g_RBWorld.AddRigidBody(&g_Wheel, dwStride, 1.f, mass, pos+Vector3F(0, 0, offset), rot, zero,
				Vector3F(0, 0, 0), 1.f, 0.f, zero, Vector3F(0, 0, 0), "/wheel.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
			/*DxUt::Matrix4x4F iBody2(g_RBWorld.GetRigidBody(dwRB)->GetIBody());
			iBody2.m[0][1] = 0;
			iBody2.m[0][0] = iBody2.m[1][0] = iBody2.m[2][0] = iBody2.m[3][0] = 0;
			iBody2.m[0][2] = iBody2.m[1][1] = iBody2.m[2][1] = iBody2.m[3][1] = 0;
			iBody2.m[0][0] = 100000; iBody2.m[1][1] = 100000; iBody2.m[2][2] = 10.f;
			g_RBWorld.GetRigidBody(dwRB)->SetIBody(iBody2);
			g_RBWorld.AddCenterOfMassPositionConstraint(dwRB, pos);*/

			/*DWORD dwRB = g_RBWorld.AddRigidBody(&g_Wheel, dwStride, 1.f, -mass, pos, rot, zero,
				Vector3F(0, 0, 1.f), 1.f, 0.f, zero, Vector3F(0, 0, 0), "/wheel.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
			DxUt::Matrix4x4F iBody2(g_RBWorld.GetRigidBody(dwRB)->GetIBody());
			iBody2.m[0][1] = 0;
			iBody2.m[0][0] = iBody2.m[1][0] = iBody2.m[2][0] = iBody2.m[3][0] = 0;
			iBody2.m[0][2] = iBody2.m[1][1] = iBody2.m[2][1] = iBody2.m[3][1] = 0;
			iBody2.m[0][0] = 100000; iBody2.m[1][1] = 100000; iBody2.m[2][2] = 10.f;
			g_RBWorld.GetRigidBody(dwRB)->SetIBody(iBody2);
			g_RBWorld.AddCenterOfMassPositionConstraint(dwRB, pos);*/
		} else if (strstr(name, "Rod") != NULL) {
			DWORD dwRB = g_RBWorld.AddRigidBody(&g_Rod, dwStride, 1.f, mass, pos, rot, zero,
			Vector3F(0, 0, 0), 1.f, 0.f, zero, Vector3F(0, 0, 0), "/rod.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
			g_dwRod[dwRod++] = dwRB;
			//DxUt::Matrix4x4F iBody2(g_RBWorld.GetRigidBody(dwRB)->GetIBody());
			/*iBody2.m[0][1] = 0;
			iBody2.m[0][0] = iBody2.m[1][0] = iBody2.m[2][0] = iBody2.m[3][0] = 0;
			iBody2.m[0][2] = iBody2.m[1][1] = iBody2.m[2][1] = iBody2.m[3][1] = 0;
			iBody2.m[0][0] = 100000; iBody2.m[1][1] = 100000; iBody2.m[2][2] = 10.f;*/
			//g_RBWorld.GetRigidBody(dwRB)->SetIBody(iBody2);
			//g_RBWorld.AddCenterOfMassPositionConstraint(dwRB, pos);
		} else if(strstr(name, "Plank") != NULL) {
			DxUt::SMaterial matCopy = mat;
			matCopy.amb = D3DXCOLOR(.7f, .0f, .0f, 1.f);
			matCopy.dif = D3DXCOLOR(.7f, .0f, .0f, 1.f);
			g_RBWorld.AddRigidBody(&g_Plank, dwStride, 1.f, -mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/plank.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &matCopy);
		} else if(strstr(name, "Ground") != NULL) {
			DxUt::SMaterial matCopy = mat;
			matCopy.amb = D3DXCOLOR(.7f, .0f, .0f, 1.f);
			matCopy.dif = D3DXCOLOR(.7f, .0f, .0f, 1.f);
			g_RBWorld.AddRigidBody(&g_Ground, dwStride, 1.f, -mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/ground.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &matCopy);
		} else if (strstr(name, "Connector") != NULL) {
			g_RBWorld.AddRigidBody(&g_Connector, dwStride, 1.f, mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/connector.lvset", 100, DxUt::CRigidBody::GT_OBBOX);
			g_RBWorld.AddRigidBody(&g_Connector, dwStride, 1.f, mass, pos+Vector3F(0, 0, 4.f), rot, zero,
				zero, 1.f, 0.f, zero, zero, "/connector.lvset", 100, DxUt::CRigidBody::GT_OBBOX);
		} else if  (strstr(name, "Ramp") != NULL) {
			g_RBWorld.AddRigidBody(&g_Ramp, dwStride, 1.f, -mass, pos, rot, zero,
				zero, 1.f, 0.f, zero, zero, "/ramp.lvset", 100, DxUt::CRigidBody::GT_OBBOX);
		}
	}

	float height  = 14.f;
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(90.f, -height, 0.f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);	
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(90.f, -height, 16.f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(105.f, -height, 0.f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);	
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(105.f, -height, 16.f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(115.f, -height, 0.f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);	
	g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(115.f, -height, 16.f), idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/box.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
}

void CreateBottleScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

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
	rot2.MRotationXLH(-.5*D3DX_PI);
	rot1.MRotationYLH(.25*3.141);

	float mass  = 1.f;
	g_RBWorld.AddRigidBody(&g_SquareFunnel, dwStride, 1.f, -mass, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/funnel.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);

	DWORD sizeX = 1;
	DWORD sizeZ = 1;
	float width = 6.f;
	float startX = -.5f*sizeX*width;
	float startZ = -.5f*sizeZ*width;
	Vector3F capOffset(0, 8.2f, 0);
	for (DWORD i=0; i<sizeX; i++) {
		for (DWORD j=0; j<sizeZ; j++) {
			mat.amb = D3DXCOLOR((rand() % 255)/255.f, (rand() % 255)/255.f, (rand() % 255)/255.f, 1.f);//D3DXCOLOR(.6f, .3f, .3f, 1.f);
			mat.dif = 2.f*mat.amb;
			Vector3F pos(i*width + startX, 30.f, j*width + startZ);

			g_RBWorld.AddRigidBody(&g_Bottle, dwStride, 1.f, mass, pos, rot2, zero,
				zero, 1.f, 0.f, zero, zero, "/bottle.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
			
			g_RBWorld.AddRigidBody(&g_Cap, dwStride, 1.f, mass, pos+capOffset, rot2, zero,
				zero, 1.f, 0.f, zero, zero, "/cap.lvset", 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		}
	}
}

void CreateLionScene()
{
	//Create the RBWorld
	g_RBWorld.CreateRigidBodyWorld(2, 10000, g_bUseHierarchicalLevelSet, &DxUt::Vector3F(0, -3.8f, 0), .03f, 20.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.5f, .5f, .5f, 1.f);
	mat.pow = 16.f;

	mat.amb = D3DXCOLOR(.8f, .0f, .0f, 1.f);
	mat.dif = D3DXCOLOR(.8f, .0f, .0f, 1.f);

	g_RBWorld.AddRigidBody(&g_Platform, dwStride, 1.f, -200.f, zero, idenity, zero,
		zero, 1.f, 0.f, zero, zero, "/Platform_Big.lvset", 100, DxUt::CRigidBody::GT_OBBOX);

	mat.amb = D3DXCOLOR(.0f, .0f, .8f, 1.f);
	mat.dif = D3DXCOLOR(.0f, .0f, .8f, 1.f);

	std::fstream stream("lion_scene_info.txt");
	char name[256];
	DWORD dwRod = 0;
	DWORD i=0;
	char file[256] = {"/lion_00.lvset"};
	while ((stream >> name)) {
		float mass = 1.f;
		Vector3F pos;
		stream >> pos.x;
		stream >> pos.y;
		stream >> pos.z;
		D3DXQUATERNION quat;
		stream >> quat.x;
		stream >> quat.z;
		stream >> quat.y;
		stream >> quat.w;
		DxUt::Matrix4x4F rot;
		D3DXMatrixRotationQuaternion((D3DXMATRIX*)&rot, &quat);
		float offset = 16.f;

		file[strlen(file)-7] = '0' + i;
		g_RBWorld.AddRigidBody(&g_rgLionPart[i], dwStride, 1.f, 1.f, pos, idenity, zero,
			zero, 1.f, 0.f, zero, zero, file, 100, DxUt::CRigidBody::GT_OBBOX, &mat);
		i++;
	}
}

void CreateChainScene()
{
	g_RBWorld.CreateRigidBodyWorld(2, 32, g_bUseHierarchicalLevelSet, &Vector3F(0, -3.8f, 0), .03, 14.f);
	DxUt::Matrix4x4F idenity; idenity.MIdenity();
	DxUt::Vector3F zero(0, 0, 0);
	DWORD dwStride = sizeof(DxUt::SVertexPNT);

	DxUt::Matrix4x4F rot[2];
	rot[0].MRotationXLH(.5*3.141);
	rot[1].MRotationZLH(.0*3.141);

	DxUt::SMaterial mat;
	mat.spe = D3DXCOLOR(.8f, .8f, .8f, 1.f);
	mat.pow = 16.f;
	
	mat.amb = D3DXCOLOR(.6f, .1f, .1f, 1.f);
	mat.dif = D3DXCOLOR(.75f, .1f, .1f, 1.f);
	
	// Torus chain
	int nTorusChain = 12;
	int nTorusRing = 6;
	float angleOffset = 6.28f/nTorusRing;
	Vector3F center(0,0,0);
	Vector3F offset(-2.1f, 0, 0);
		//srand(984);
	for (int i=0; i<nTorusRing; i++) {
		DxUt::Matrix4x4F angleRot;
		angleRot.MRotationYLH(i*angleOffset);
		for (int j=1; j<nTorusChain-1; j++) {
			//mat.amb = D3DXCOLOR((rand() % 255)/255.f, (rand() % 255)/255.f, (rand() % 255)/255.f, 1.f);//D3DXCOLOR(.6f, .3f, .3f, 1.f);
			//mat.dif = 2.f*mat.amb;

			g_RBWorld.AddRigidBody(&g_Torus, dwStride, 1.f, 1.f, angleRot*(center + j*offset), angleRot*rot[(j+1)%2], zero,
				zero, 1.f, 0.f, zero, zero, "/torus.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
		}
		g_RBWorld.AddRigidBody(&g_Torus, dwStride, 1.f, -1.f, angleRot*(center + (nTorusChain-1)*offset), angleRot*rot[(nTorusChain)%2], zero,
			zero, 1.f, 0.f, zero, zero, "/torus.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);
	}
	g_RBWorld.AddRigidBody(&g_Torus, dwStride, 1.f, 1.f, center, rot[1], zero,
		zero, 1.f, 0.f, zero, zero, "/torus.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	mat.amb = D3DXCOLOR(.1f, .1f, .6f, 1.f);
	mat.dif = D3DXCOLOR(.1f, .1f, .6f, 1.f);

	g_RBWorld.AddRigidBody(&g_Sphere, dwStride, 1.f, 1.f, center+ Vector3F(5, 5.f, 0), rot[1], zero,
		zero, 1.f, 0.f, zero, zero, "/sphere.lvset", 100, DxUt::CRigidBody::GT_TRIANGLE_MESH, &mat);

	//g_RBWorld.AddRigidBody(&g_Box, dwStride, 1.f, -1.f, Vector3F(0, -10, 0), idenity, zero,
	//	zero, 1.f, 0.f, zero, zero, "/Box.lvset", .05, 1, DxUt::CRigidBody::GT_OBBOX, &mat);
}

void Render()
{
	//float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	//float color[] = {0.2f, 0.2f, .5f, 1.0f};
	float color[] = {1.f, 1.f, 1.f, 1.0f};
	//float color[] = {.5f, 0.4f, 0.9f, 1.0f};
	g_pD3DDevice->ClearRenderTargetView(g_pRenderTargetView, color);
	g_pD3DDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

#ifndef HAVOK_GO
	//static int counter = 0;
	//if (counter < 18) {
	//	g_RBWorld.UpdateRigidBodies(.03, DxUt::Vector3F(0, -3.8f, 0));
	//} counter++;
	static int count = 0;
	float timeStep = .01f;
	//if (count++ > 100) timeStep = .01f;
	if (DxUt::g_MouseState.rgbButtons[1]) {
		if (g_Scene == Gears) g_RBWorld.UpdateRigidBodies(.03, DxUt::Vector3F(0, -4.8, 0));
		else g_RBWorld.UpdateRigidBodies(timeStep, DxUt::Vector3F(0, -4.8, 0));
		//Sleep(100);
	}
	//else g_RBWorld.UpdateRigidBodies(0, DxUt::Vector3F(0, -3.8f, 0));
	
	if (DxUt::g_KeysState[DIK_G]) {
		g_bDrawCollisionGraphics = !g_bDrawCollisionGraphics;
		Sleep(100);
	}
	if (g_bDrawCollisionGraphics)
		g_RBWorld.DrawCollisionGraphics(&g_Camera);
#else
	// Update if running at 60 frames per second.
	const hkReal updateFrequency = 60.0f;
	world->stepDeltaTime(1.0f / updateFrequency);
	for (int i=1,end=g_RBWorld.GetNumBodies()-1; i<end; i++) {
		g_RBWorld.GetRigidBody(i)->GetPos() = *((DxUt::Vector3F*)&rigidBodies[i-1]->getPosition());
		hkRotation rot;
		rot.set(rigidBodies[i-1]->getRotation());
		rot.getColumn(0);
		DxUt::Matrix4x4F & _rot = g_RBWorld.GetRigidBody(i)->GetRot();
		for (int i=0; i<3; i++) for (int j=0; j<3; j++) _rot.m[i][j] = rot(i,j);
		//g_RBWorld.GetRigidBody(i)->GetRot() = *((DxUt::Vector3F*)&rigidBodies[i-1]->getRotation().);
	}
#endif

	static bool once = 1;
	if (!once || DxUt::g_KeysState[DIK_SPACE]) {
		//g_RBWorld.GetRigidBody(g_dwBallId)->GetPos() = Vector3F(0, 6.f, 0);
		//g_RBWorld.GetRigidBody(g_dwBallId)->GetLinVel() = Vector3F(0, 0.f, 0);
		g_RBWorld.GetRigidBody(g_dwBallId)->GetPos() = g_Camera.GetPosition();
		g_RBWorld.GetRigidBody(g_dwBallId)->GetLinVel() = 6.f*g_Camera.GetForwardVector();once = 1;
	}

	g_Camera.UpdateFPCamera(50.f*DxUt::g_SPFrame);

	g_RBWorld.DrawRigidBodies(&g_Camera, g_Light, 0);

	if (g_Scene == Gears) {
		if (DxUt::g_KeysState[DIK_A]) {
			g_RBWorld.GetRigidBody(0)->GetAngVel() += Vector3F(0, 0, .01f);
		}
		if (DxUt::g_KeysState[DIK_D]) {
			g_RBWorld.GetRigidBody(0)->GetAngVel() -= Vector3F(0, 0, .01f);
		}
	}

	if (g_Scene == Tank) {
		if (DxUt::g_KeysState[DIK_A]) {
			DxUt::Matrix4x4F rot = g_RBWorld.GetRigidBody(g_dwRod[0])->GetRot();
			Vector3F v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_dwRod[0])->GetAngVel() += 10.f*v3;
			rot = g_RBWorld.GetRigidBody(g_dwRod[1])->GetRot();
			v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_dwRod[1])->GetAngVel() += -10.f*v3;
		}
		if (DxUt::g_KeysState[DIK_D]) {
			DxUt::Matrix4x4F rot = g_RBWorld.GetRigidBody(g_dwRod[0])->GetRot();
			Vector3F v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_dwRod[0])->GetAngVel() += -10.f*v3;
			rot = g_RBWorld.GetRigidBody(g_dwRod[1])->GetRot();
			v3 = rot.GetColumnVec3F(2);
			g_RBWorld.GetRigidBody(g_dwRod[1])->GetAngVel() += -10.f*v3;
		}
	}

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

	g_CGraphics.DestroyGraphics();

	g_Box.DestroyMesh();
	g_BigBox.DestroyMesh();
	g_BoxInflated.DestroyMesh();
	g_Platform.DestroyMesh();
	g_Sphere.DestroyMesh();
	g_Torus.DestroyMesh();
	g_TorusHR.DestroyMesh();
	g_Bowl.DestroyMesh();
	g_Bunny.DestroyMesh();
	g_Bunny_slr.DestroyMesh();
	g_Glass.DestroyMesh();
	g_SmoothCube.DestroyMesh();
	g_BunnyHR.DestroyMesh();
	g_Dragon.DestroyMesh();
	g_Nut.DestroyMesh();
	g_Bolt.DestroyMesh();
	g_Funnel.DestroyMesh();

	g_Effect.DestroyEffect();

	//g_RayTracer.DestroyRayTracer();
}


