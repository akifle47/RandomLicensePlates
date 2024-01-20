#define _CRT_SECURE_NO_WARNINGS

#include <d3dx9.h>

#include "rage/TxdStore.h"
#include "rage/Device.h"
#include "rage/T_CB_Generic.h"
#include "rage/Vector.h"

void Init();
void LoadShaders();
void Run();

void RenderCallback();

void __fastcall OnDeviceLost();
void __fastcall OnDeviceReset();

void (__cdecl *CVehicle__SubmitDrawCommandsO)() = nullptr;

void __declspec(naked) CVehicle__SubmitDrawCommandsH()
{
	_asm
	{
		push ebp
		mov ebp, esp

		push ecx
		call Run
		pop ecx

		push [ebp+0x14]
		push [ebp+0x10]
		push [ebp+0xC]
		push [ebp+0x8]
		call CVehicle__SubmitDrawCommandsO

		mov esp, ebp
		pop ebp
		ret 0x10
	}
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		CVehicle__SubmitDrawCommandsO = (void(__cdecl *)())Utils::ReadMemory(0x4F2630);
		Utils::WriteMemory(0x9915B0, CVehicle__SubmitDrawCommandsH);
		Utils::WriteMemory(0x99D4E0, CVehicle__SubmitDrawCommandsH);
		Utils::WriteMemory(0x99FCF8, CVehicle__SubmitDrawCommandsH);
		Utils::WriteMemory(0x9AFB38, CVehicle__SubmitDrawCommandsH);
		Utils::WriteMemory(0x9B0738, CVehicle__SubmitDrawCommandsH);
		Utils::WriteMemory(0x9CDA80, CVehicle__SubmitDrawCommandsH);
		Utils::WriteMemory(0x9CF070, CVehicle__SubmitDrawCommandsH);
	}

	return true;
}

IDirect3DVertexBuffer9 *gQuadVertexBuffer = nullptr;
IDirect3DVertexDeclaration9 *gQuadVertexDecl = nullptr;
IDirect3DVertexShader9 *gShaderVS = nullptr;
IDirect3DPixelShader9 *gShaderPS = nullptr;
IDirect3DTexture9 *gVehicleDetailRT = nullptr;

IDirect3DVertexShader9 *gBlitVS = nullptr;
IDirect3DPixelShader9 *gBlitPS = nullptr;

void Run()
{
	Init();

	auto cb = new rage::T_CB_Generic_NoArgs<void (__cdecl*)()>(RenderCallback);
	cb->Init();
}

void Init()
{
	static bool initialized = false;
	if(initialized)
	{
		return;
	}

	LoadShaders();

	OnDeviceReset();

	IDirect3DDevice9 *device = rage::grcDevice::GetDeviceD3D();

	rage::Functor0 onLostCB = rage::Functor0(nullptr, OnDeviceLost, nullptr, 0);
	rage::Functor0 onResetCB = rage::Functor0(nullptr, OnDeviceReset, nullptr, 0);

	rage::grcDevice::RegisterDeviceLostCallbacks(onLostCB, onResetCB);
	
	initialized = true;
}

