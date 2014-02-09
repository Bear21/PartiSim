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
#include "DxApp.h"



void DxApp::Render()
{
	/*array<int, 3>& gidd = *m_pData;
	array<graphics::float_4, 2>& locs = *m_pDataPartLoc;
	array<unsigned int, 2>& gids = *m_pCountGrid;*/
	//graphics::texture<graphics::float_2, 3>& gidd = *m_pDataGrid;
	
	
	float resw=m_oWidth, resh=m_oHeight;
	float tscale = m_scale;
	int iresw=m_settings.width;
	//float timeP = m_timer.Check();
#ifdef DEBUG_OUTPUT_PROFILE
	wchar_t report[64];
#endif
	SimInput input;
	input.numControl = 0;

	if(!GetInput(input))
		return;
	float timeP = input.timeP;
	//simulate
	//m_pTextureBufferUAView

	ID3D11DepthStencilView *dsNullview = NULL;
	ID3D11ShaderResourceView *srvNull = NULL;
	ID3D11RenderTargetView *rtvNull = NULL;
	ID3D11ShaderResourceView *srvSim[] = {m_pTextureDataView[m_flip], NULL, NULL};
	m_pImmediateContext->RSSetViewports( 1, &m_simViewPort );
	m_pImmediateContext->UpdateSubresource( m_pSimInput, 0, NULL, &input, 0, 0 );

	for(int i=0; i<input.numControl; i++)
	{
		if((input.controlInput[i].inputLow & 1<<3) == 1<<3)
		{
			m_pImmediateContext->OMSetRenderTargets(1, &m_pDataRenderTargetView[!m_flip], dsNullview);
			m_pImmediateContext->PSSetShaderResources(0, 1, srvSim);
			m_pImmediateContext->PSSetShader(m_pResetShader, NULL, 0);
			m_pImmediateContext->Draw(4, 0);
			m_flip = !m_flip;
			srvSim[0] = m_pTextureDataView[m_flip];
			m_pImmediateContext->PSSetShaderResources(0, 1, &srvNull);
			m_pImmediateContext->OMSetRenderTargets(1, &rtvNull, dsNullview);
			
			break;//no point in running any more
		}
		if((input.controlInput[i].inputLow & 1<<4) == 1<<4)
		{
			m_pImmediateContext->OMSetRenderTargets(1, &m_pDataRenderTargetView[!m_flip], dsNullview);
			m_pImmediateContext->PSSetShaderResources(0, 1, srvSim);
			m_pImmediateContext->PSSetShader(m_pHaltShader, NULL, 0);
			m_pImmediateContext->Draw(4, 0);
			m_flip = !m_flip;
			srvSim[0] = m_pTextureDataView[m_flip];
			m_pImmediateContext->PSSetShaderResources(0, 1, &srvNull);
			m_pImmediateContext->OMSetRenderTargets(1, &rtvNull, dsNullview);
		}
	}
	
	if(m_settings.renderMode==0)
	{
		m_pImmediateContext->PSSetShaderResources(0, 1, srvSim);
		m_pImmediateContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pDataRenderTargetView[!m_flip], dsNullview, 1, 1, &m_pTextureBufferUAView, NULL);
		m_pImmediateContext->PSSetShader(m_pSimShader, NULL, 0);
	
		m_pImmediateContext->Draw(4, 0);
	}
	else
	{		
		m_pImmediateContext->CSSetShaderResources(0, 1, srvSim);
		ID3D11UnorderedAccessView *UAVList[] = {m_pTextureDataUAView[!m_flip], m_pTextureBufferUAView};
		m_pImmediateContext->CSSetUnorderedAccessViews(0, 2, UAVList, NULL);
		m_pImmediateContext->Dispatch(PARTNUM/16, PARTNUM/16, 1);

		UAVList[0] = NULL;
		UAVList[1] = NULL;
		m_pImmediateContext->CSSetUnorderedAccessViews(0, 2, UAVList, NULL);
		m_pImmediateContext->CSSetShaderResources(0, 1, &srvNull);
	}

	/*m_pImmediateContext->CopyResource(m_pTextureExport, m_pTextureBuffer);
	D3D11_MAPPED_SUBRESOURCE debughelper;
	m_pImmediateContext->Map(m_pTextureExport, 0, D3D11_MAP_READ, 0, &debughelper);
	m_pImmediateContext->Unmap(m_pTextureExport, 0);*/
	
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderFinalTargetView, dsNullview);
	ID3D11ShaderResourceView *resources[] = {m_pTextureView, NULL, NULL};
	m_pImmediateContext->PSSetShaderResources(0, 1, resources);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->RSSetViewports( 1, &m_finalViewPort );
	m_pImmediateContext->Draw(4, 0);

	unsigned int initTemp[4] = {0,0,0,0};
	m_pImmediateContext->ClearUnorderedAccessViewUint(m_pTextureBufferUAView, initTemp);
	m_flip = !m_flip;

	if(m_settings.timeMode!=0 && (m_settings.inputType & 36)!=0)
	{
		float gap;
		//temp code for stable network play
		while((gap = m_holdTime.Peek())<1.f/m_settings.timeMode)
		{
			if((1.f/m_settings.timeMode-gap)>0.002f)
				Sleep(1);
		}
	}
	m_pSwapChain->Present( 0, 0 );
	if(m_settings.profile)
	{
#ifdef DEBUG_OUTPUT_PROFILE
		swprintf(report, L" PRE, %fmsec", m_performanceS.Check()*1000.f);
		OutputDebugString(report);
#endif
	}
	//m_pImmediateContext->PSSetShaderResources(0, 1, srvNull);
}

