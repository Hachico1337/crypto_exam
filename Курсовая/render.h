#pragma once
#include "dx12_imgui_utils.h"
#include "string"

bool open_info = false;
bool owner_info = false;
int my_image_width = 0;
int my_image_height = 0;
ID3D12Resource* my_texture = NULL;
D3D12_CPU_DESCRIPTOR_HANDLE my_texture_srv_cpu_handle;
D3D12_GPU_DESCRIPTOR_HANDLE my_texture_srv_gpu_handle;
class StartRender
{
public:
   
   std::pair<WNDCLASSEX, HWND> CreateWnd()
    {
      
       WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC,(WNDPROC)(WndProc), 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Cursovaya"), NULL };
       ::RegisterClassEx(&wc);


       HWND hwnd = ::CreateWindowEx(WS_EX_LAYERED, wc.lpszClassName, _T(""), WS_POPUP, NULL, NULL, 200, 200, NULL, NULL, wc.hInstance, NULL);
       if (!CreateDeviceD3D(hwnd))
       {
           CleanupDeviceD3D();
           ::UnregisterClass(wc.lpszClassName, wc.hInstance);
           exit(0);
       }

       ::ShowWindow(hwnd, SW_HIDE);
       ::UpdateWindow(hwnd);
       return { wc, hwnd };
    }

   void SetKeysCallback()
   {
       WNDPROC v = WndProc;
   }
   
   ImFont* InitImGuiRender(std::string font, HWND hwnd)
   {
      
       IMGUI_CHECKVERSION();
       ImGui::CreateContext();
       ImGuiIO& io = ImGui::GetIO(); (void)io;
       io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
       io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

       ImGui::StyleColorsLight();


       ImGuiStyle& style = ImGui::GetStyle();
       if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
       {
           style.WindowRounding = 0.0f;
           style.Colors[ImGuiCol_WindowBg].w = 1.0f;
       }


       ImGui_ImplWin32_Init(hwnd);
       ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
           DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
           g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
           g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

       UINT handle_increment = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
       int descriptor_index = 1; 
       my_texture_srv_cpu_handle = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
       my_texture_srv_cpu_handle.ptr += (handle_increment * descriptor_index);
       my_texture_srv_gpu_handle = g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
       my_texture_srv_gpu_handle.ptr += (handle_increment * descriptor_index);

      
       bool ret = LoadTextureFromFile("image.jpg", g_pd3dDevice, my_texture_srv_cpu_handle, &my_texture, &my_image_width, &my_image_height);
       IM_ASSERT(ret);
       
       return   io.Fonts->AddFontFromFileTTF(font.c_str(), 14, NULL, io.Fonts->GetGlyphRangesCyrillic());;
   }
   void StartFrame()
   {

       ImGui_ImplDX12_NewFrame();
       ImGui_ImplWin32_NewFrame();
       ImGui::NewFrame();
   }
  void RenderInfoFrame()
   {
      ImGui::Begin(u8"Ура", NULL, ImGuiWindowFlags_NoResize );
      ImGui::SetWindowSize({ 400, 130 });

      ImGui::Text(u8"Ты выйграл");

      if (ImGui::Button(u8"Ок"))
          open_info = false;
      ImGui::End();
   }
  void RenderOwnerFrame()
  {
      ImGui::Begin(u8"Об авторе", NULL, ImGuiWindowFlags_NoResize);
      ImGui::SetWindowSize({ 400, 200 });
      ImGui::Text(u8"Артем Багдасарян 221-3210");
      ImGui::Text(u8"2023, ab6323003@yandex.ru");
      ImGui::Image((ImTextureID)my_texture_srv_gpu_handle.ptr, ImVec2((float)100, (float)100));

      if (ImGui::Button(u8"Ок"))
          owner_info = false;
      ImGui::End();
  }

   void RenderFrame()
   {
       ImGui::Render();

       FrameContext* frameCtx = WaitForNextFrameResources();
       UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
       frameCtx->CommandAllocator->Reset();

       D3D12_RESOURCE_BARRIER barrier = {};
       barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
       barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
       barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
       barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
       barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
       barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
       g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
       g_pd3dCommandList->ResourceBarrier(1, &barrier);

      
       g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
       g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
       ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
       barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
       barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
       g_pd3dCommandList->ResourceBarrier(1, &barrier);
       g_pd3dCommandList->Close();

       g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);
       ImGuiIO& io = ImGui::GetIO(); (void)io;
       if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
       {
           ImGui::UpdatePlatformWindows();
           ImGui::RenderPlatformWindowsDefault(NULL, (void*)g_pd3dCommandList);
       }

       g_pSwapChain->Present(1, 0);


       UINT64 fenceValue = g_fenceLastSignaledValue + 1;
       g_pd3dCommandQueue->Signal(g_fence, fenceValue);
       g_fenceLastSignaledValue = fenceValue;
       frameCtx->FenceValue = fenceValue;
   }
   void CleanAndDestroy( WNDCLASSEX wc, HWND hwnd)
   {
       WaitForLastSubmittedFrame();


       ImGui_ImplWin32_Shutdown();
       ImGui_ImplDX12_Shutdown();
       ImGui::DestroyContext();

       CleanupDeviceD3D();
       ::DestroyWindow(hwnd);
       ::UnregisterClass(wc.lpszClassName, wc.hInstance);
   }

};