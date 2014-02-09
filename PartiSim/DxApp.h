// Copyright (c) 2014 All Right Reserved, http://8bitbear.com/
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// <author>Stephen Wheeler</author>
// <email>bear@8bitbear.com</email>
// <date>2014-01-15</date>
#pragma once
#include "Comms.h"
#include "Common.h"
#include "errorrecord.h"
#include <stdio.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "timecounter.h"



using namespace DirectX;

#define PARTNUM 2048

struct DxAppSetupDesc
{
	HINSTANCE		hInst;
	wchar_t			*windowName;
	wchar_t			*className;
	DWORD			windowStyle;
	int				windowSizeX, windowSizeY;
	int				cmdShow;
	int				renderMode;
	int				inputType;
	int				timeMode;
	wchar_t			*inFileName, *outFileName;
	wchar_t			*hostName;
};


struct Settings
{
	int width;
	int height;
	
	DXGI_FORMAT colour;
	int refreshRate;

	HINSTANCE hinst;
	bool fullscreen;
	DWORD winStyle;

	int renderMode;

	//gamesettings
	//Todo: Make an enum for inputType bitfield
	int inputType; //1=local, 2=file, 4=internet, 8=record, 16=benchmark, 32 connect. Combine local file and record to add multiple local controllers.
	int timeMode; //used with local inputType, 0 for realtime, or set number of samples per second.

	FILE* inFile;
	FILE* outFile;

	int profile;
};

class DxApp
{
private:
	HWND m_hWnd;
	Settings m_settings;
	Comms m_Comms;

	IDXGIFactory*				m_pFactory;
	D3D_DRIVER_TYPE				m_driverType;
	D3D_FEATURE_LEVEL			m_featureLevel;
	ID3D11Device*				m_pd3dDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RasterizerState*		m_pRasterizeNoDepth;
	ID3D11RasterizerState*		m_pRasterizeDepth;
	ID3D11RenderTargetView*		m_pRenderFinalTargetView;
	ID3D11SamplerState*         m_pSamplerState;
	D3D11_VIEWPORT				m_simViewPort;
	D3D11_VIEWPORT				m_finalViewPort;

	bool m_close;

	ID3D11VertexShader*			m_pVertexShader;
	ID3D11InputLayout*			m_pVertexInput;
	ID3D11PixelShader*			m_pPixelShader;
	ID3D11PixelShader*			m_pSimShader;
	ID3D11ComputeShader*		m_pCSSimShader;
	ID3D11PixelShader*			m_pResetShader;
	ID3D11PixelShader*			m_pHaltShader;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11Buffer*				m_pSimInfoCB;
	ID3D11Buffer*				m_pSimInput;

	/*array<int, 3>				*m_pData;
	array<graphics::float_4, 2>	*m_pDataPartLoc;
	array<unsigned int, 2>		*m_pCountGrid;*/

	ID3D11Buffer				*m_pTextureBuffer;
	//ID3D11Buffer				*m_pTextureExport; // can be used for debugging
	ID3D11UnorderedAccessView	*m_pTextureBufferUAView;
	ID3D11ShaderResourceView	*m_pTextureView;
	ID3D11Texture2D				*m_pTextureDataBuffer[2];
	//ID3D11Texture2D			*m_pTextureDataExport; // can be used for debugging
	ID3D11ShaderResourceView	*m_pTextureDataView[2];
	ID3D11RenderTargetView		*m_pDataRenderTargetView[2];
	ID3D11UnorderedAccessView	*m_pTextureDataUAView[2];
	int							m_flip;
	ID3D11Buffer                *m_pTextureRefBuffer;
	ID3D11ShaderResourceView	*m_pTextureRefView;


	bool						m_mouse1;
	bool						m_mouse2;
	int							m_mousePositionX;
	int							m_mousePositionY;
	TimePast					m_timer;
	TimePast					m_performanceS;
	TimePast					m_performanceD;
	TimePast					m_performanceR;
	float						m_counterExpectedTime;
	float						m_counterChunk;

	int							m_oWidth;
	int							m_oHeight;
	float						m_scale;
	int							m_noRedraw;
	TimePast					m_holdTime;
	int							m_simReset, m_simHalt;
public:
	DxApp(void);
	~DxApp(void);

	int Init(DxAppSetupDesc *in_desc);
	

	int Run();
private:
	int SetupWindow(DxAppSetupDesc *in_desc);

	int SetupDxResources();
	int SetupSizeDependentResources();
	void ResizeSizeDependentResources(int width, int height);
	void Render();

	static LRESULT CALLBACK DxApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

	void OnKeyUp(SHORT vkey);

	bool GetInput(SimInput &input);

	void SimReset();
	void SimZeroVelocity();

	void DisplayBenchmarkComplete();

	void RecordUpdate(wchar_t *filename);
};

struct SimDetailsCB
{
	float Box[4];
	float tscale;
	int	dimensions[2];
};