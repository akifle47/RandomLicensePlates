#define _CRT_SECURE_NO_WARNINGS

#include <d3dx9.h>

#include "rage/TxdStore.h"
#include "rage/Device.h"

void Init();
void Run();
void LoadShaders();
void __fastcall OnDeviceLost();
void __fastcall OnDeviceReset();

void (__cdecl *CVehicle__SubmitDrawCommandsO)() = nullptr;

void __declspec(naked) CVehicle__SubmitDrawCommandsH()
{
	_asm
	{
		call Run
	}
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		CVehicle__SubmitDrawCommandsO = *(void(__cdecl **)())Utils::ReadMemory(0x4F2630);
		Utils::WriteMemory(0x4F2630, CVehicle__SubmitDrawCommandsH);
	}

	return true;
}

IDirect3DVertexBuffer9 *gQuadVertexBuffer = nullptr;
IDirect3DVertexDeclaration9 *gQuadVertexDecl = nullptr;
IDirect3DVertexShader9 *gShaderVS = nullptr;
IDirect3DPixelShader9 *gShaderPS = nullptr;
IDirect3DSurface9 *gVehicleDetailRT = nullptr;

void Run()
{

}

void Init()
{
	static bool initialized = false;
	if(initialized)
	{
		return;
	}

	IDirect3DDevice9 *device = rage::grcDevice::GetDeviceD3D();

	rage::Functor0 onLostCB = rage::Functor0(NULL, OnDeviceLost, NULL, 0);
	rage::Functor0 onResetCB = rage::Functor0(NULL, OnDeviceReset, NULL, 0);

	rage::grcDevice::RegisterDeviceLostCallbacks(onLostCB, onResetCB);

	int32_t slot = rage::CTxdStore::FindSlot("vehshare");
	if(slot > 0)
	{
		rage::grcTexturePC *texture = rage::CTxdStore::GetSlot(slot)->Lookup(rage::atStringHash("vehicle_generic_detail2", 0));
		texture->mTextureD3D;
	}

#ifdef _DEBUG
	LoadShaders();
#else
	//TODO!!!
#endif //_DEBUG

	initialized = true;
}

void LoadShaders()
{
	HRESULT hr;
	LPD3DXBUFFER vertexShaderData = nullptr;
	LPD3DXBUFFER pixelShaderData = nullptr;
	LPD3DXBUFFER errorBuffer = nullptr;

	hr = D3DXCompileShaderFromFile(L"shaders/Shader.vs", NULL, NULL, "main", "vs_3_0", 0, &vertexShaderData, &errorBuffer, 0);
	if(FAILED(hr))
	{
		MessageBoxA(0, (char *)errorBuffer->GetBufferPointer(), "VS ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	gShaderVS->Release();
	rage::grcDevice::GetDeviceD3D()->CreateVertexShader((DWORD *)vertexShaderData->GetBufferPointer(), &gShaderVS);

	hr = D3DXCompileShaderFromFile(L"shaders/Shader.ps", NULL, NULL, "main", "ps_3_0", 0, &pixelShaderData, &errorBuffer, 0);
	if(FAILED(hr))
	{
		MessageBoxA(0, (char *)errorBuffer->GetBufferPointer(), "PS ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	gShaderPS->Release();
	rage::grcDevice::GetDeviceD3D()->CreatePixelShader((DWORD *)pixelShaderData->GetBufferPointer(), &gShaderPS);

	return;
}