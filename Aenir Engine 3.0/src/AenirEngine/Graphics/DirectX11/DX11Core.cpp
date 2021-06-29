#include"PCH.h"
#include"DX11Core.h"

namespace Aen {

    ComDevice GCore::m_device = NULL;
    ComDeviceContext GCore::m_dContext = NULL;
    ComSwapChain GCore::m_sChain = NULL;

	bool GCore::Concealed::Initialize(const Window& window) {
    
        DXGI_SWAP_CHAIN_DESC sChainDesc;
        ZeroMemory(&sChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        sChainDesc.BufferDesc.Width = window.GetSize().x;
        sChainDesc.BufferDesc.Height = window.GetSize().y;
        sChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        sChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        sChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        
        sChainDesc.SampleDesc.Count = 1;
        sChainDesc.SampleDesc.Quality = 0;
        
        sChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sChainDesc.BufferCount = 1;
        sChainDesc.OutputWindow = window.m_hwnd;
        sChainDesc.Windowed = TRUE;
        sChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        
        ComFactory6 pFactory = NULL;
        if(FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory6), reinterpret_cast<void**>(pFactory.GetAddressOf()))))
            return false;

        ComAdapter1 pAdapter = NULL;
        if(FAILED(pFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(pAdapter.GetAddressOf()))))
            return false;

        const UINT featureLvls = 2;
        D3D_FEATURE_LEVEL featureLvl[featureLvls] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };

        UINT flags = 0;
        #ifdef _DEBUG
        flags = D3D11_CREATE_DEVICE_DEBUG;
        #endif

        return SUCCEEDED(D3D11CreateDeviceAndSwapChain(
            pAdapter.Get(),  
            D3D_DRIVER_TYPE_UNKNOWN,
            NULL, 
            flags, 
            featureLvl, 
            featureLvls,
            D3D11_SDK_VERSION, 
            &sChainDesc, 
            &m_sChain, 
            &m_device, 
            NULL, 
            &m_dContext));
	}
}