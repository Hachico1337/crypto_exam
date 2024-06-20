#include "render.h"
#include <vector>
#include "dear_rects.h"
#include <iostream>



#include <random>

const  uint8_t KEY[32] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
const  uint8_t IV[16] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
#include "aes.hpp"
std::string  gen_num()
{
    std::random_device random_device; 
    std::mt19937 generator(random_device()); 



    std::uniform_int_distribution<> distribution(-1000, 1000);




    int x = distribution(generator);
    std::string num = std::to_string(x);
    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, KEY, IV);
    AES_CTR_xcrypt_buffer(&ctx,(uint8_t*)num.c_str(), num.size());    

    return num;
}

bool Contains(RectInfo self ,ImVec2 point)
{
    ImVec2 curpos;
    curpos.x = ImGui::GetCursorScreenPos().x + self.Pos.x * self.size.x;
    curpos.y = ImGui::GetCursorScreenPos().y + self.Pos.y * self.size.y;
    return point.x >= curpos.x && point.x < (curpos.x + self.size.x) && point.y >= curpos.y && point.y < (curpos.y + self.size.y);
}

ImVec2 MousePos;
ImVec2 MouseDelta;
ImVec2 OldMousePos;
bool in_action = false;

void ProcessInput()
{ 
    MousePos = ImGui::GetIO().MousePos;
    MouseDelta.x = MousePos.x - OldMousePos.x;
    MouseDelta.y = MousePos.y - OldMousePos.y;
  
    OldMousePos = MousePos;
}

void reset_game()
{
    open_info = false;

    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            RectList[i][j] = "";
        }
    }
}
namespace global_config
{
     bool entered = false;
     char pin[255];
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
   // AllocConsole();
    //freopen("CONOUT$", "w", stdout);
    //std::cout << "debug console opened" << std::endl;
    memset(global_config::pin, 0x0, sizeof(global_config::pin));

    StartRender* manager = nullptr;
    manager->SetKeysCallback();
    auto wc_hwnd = manager->CreateWnd();

    ImFont* MyFont = manager->InitImGuiRender("C:\\Windows\\Fonts\\Arial.ttf", wc_hwnd.second);

    bool is_open = true;
    while (is_open)
    {

        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                is_open = false;
        }
        if (!is_open)
            break;

        manager->StartFrame();

        ImGui::PushFont(MyFont);
       
        ImGuiWindowFlags_ flags = ImGuiWindowFlags_None ;
        if (in_action)
            flags = ImGuiWindowFlags_NoMove;
       

        in_action = false;

        ProcessInput();
        ImGui::Begin(u8"Экзамен Багдасарян", &is_open, ImGuiWindowFlags_NoResize  | flags);
        int score = 0;
        ImGui::SetWindowSize({ 400, 400 });
        if (global_config::entered)
        {
    
            if (ImGui::Button(u8"Сбросить"))
            {
                reset_game();
            }
         
            ImGui::SetCursorPos({ 100, 100 });
            int counter = 0;
            for (int i = 0; i < size_; i++)
            {
                for (int j = 0; j < size_; j++)
                {

                    ImVec2 Pos;
                    Pos.x = ImGui::GetCursorScreenPos().x + i * 55;
                    Pos.y = ImGui::GetCursorScreenPos().y + j * 55;
                    if (RectList[i][j] != "")
                    {
                        counter++;
                        struct AES_ctx ctx;
                        std::string decrypted_buf = RectList[i][j];
                        AES_init_ctx_iv(&ctx, KEY, IV);
                        AES_CTR_xcrypt_buffer(&ctx, (uint8_t*)decrypted_buf.c_str(), decrypted_buf.size());

                        //std::cout << decrypted_buf.c_str() << std::endl;
                        score += atoi(decrypted_buf.c_str());
                        ImGui::GetForegroundDrawList()->AddRectFilled(Pos, ImVec2(Pos.x + 55, Pos.y + 55), ImGui::GetColorU32(ImGuiCol_Button));
                        ImGui::GetForegroundDrawList()->AddText(ImVec2(Pos.x, Pos.y), ImColor(0, 0, 0), decrypted_buf.c_str());
                    }
                    ImGui::GetForegroundDrawList()->AddRect(Pos, ImVec2(Pos.x + 55, Pos.y + 55), ImColor(0, 0, 0));

             
                    RectInfo rect;
                    rect.Pos = ImVec2(i, j);
                    rect.size = SizeP();
                    if (Contains(rect, ImGui::GetIO().MousePos))
                    {
                        in_action = true;
                        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        {

                            for (int x = 0; x < 1; x++)
                            {
                                for (int y = 0; y < 1; y++)
                                {
                                  
                               
                                    if (RectList[i + x][j + y] == "" && counter < 3 && !open_info)
                                    {
                                      
                                        RectList[i + x][j + y] = gen_num();
                                       
                                    }
                                    

                                }
                            }
                        }

                    }
                    
                }
            }
            
            if (counter > 2)
            {
                open_info = true;
            }


            ImGui::SetCursorPosY(350);
            ImGui::Text(u8"Результат : %d", score);
            if (ImGui::Button(u8"Загрузить настройки")) read();
            ImGui::SameLine();
            if (ImGui::Button(u8"Сохранить настройки")) write();
        }
        else
        {
            ImGui::SetCursorPosY(((ImGui::GetWindowHeight()) - 25) / 2);
   
            ImGui::InputText(u8"Пин-код", global_config::pin, sizeof(global_config::pin), ImGuiInputTextFlags_Password);
            static bool bad_enter = false;
            ImGui::SameLine();
            if (ImGui::Button(u8"Вход"))
            {
                if (strcmp(global_config::pin, "2509") == 0)
                {
                    reset_game();
                    global_config::entered = true;
                    bad_enter = false;
                }
                else {
                    bad_enter = true;
                  
                }
            }
            if (bad_enter)
             ImGui::Text(u8"Неверный пин-код");
        }
        ImGui::End();
     
        if (open_info)
        {
            ImGui::Begin(u8"Игра закончена", NULL, ImGuiWindowFlags_NoResize);
            ImGui::SetWindowSize({ 200, 100 });
            
            ImGui::Text(u8"Ваш результат %d", score);

            if (ImGui::Button(u8"Ок"))
                reset_game();
            ImGui::End();
        }



        ImGui::PopFont();

        manager->RenderFrame();
      
    }
    manager->CleanAndDestroy( wc_hwnd.first, wc_hwnd.second);
 
	return 0;
}