void LoadShaders()
{
	HRESULT hr;
	LPD3DXBUFFER vertexShaderData = nullptr;
	LPD3DXBUFFER pixelShaderData = nullptr;
	LPD3DXBUFFER errorBuffer = nullptr;

#ifdef _DEBUG
	hr = D3DXCompileShaderFromFile(L"shaders/Shader.vs", nullptr, nullptr, "main", "vs_3_0", 0, &vertexShaderData, &errorBuffer, 0);
	if(FAILED(hr))
	{
		MessageBoxA(0, (char *)errorBuffer->GetBufferPointer(), "VS ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	if(gShaderVS)
	{
		gShaderVS->Release();
	}
	rage::grcDevice::GetDeviceD3D()->CreateVertexShader((DWORD *)vertexShaderData->GetBufferPointer(), &gShaderVS);

	hr = D3DXCompileShaderFromFile(L"shaders/Shader.ps", nullptr, nullptr, "main", "ps_3_0", 0, &pixelShaderData, &errorBuffer, 0);
	if(FAILED(hr))
	{
		MessageBoxA(0, (char *)errorBuffer->GetBufferPointer(), "PS ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	if(gShaderPS)
	{
		gShaderPS->Release();
	}
	rage::grcDevice::GetDeviceD3D()->CreatePixelShader((DWORD *)pixelShaderData->GetBufferPointer(), &gShaderPS);
#else
	//TODO!!!
#endif //_DEBUG

	const uint8_t blitVS[] = {
		0x00, 0x03, 0xFE, 0xFF, 0xFE, 0xFF, 0x22, 0x00, 0x43, 0x54, 0x41, 0x42,
		0x1C, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFE, 0xFF,
		0x01, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x4C, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x02, 0x00, 0x05, 0x00,
		0x01, 0x00, 0x16, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x50, 0x69, 0x78, 0x65, 0x6C, 0x4F, 0x66, 0x66, 0x73, 0x65, 0x74, 0x00,
		0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x76, 0x73, 0x5F, 0x33, 0x5F, 0x30, 0x00, 0x4D,
		0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29,
		0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72,
		0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x39, 0x2E,
		0x32, 0x39, 0x2E, 0x39, 0x35, 0x32, 0x2E, 0x33, 0x31, 0x31, 0x31, 0x00,
		0x51, 0x00, 0x00, 0x05, 0x00, 0x00, 0x0F, 0xA0, 0x00, 0x00, 0x80, 0x3F,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1F, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0F, 0x90,
		0x1F, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80, 0x01, 0x00, 0x0F, 0x90,
		0x1F, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0F, 0xE0,
		0x1F, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80, 0x01, 0x00, 0x03, 0xE0,
		0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0xE0, 0x05, 0x00, 0xE4, 0xA0,
		0x00, 0x00, 0xE4, 0x90, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0C, 0xE0,
		0x00, 0x00, 0xAA, 0x90, 0x00, 0x00, 0x44, 0xA0, 0x00, 0x00, 0x14, 0xA0,
		0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x03, 0xE0, 0x01, 0x00, 0xE4, 0x90,
		0xFF, 0xFF, 0x00, 0x00
	};

	const uint8_t blitPS[] = {
		0x00, 0x03, 0xFF, 0xFF, 0xFE, 0xFF, 0x23, 0x00, 0x43, 0x54, 0x41, 0x42,
		0x1C, 0x00, 0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF,
		0x01, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x50, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x53, 0x61, 0x6D, 0x70, 0x6C,
		0x65, 0x72, 0x00, 0xAB, 0x04, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x01, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x73, 0x5F, 0x33,
		0x5F, 0x30, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74,
		0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68,
		0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65,
		0x72, 0x20, 0x39, 0x2E, 0x32, 0x39, 0x2E, 0x39, 0x35, 0x32, 0x2E, 0x33,
		0x31, 0x31, 0x31, 0x00, 0x1F, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80,
		0x00, 0x00, 0x03, 0x90, 0x1F, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
		0x00, 0x08, 0x0F, 0xA0, 0x42, 0x00, 0x00, 0x03, 0x00, 0x08, 0x0F, 0x80,
		0x00, 0x00, 0xE4, 0x90, 0x00, 0x08, 0xE4, 0xA0, 0xFF, 0xFF, 0x00, 0x00
	};
	
	rage::grcDevice::GetDeviceD3D()->CreateVertexShader((DWORD *)blitVS, &gBlitVS);
	rage::grcDevice::GetDeviceD3D()->CreatePixelShader((DWORD *)blitPS, &gBlitPS);

	return;
}

void __fastcall OnDeviceLost()
{
	if(gQuadVertexBuffer)
	{
		gQuadVertexBuffer->Release();
		gQuadVertexBuffer = nullptr;
	}

	if(gQuadVertexDecl)
	{
		gQuadVertexDecl->Release();
		gQuadVertexDecl = nullptr;
	}

	if(gVehicleDetailRT)
	{
		gVehicleDetailRT->Release();
		gVehicleDetailRT = nullptr;
	}
}

void __fastcall OnDeviceReset()
{
	IDirect3DDevice9 *device = rage::grcDevice::GetDeviceD3D();

	struct VertexFormat
	{
		float Pos[3];
		float TexCoord[2];
	};

	D3DVERTEXELEMENT9 vertexDeclElements[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	device->CreateVertexDeclaration(vertexDeclElements, &gQuadVertexDecl);
	device->CreateVertexBuffer(6 * sizeof(VertexFormat), 0, 0, D3DPOOL_DEFAULT, &gQuadVertexBuffer, nullptr);

	VertexFormat *vertexData;
	gQuadVertexBuffer->Lock(0, 0, (void **)&vertexData, 0);

	vertexData[0] = {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f};
	vertexData[1] = {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
	vertexData[2] = {1.0f, -1.0f, 0.0f, 1.0f, 1.0f};
	vertexData[3] = {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
	vertexData[4] = {1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	vertexData[5] = {1.0f, -1.0f, 0.0f, 1.0f, 1.0f};

	gQuadVertexBuffer->Unlock();

	int32_t slot = rage::CTxdStore::FindSlot("vehshare");
	if(slot > 0)
	{
		rage::grcTexturePC *texture = rage::CTxdStore::GetSlot(slot)->Lookup(rage::atStringHash("vehicle_generic_detail2", 0));

		device->CreateTexture(texture->mWidth, texture->mHeight, texture->mMipCount, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &gVehicleDetailRT, 0);
	}
}

void RenderCallback()
{
	IDirect3DDevice9 *device = rage::grcDevice::GetDeviceD3D();

	int32_t slot = rage::CTxdStore::FindSlot("vehshare");
	if(slot > 0)
	{
		rage::grcTexturePC *texture = rage::CTxdStore::GetSlot(slot)->Lookup(rage::atStringHash("vehicle_generic_detail2", 0));
		if(!texture->mTextureD3D)
		{
			return;
		}

		IDirect3DSurface9 *vehicleDetailSurf = nullptr;

		//save render states
		IDirect3DSurface9 *prevSurf;
		IDirect3DVertexShader9 *prevVS;
		IDirect3DPixelShader9 *prevPS;
		IDirect3DVertexDeclaration9 *prevDecl;
		D3DVIEWPORT9 prevVP;

		device->GetRenderTarget(0, &prevSurf);
		device->GetVertexShader(&prevVS);
		device->GetPixelShader(&prevPS);
		device->GetVertexDeclaration(&prevDecl);
		device->GetViewport(&prevVP);

		//copy
		if(texture->mTextureD3D != gVehicleDetailRT)
		{
			texture->mTextureD3D->Release();
			texture->mTextureD3D = gVehicleDetailRT;

			gVehicleDetailRT->GetSurfaceLevel(0, &vehicleDetailSurf);

			device->SetRenderTarget(0, vehicleDetailSurf);

			device->SetRenderState(D3DRS_ZENABLE, false);

			device->SetVertexShader(gShaderVS);
			device->SetPixelShader(gShaderPS);

			device->SetVertexDeclaration(gQuadVertexDecl);
			device->SetStreamSource(0, gQuadVertexBuffer, 0, 20);

			rage::Vector4 pixelOffset;
			pixelOffset.x = (-1.0f / texture->mWidth);
			pixelOffset.y = (1.0f / texture->mHeight);

			rage::Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

			device->SetVertexShaderConstantF(5, &pixelOffset.x, 1);
			device->SetPixelShaderConstantF(0, &color.x, 1);

			device->SetTexture(0, texture->mTextureD3D);

			device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			device->SetRenderState(D3DRS_ZENABLE, true);
		}

		//test
		gVehicleDetailRT->GetSurfaceLevel(0, &vehicleDetailSurf);

		device->SetRenderTarget(0, vehicleDetailSurf);

		device->SetRenderState(D3DRS_ZENABLE, false);

		device->SetVertexShader(gShaderVS);
		device->SetPixelShader(gShaderPS);

		device->SetVertexDeclaration(gQuadVertexDecl);
		device->SetStreamSource(0, gQuadVertexBuffer, 0, 20);

		rage::Vector4 pixelOffset;
		pixelOffset.x = (-1.0f / texture->mWidth);
		pixelOffset.y = (1.0f / texture->mHeight);

		rage::Vector4 color = {0.0f, 1.0f, 0.0f, 1.0f};

		device->SetVertexShaderConstantF(5, &pixelOffset.x, 1);
		device->SetPixelShaderConstantF(0, &color.x, 1);

		device->SetTexture(0, texture->mTextureD3D);

		D3DVIEWPORT9 viewport = {0, 468, 97, 44, 0, 1};
		device->SetViewport(&viewport);

		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		device->SetRenderState(D3DRS_ZENABLE, true);

		//update mip maps
		device->SetViewport(&prevVP);
		for(uint32_t i = 0; i < texture->mMipCount; i++)
		{
			IDirect3DSurface9 *surf1, *surf2;
			gVehicleDetailRT->GetSurfaceLevel(i, &surf1);
			gVehicleDetailRT->GetSurfaceLevel(i + 1, &surf2);

			device->StretchRect(surf1, nullptr, surf2, nullptr, D3DTEXF_LINEAR);
		}

		//restore render states
		device->SetVertexShader(prevVS);
		device->SetPixelShader(prevPS);
		device->SetRenderTarget(0, prevSurf);
		device->SetVertexDeclaration(prevDecl);
		device->SetViewport(&prevVP);

		vehicleDetailSurf->Release();
		prevSurf->Release();
		prevDecl->Release();
	}
}