#include "PSOManager.h"
#include "D3D12RHI.h"
#include "Shader.h"

using namespace TD3D12RHI;

namespace PSOManager
{
	std::unordered_map<std::string, TShader> m_shaderMap;

	std::unordered_map<std::string, GraphicsPSO> m_gfxPSOMap;

	void InitializePSO()
	{
		{
			TShaderInfo info;
			info.FileName = "shaders/modelShader";
			info.bCreateVS = true;
			info.bCreatePS = true;
			info.bCreateCS = false;
			info.VSEntryPoint = "VSMain";
			info.PSEntryPoint = "PSMain";

			TShader shader(info);
			m_shaderMap["modelShader"] = shader;

			TShaderInfo boxInfo;
			boxInfo.FileName = "shaders/skyboxShader";
			boxInfo.bCreateVS = true;
			boxInfo.bCreatePS = true;
			boxInfo.bCreateCS = false;
			boxInfo.VSEntryPoint = "VSMain";
			boxInfo.PSEntryPoint = "PSMain";

			TShader boxShader(boxInfo);
			m_shaderMap["skyboxShader"] = boxShader;
		}

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		GraphicsPSO pso(L"Normal PSO");
		pso.SetShader(&m_shaderMap["modelShader"]);
		pso.SetInputLayout(_countof(inputElementDescs), inputElementDescs);
		pso.SetRasterizerState(CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT));
		pso.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));

		D3D12_DEPTH_STENCIL_DESC dsvDesc = {};
		dsvDesc.DepthEnable = TRUE;
		dsvDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		dsvDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		dsvDesc.StencilEnable = FALSE;
		pso.SetDepthStencilState(dsvDesc);

		pso.SetSampleMask(UINT_MAX);
		pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		//pso.SetDepthTargetFormat(g_DepthBuffer.GetFormat());
		pso.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, g_DepthBuffer.GetFormat());
		pso.Finalize();

		m_gfxPSOMap["pso"] = pso;

		GraphicsPSO boxPso(L"skybox PSO");
		boxPso.SetShader(&m_shaderMap["skyboxShader"]);
		boxPso.SetInputLayout(_countof(inputElementDescs), inputElementDescs);
		D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE; // camere inside skybox
		boxPso.SetRasterizerState(rasterizerDesc);
		boxPso.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
		dsvDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // depth = 1
		boxPso.SetDepthStencilState(dsvDesc);

		boxPso.SetSampleMask(UINT_MAX);
		boxPso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		//pso.SetDepthTargetFormat(g_DepthBuffer.GetFormat());
		boxPso.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, g_DepthBuffer.GetFormat());
		boxPso.Finalize();

		m_gfxPSOMap["skyboxPSO"] = boxPso;
	}

}