int DxApp::SetupSizeDependentResources()
{
	m_simViewPort.Width = (FLOAT)PARTNUM;
	m_simViewPort.Height = (FLOAT)PARTNUM; 
	m_simViewPort.MinDepth = 0.0f;
    m_simViewPort.MaxDepth = 1.0f;
    m_simViewPort.TopLeftX = 0;
    m_simViewPort.TopLeftY = 0;

	m_finalViewPort.Width = (FLOAT)m_settings.width;
	m_finalViewPort.Height = (FLOAT)m_settings.height; 

    m_finalViewPort.MinDepth = 0.0f;
    m_finalViewPort.MaxDepth = 1.0f;
    m_finalViewPort.TopLeftX = 0;
    m_finalViewPort.TopLeftY = 0;

	m_pImmediateContext->RSSetViewports( 1, &m_simViewPort );


	D3D11_BUFFER_DESC descT;
	ZeroMemory( &descT, sizeof(descT) );
	descT.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	descT.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	descT.ByteWidth = m_settings.width*m_settings.height*4;
	descT.Usage = D3D11_USAGE_DEFAULT;
	descT.StructureByteStride = 0;
	m_pd3dDevice->CreateBuffer(&descT, NULL, &m_pTextureBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_settings.width*m_settings.height;
	UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	m_pd3dDevice->CreateUnorderedAccessView(m_pTextureBuffer, &UAVDesc, &m_pTextureBufferUAView);

	D3D11_SHADER_RESOURCE_VIEW_DESC descSR;
	ZeroMemory( &descSR, sizeof(descSR) );
	descSR.Format = DXGI_FORMAT_R32_TYPELESS;
	descSR.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	descSR.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	descSR.BufferEx.NumElements = m_settings.width*m_settings.height;
	m_pd3dDevice->CreateShaderResourceView(m_pTextureBuffer, &descSR, &m_pTextureView);

	unsigned int initTemp[4] = {0,0,0,0};
	m_pImmediateContext->ClearUnorderedAccessViewUint(m_pTextureBufferUAView, initTemp);

	SimDetailsCB CB = {0, 0, (float)m_oWidth, (float)m_oHeight, m_scale, m_settings.width, m_settings.height};
	m_pImmediateContext->UpdateSubresource( m_pSimInfoCB, 0, NULL, &CB, 0, 0 );

	ID3D11Buffer *CBList[] = {m_pConstantBuffer, m_pSimInfoCB, m_pSimInput};
	m_pImmediateContext->PSSetConstantBuffers(0, 3, CBList);
	m_pImmediateContext->CSSetConstantBuffers(0, 3, CBList);

	return 0;
}

//this function has been disabled while converting from C++ AMP to 100% directX
void DxApp::ResizeSizeDependentResources(int width, int height)
{
	if(width!=0)
	{
		if((m_settings.inputType&16)!=16)
		{
			m_settings.width = width;
			m_settings.height = height;

			m_scale = (float)m_settings.width/(float)m_oWidth;
			if((float)m_settings.height/(float)m_oHeight< m_scale)
			{
				m_scale = (float)m_settings.height/(float)m_oHeight;
			}
			if(m_pImmediateContext)
			{
				ID3D11ShaderResourceView *srvNull[] = {NULL, NULL, NULL};
				m_pImmediateContext->PSSetShaderResources(0, 1, srvNull);

			
				m_finalViewPort.Width = (FLOAT)m_settings.width;
				m_finalViewPort.Height = (FLOAT)m_settings.height; 
				m_finalViewPort.MinDepth = 0.0f;
				m_finalViewPort.MaxDepth = 1.0f;
				m_finalViewPort.TopLeftX = 0;
				m_finalViewPort.TopLeftY = 0;

				m_pImmediateContext->RSSetViewports( 0, NULL );

				// Create a render target view
				ID3D11DepthStencilView *dsNullview = NULL;
				ID3D11RenderTargetView *rtNullview = NULL;
				m_pImmediateContext->OMSetRenderTargets( 1, &rtNullview, dsNullview );
				m_pRenderFinalTargetView->Release();

				m_pTextureView->Release();
				m_pTextureBufferUAView->Release();
				m_pTextureBuffer->Release();

				m_pSwapChain->ResizeBuffers(2, m_settings.width, m_settings.height, DXGI_FORMAT_UNKNOWN, 0);

				DXGI_SWAP_CHAIN_DESC swdesc;
				m_pSwapChain->GetDesc(&swdesc);
				if(swdesc.BufferDesc.Width!=m_settings.width)
					OutputDebugString(L"welp");

				ID3D11Texture2D* pBackBuffer = NULL;
				m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );

				m_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pRenderFinalTargetView );
				pBackBuffer->Release();

				m_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderFinalTargetView, dsNullview );

				m_pImmediateContext->RSSetState(m_pRasterizeNoDepth);

				m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);

				D3D11_BUFFER_DESC descT;
				ZeroMemory( &descT, sizeof(descT) );
				descT.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
				descT.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
				descT.ByteWidth = m_settings.width*m_settings.height*4;
				descT.Usage = D3D11_USAGE_DEFAULT;
				descT.StructureByteStride = 0;
				m_pd3dDevice->CreateBuffer(&descT, NULL, &m_pTextureBuffer);

				D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
				UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				UAVDesc.Buffer.FirstElement = 0;
				UAVDesc.Buffer.NumElements = m_settings.width*m_settings.height;
				UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
				m_pd3dDevice->CreateUnorderedAccessView(m_pTextureBuffer, &UAVDesc, &m_pTextureBufferUAView);

				D3D11_SHADER_RESOURCE_VIEW_DESC descSR;
				ZeroMemory( &descSR, sizeof(descSR) );
				descSR.Format = DXGI_FORMAT_R32_TYPELESS;
				descSR.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
				descSR.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
				descSR.BufferEx.NumElements = m_settings.width*m_settings.height;
				m_pd3dDevice->CreateShaderResourceView(m_pTextureBuffer, &descSR, &m_pTextureView);

				unsigned int initTemp[4] = {0,0,0,0};
				m_pImmediateContext->ClearUnorderedAccessViewUint(m_pTextureBufferUAView, initTemp);

				SimDetailsCB CB = {0, 0, (float)m_oWidth, (float)m_oHeight, m_scale, m_settings.width, m_settings.height};
				m_pImmediateContext->UpdateSubresource( m_pSimInfoCB, 0, NULL, &CB, 0, 0 );

				ID3D11Buffer *CBList[] = {m_pConstantBuffer, m_pSimInfoCB, m_pSimInput};
				m_pImmediateContext->PSSetConstantBuffers(0, 3, CBList);
				m_pImmediateContext->CSSetConstantBuffers(0, 3, CBList);
			}
		}
		else
		{
		
		}
	}

}


void DxApp::SimReset()
{
	m_simReset=1;
	/*array<graphics::float_4, 2>& locs = *m_pDataPartLoc;

	float scale = 1.f/PARTNUM;

	float sw = m_oWidth, sh=m_oHeight;

	parallel_for_each(locs.extent, [=, &locs](index<2> idx) restrict(amp)
	{
		graphics::float_4 &ref = locs[idx[0]][idx[1]];
		ref.z = ((idx[0])*scale)*sw;
		ref.w = ((idx[1])*scale)*sh;
		ref.x = 0;
		ref.y = 0;
	});*/
}
void DxApp::SimZeroVelocity()
{
	m_simHalt=1;
	//array<graphics::float_4, 2>& locs = *m_pDataPartLoc;

	//parallel_for_each(locs.extent, [=, &locs](index<2> idx) restrict(amp)
	//{
	//	graphics::float_4 &ref = locs[idx[0]][idx[1]];
	//	ref.x = 0;
	//	ref.y = 0;
	//});
}