#include <d3dx9.h>
#include <d3d9.h>
#include <math.h>
#include<windows.h>
#include<iostream>
#include<thread>
#include<process.h>
#include<d3dx9core.h>
#include<vector>
#include<random>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment (lib, "winmm.lib")
#define D3D_FVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
using namespace std;
struct data_player { FLOAT x, y, z, rhw; DWORD color; };
struct data_button { FLOAT x, y, plus_x, plus_y; };
struct data_particle { FLOAT x, y; int plus_x, plus_y; FLOAT a; DWORD color; };
struct data_monster { FLOAT x, y, plus_x, plus_y; };
struct data_dangerplace { FLOAT x, y; int a,  color, danger_time; };
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
// Desc: 全局变量
LPDIRECT3D9             g_pD3D = NULL; // Direct3D对象
LPDIRECT3DDEVICE9       Device = NULL; // Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // 顶点缓冲区对象
LPDIRECT3DINDEXBUFFER9  g_pIB = NULL; // 索引缓冲区对象
RECT					clientRect={0,0,222,222};
int choose_monster = 1000000;
LPD3DXFONT				g_pFont = 0;
vector<data_monster> monster_list;
vector<data_button> button_list;
vector<data_dangerplace> danger_list;
time_t game_time;
vector<data_particle> particle_list;
int	 dingdianshu = 360;
FLOAT plx = 300.0f, ply = 300.0f, V = 1.0f;
bool live = 1;
/*
button'r=5
player'r=7
monster'r=8
*/
DWORD color_particle() {
    int a = rand() % 3;
    if (a == 0) {
        return D3DCOLOR_XRGB(1, 255, 255, 0);
    }
    if (a == 1) {
        return D3DCOLOR_XRGB(1, 240, 0, 0);
    }
    else {
        return D3DCOLOR_XRGB(1, 255, 165, 0);
    }
}
int particle_plus() {
    if (rand() % 2 == 0) {
        return (rand() % 5);
    }
    else {
        return (-rand() % 5);
    }
}
void kButton() {
    vector<data_button>::iterator it = button_list.begin();
    for (; it != button_list.end(); ++it) {
        if ((*it).x + (*it).plus_x <= 0 || (*it).x + (*it).plus_x >= 650 || (*it).y + (*it).plus_y <= 0 || (*it).y + (*it).plus_y >= 650) {
            button_list.erase(it);
            break;
        }
        (*it).x += (*it).plus_x;
        (*it).y += (*it).plus_y;
    }
}
bool ybutton = false;
int button_i = 1;
DWORD WINAPI part_move(LPVOID pParam) {
    const float monster_v = 1.0;
    int i = 1;
    const float button_v = 3.0f;
    while (live) {
        Sleep(10);
        vector<data_particle>::iterator the = particle_list.begin();
        for (; the != particle_list.end(); ++the) {
            if ((*the).a - 0.22 >= 0) {
                (*the).a -= 0.22;
            }
            else {
                the = particle_list.erase(the);
                if (the == particle_list.end())break;
            }
            (*the).x += (*the).plus_x * 1.0 / 2.0;
            (*the).y += (*the).plus_y * 1.0 / 2.0;
        }
        if (ybutton) {
            if (!empty(monster_list)) {
                FLOAT x = plx - monster_list[choose_monster%monster_list.size()].x, y = ply - monster_list[choose_monster % monster_list.size()].y;
                FLOAT fars = sqrt(abs(x) * abs(x) + abs(y) * abs(y));
                button_list.push_back({ plx,ply,-(x / fars) * button_v,-(y / fars) * button_v });
            }
            ybutton = false;
        }
        if (i % 70 == 0) {
            if (rand() % 3 != 0)monster_list.push_back({ 321,321,0,0 });
        }
        if (i % 90 == 0) {
            if (rand() % 5 < 3)monster_list.push_back({ 321,321,0,0 });
        }
        if (i % 180 == 0) {
            i = 1;
            if (rand() % 3 != 0)monster_list.push_back({ 321,321,0,0 });
            else {
                monster_list.push_back({ 321,321,0,0 });
                monster_list.push_back({ 321,321,0,0 });
            }
            if (clock() - game_time >= 45000) {
                if (rand() % 2 == 0)monster_list.push_back({ 321,321,0,0 });
            }
            if (clock() - game_time >= 100000) {
                if (rand() % 3 != 0)monster_list.push_back({ 321,321,0,0 });
            }
        }
        ++i;
        kButton();
        vector<data_monster>::iterator it = monster_list.begin();
        for (; it != monster_list.end(); ++it)
        {
            vector<data_button>::iterator button = button_list.begin();
            if (plx >= (*it).x - 9 && plx <= (*it).x + 9 && ply >= (*it).y - 9 && ply <= (*it).y + 9) {
                live = 0;
                break;
            }
            for (; button != button_list.end(); ++button) {
                if ((*button).x >= (*it).x - 8 && (*button).x <= (*it).x + 8 && (*button).y >= (*it).y - 8 && (*button).y <= (*it).y + 8) {
                    button_i += 1;
                    FLOAT x = (*button).x, y = (*button).y;
                    button_list.erase(button);
                    if (button_i % 5 == 0) {
                        button_list.push_back({ x, y, FLOAT(1), FLOAT(0) });
                        button_list.push_back({ x, y, FLOAT(0.7), FLOAT(0.7) });
                        button_list.push_back({ x, y, FLOAT(0), FLOAT(1) });
                        button_list.push_back({ x, y, FLOAT(-1), FLOAT(0) });
                        button_list.push_back({ x, y, FLOAT(0), FLOAT(-1) });
                        button_list.push_back({ x, y, FLOAT(-0.7), FLOAT(-0.7) });
                        button_list.push_back({ x, y, FLOAT(0.7), FLOAT(-0.7) });
                        button_list.push_back({ x, y, FLOAT(-0.7), FLOAT(0.7) });
                    }
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });
                    particle_list.push_back({ (*it).x, (*it).y, particle_plus(), particle_plus(), FLOAT(4 + rand() % 10), color_particle() });

                    monster_list.erase(it);
                    goto monster_end;
                }
            }
            float x = (*it).x - plx, y = (*it).y - ply;
            float fars = sqrt(abs(x) * abs(x) + abs(y) * abs(y));
            (*it).x -= (x / fars) * monster_v;
            (*it).y -= (y / fars) * monster_v;
            vector<data_monster>::iterator its = monster_list.begin();
            for (; its != monster_list.end(); ++its) {
                if ((its) != (it))if ((*it).x <= (*its).x + 7 && (*it).x >= (*its).x - 7 && (*it).y <= (*its).y + 7 && (*it).y >= (*its).y - 7) {
                    (*it).x -= (x / fars) * monster_v;
                    (*it).y -= (y / fars) * monster_v;
                    break;
                }
            }
        }
        monster_end:
        i = i;
    }
    return (DWORD)1;
}
DWORD WINAPI other_part_move(LPVOID pParam) {
    long time_i = 0;
    while (live) {
        if (time_i >= 3000 && rand() % 4 != 0 && time_i%1000==0) {
            FLOAT x, y;int  a;
            a = 20 + rand() % 180;
            danger_list.push_back({ plx,ply,a,5,0 });
        }
        for (vector<data_dangerplace>::iterator it = danger_list.begin(); it != danger_list.end(); ++it) {
            if ((*it).color + 1 < 255)(*it).color += 1;
            else if((*it).danger_time<147) {
                (*it).danger_time += 1;
                if (plx <= (*it).x + (*it).a && ply>= (*it).x- (*it).a&&ply<= (*it).x+ (*it).a&&plx>= (*it).x- (*it).a) {
                    live = false;
                }
            }
            else {
                danger_list.erase(it);
                break;
            }
        }
        time_i++;
        Sleep(10);
    }
    return DWORD(1);
}
DWORD WINAPI other_thread(LPVOID pParam) {
    long time_i = 0;
    while (live) {
        if (choose_monster > monster_list.size())choose_monster = 0;
        if (KEY_DOWN('J')) {
            choose_monster += 1;
            Sleep(300);
        }
        Sleep(20);
    }
    return DWORD(1);
}
DWORD WINAPI kPlayer(LPVOID pParam) {
    while (live) {
        if (KEY_DOWN('W') || KEY_DOWN(VK_UP)) {
            if (ply - 2.0 * V > 0)ply -= 2.0 * V;
        }
        if (KEY_DOWN('A') || KEY_DOWN(VK_LEFT)) {
            if (plx - 2.0 * V > 0)plx -= 2.0 * V;
        }
        if (KEY_DOWN('S') || KEY_DOWN(VK_DOWN)) {
            if (ply + 2.0 * V < 640)ply += 2.0 * V;
        }
        if (KEY_DOWN('D') || KEY_DOWN(VK_RIGHT)) {
            if (plx + 2.0 * V < 640)plx += 2.0 * V;
        }
        if (KEY_DOWN(VK_SPACE) || KEY_DOWN(VK_RETURN)) {
            ybutton = true;
            if (!empty(monster_list)) Sleep(500);
        }
        Sleep(10);
    }
    return (DWORD)1;
}
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; 		// 经过坐标转换的顶点位置
    DWORD color;       			// 顶点漫反射颜色值
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


// Desc: 初始化Direct3D
HRESULT InitD3D(HWND hWnd)
{
    //创建Direct3D对象, 该对象用于创建Direct3D设备对象
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;

    //设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    //创建Direct3D设备对象
    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &Device)))
    {
        return E_FAIL;
    }
    if (FAILED(D3DXCreateFont(Device, 0, 0, 400, 0, TRUE, GB2312_CHARSET, OUT_TT_ONLY_PRECIS,DEFAULT_QUALITY, 0, L"Arail", &g_pFont)))
        return false;
    return S_OK; 
}


// Desc: 创建并填充顶点缓冲区和索引缓冲区
HRESULT InitVBAndIB()
{
    //顶点数据 
    CUSTOMVERTEX g_Vertices[361];
    g_Vertices[0].x = 300;
    g_Vertices[0].y = 250;
    g_Vertices[0].z = 0.5f;
    g_Vertices[0].rhw = 1.0f;
    g_Vertices[0].color = 0xffff0000;
    for (int i = 0; i < 360; i++)
    {
        g_Vertices[i + 1].x = (float)(200 * cos(i * 3.14159 / 180)) + 300;
        g_Vertices[i + 1].y = (float)(200 * sin(i * 3.14159 / 180)) + 250;
        g_Vertices[i + 1].z = 0.5f;
        g_Vertices[i + 1].rhw = 1.0f;
        g_Vertices[i + 1].color = 0xff00ff00;
    }

    //顶点索引数组
    //WORD g_Indices[] ={ 0,1,2, 0,2,3, 0,3,4, 0,4,5, 0,5,6, 0,6,7, 0,7,8, 0,8,1 };
    WORD g_Indices[1080];
    for (int i = 1; i < dingdianshu + 1; i++)
    {
        if (i == dingdianshu)
        {
            g_Indices[i * 3 - 3] = 0;
            g_Indices[i * 3 - 2] = i;
            g_Indices[i * 3 - 1] = 1;
        }
        else
        {
            g_Indices[i * 3 - 3] = 0;
            g_Indices[i * 3 - 2] = i;
            g_Indices[i * 3 - 1] = i + 1;
        }

    }


    //创建顶点缓冲区
    if (FAILED(Device->CreateVertexBuffer((dingdianshu + 1) * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB, NULL)))
    {
        return E_FAIL;
    }

    //填充顶点缓冲区
    VOID* pVertices;
    if (FAILED(g_pVB->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0)))
        return E_FAIL;
    memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
    g_pVB->Unlock();

    //创建索引缓冲区
    if (FAILED(Device->CreateIndexBuffer(dingdianshu * 3 * sizeof(WORD),
        0, D3DFMT_INDEX16,  //索引类型
        D3DPOOL_DEFAULT, &g_pIB, NULL)))
    {
        return E_FAIL;
    }

    //填充顶点缓冲区
    VOID* pIndices;
    if (FAILED(g_pIB->Lock(0, sizeof(g_Indices), (void**)&pIndices, 0)))
        return E_FAIL;
    memcpy(pIndices, g_Indices, sizeof(g_Indices));
    g_pIB->Unlock();

    return S_OK;
}


// Desc: 释放创建的对象
VOID Cleanup()
{
    //释放顶点缓冲区对象
    if (g_pVB != NULL)
        g_pVB->Release();

    //释放索引缓冲区对象
    if (g_pIB != NULL)
        g_pIB->Release();

    //释放Direct3D设备对象
    if (Device != NULL)
        Device->Release();

    //释放Direct3D对象
    if (g_pD3D != NULL)
        g_pD3D->Release();
}


#define D3D_FVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
// Desc: 渲染图形 
LPDIRECT3DVERTEXBUFFER9 D3DVertexBuffer = NULL;
UINT size_pl = sizeof(data_player);
data_monster monster_show;
data_particle particle_show;
data_button button_show;
HWND hwnd;
#include<string>

void end_rend() {
    int i = 0;
    do {
        i++;
        Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
        //Device.RenderState.CullMode = Cull.Clockwise;
        Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        data_player player_paint[] = {
            {plx - 7,ply - 7,0,1,D3DCOLOR_XRGB(255-i,0,0)},
            {plx,ply,0,1,D3DCOLOR_XRGB(255 - i,0,0)},
            {plx,ply + 7,0,1,D3DCOLOR_XRGB(255 - i,0,0)},
            {plx,ply,0,1,D3DCOLOR_XRGB(255 - i,0,0)},
            {plx,ply + 7,0,1,D3DCOLOR_XRGB(255 - i,0,0)},
            {plx + 7,ply - 7,0,1,D3DCOLOR_XRGB(255 - i,0,0)}
        };
        Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)player_paint, size_pl);

        Device->BeginScene();
        Device->SetFVF(D3D_FVF_VERTEX);	// 设置灵活顶点格式
        Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)player_paint, size_pl);
        Device->EndScene();


        //将在后台缓冲区绘制的图形提交到前台缓冲区显示
        Device->Present(NULL, NULL, NULL, NULL);
    } while (i < 255);
}
VOID Render()
{
        Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
        Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        data_player player_paint[] = {
            {plx - 7,ply - 7,0,1,D3DCOLOR_XRGB(255,0,0)},
            {plx,ply,0,1,D3DCOLOR_XRGB(255,0,0)},
            {plx,ply + 7,0,1,D3DCOLOR_XRGB(255,0,0)},
            {plx,ply,0,1,D3DCOLOR_XRGB(255,0,0)},
            {plx,ply + 7,0,1,D3DCOLOR_XRGB(255,0,0)},
            {plx + 7,ply - 7,0,1,D3DCOLOR_XRGB(255,0,0)}
        };

        Device->BeginScene();
        Device->SetFVF(D3D_FVF_VERTEX);	// 设置灵活顶点格式
        for (unsigned int i = 0; i < danger_list.size(); ++i)
        {
            data_dangerplace danger_show = danger_list[i];
            FLOAT a = danger_show.a;
            DWORD col = D3DCOLOR_XRGB(danger_show.color,0,0);
            if (!danger_show.danger_time) {
                data_player danger_paint[] = {
                    {danger_show.x - a,danger_show.y - a,0,1,col},
                    {danger_show.x - a,danger_show.y + a,0,1,col},
                    {danger_show.x + a,danger_show.y + a,0,1,col},
                    {danger_show.x - a,danger_show.y - a,0,1,col},
                    {danger_show.x + a,danger_show.y - a,0,1,col},
                    {danger_show.x + a,danger_show.y + a,0,1,col}
                };
                Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)danger_paint, size_pl);
            }
            else {
                data_player danger_paint[] = {
                {danger_show.x - a,danger_show.y - a,0,1,D3DCOLOR_XRGB(190,190,190)},
                {danger_show.x - a,danger_show.y + a,0,1,D3DCOLOR_XRGB(190,190,190)},
                {danger_show.x + a,danger_show.y + a,0,1,D3DCOLOR_XRGB(190,190,190)},
                {danger_show.x - a,danger_show.y - a,0,1,D3DCOLOR_XRGB(190,190,190)},
                {danger_show.x + a,danger_show.y - a,0,1,D3DCOLOR_XRGB(190,190,190)},
                {danger_show.x + a,danger_show.y + a,0,1,D3DCOLOR_XRGB(190,190,190)}
                };
            Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)danger_paint, size_pl);
            }
        }
        Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)player_paint, size_pl);
        for (unsigned int i = 0; i < monster_list.size(); ++i)
        {
            monster_show = monster_list[i];
            if(i!= choose_monster % monster_list.size()+1){
            data_player monster_paint[] = {
                {monster_show.x - 8,monster_show.y - 8,0,1,D3DCOLOR_XRGB(255,255,255)},
                {monster_show.x - 8,monster_show.y + 8,0,1,D3DCOLOR_XRGB(255,255,255)},
                {monster_show.x + 8,monster_show.y + 8,0,1,D3DCOLOR_XRGB(255,255,255)},
                {monster_show.x - 8,monster_show.y - 8,0,1,D3DCOLOR_XRGB(255,255,255)},
                {monster_show.x + 8,monster_show.y - 8,0,1,D3DCOLOR_XRGB(255,255,255)},
                {monster_show.x + 8,monster_show.y + 8,0,1,D3DCOLOR_XRGB(255,255,255)}
            };
            Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)monster_paint, size_pl);}
            else {
                data_player monster_paint[] = {
                {monster_show.x - 8,monster_show.y - 8,0,1,D3DCOLOR_XRGB(200,0,0)},
                {monster_show.x - 8,monster_show.y + 8,0,1,D3DCOLOR_XRGB(200,0,0)},
                {monster_show.x + 8,monster_show.y + 8,0,1,D3DCOLOR_XRGB(200,0,0)},
                {monster_show.x - 8,monster_show.y - 8,0,1,D3DCOLOR_XRGB(200,0,0)},
                {monster_show.x + 8,monster_show.y - 8,0,1,D3DCOLOR_XRGB(200,0,0)},
                {monster_show.x + 8,monster_show.y + 8,0,1,D3DCOLOR_XRGB(200,0,0)}
                };
                Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)monster_paint, size_pl);
            }
        }
        for (unsigned int i = 0; i < particle_list.size(); ++i)
        {
            particle_show = particle_list[i];
            FLOAT a = particle_show.a / 2;
            DWORD col = particle_show.color;
            data_player particle_paint[] = {
                {particle_show.x - a,particle_show.y - a,0,1,col},
                {particle_show.x - a,particle_show.y + a,0,1,col},
                {particle_show.x + a,particle_show.y + a,0,1,col},
                {particle_show.x - a,particle_show.y - a,0,1,col},
                {particle_show.x + a,particle_show.y - a,0,1,col},
                {particle_show.x + a,particle_show.y + a,0,1,col}
            };
            Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)particle_paint, size_pl);
        }
        for (unsigned int i = 0; i < button_list.size(); ++i)
        {
            button_show = button_list[i];
            data_player button_paint[] = {
                {button_show.x,button_show.y + 6,0,1,D3DCOLOR_XRGB(34,174,230)},
                {button_show.x - 6,button_show.y - 6,0,1,D3DCOLOR_XRGB(34,174,230)},
                {button_show.x + 6,button_show.y - 6,0,1,D3DCOLOR_XRGB(34,174,230)}
            };
            Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, (void*)button_paint, size_pl);
        }
        Device->EndScene();


        //将在后台缓冲区绘制的图形提交到前台缓冲区显示
        Device->Present(NULL, NULL, NULL, NULL);
}


// Desc: 消息处理
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
#include<fstream>
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    AllocConsole();  //create console
    SetConsoleTitle(L"玩法教学"); //set console title   
    FILE* tempFile = nullptr;
    freopen_s(&tempFile, "conin$", "r+t", stdin); //reopen the stdin, we can use std::cout.
    freopen_s(&tempFile, "conout$", "w+t", stdout);
    cout << "WASD或者方向键移动\n空格或者enter发射子弹\nJ切换目标\n碰到白色方块(怪物)死亡\n怪物出生在地图中间\n大的红色区域是天坑，变成白色后进入死亡" << endl;
    system("pause");
    fclose(stdin);
    fclose(stdout);
    FreeConsole();
    //注册窗口类
    time_t end_time;
    srand(time(NULL));
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC | CS_HREDRAW | CS_VREDRAW, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx(&wc);
    //创建窗口
    HWND hWnd = CreateWindow(L"ClassName", L"Fade2.0-from Solitairs",
        WS_OVERLAPPEDWINDOW, 200, 100, 650, 650,
        GetDesktopWindow(), NULL, wc.hInstance, NULL);
    //初始化Direct3D
    if (SUCCEEDED(InitD3D(hWnd)))
    {
        //创建并填充顶点缓冲区和索引缓冲区
        if (SUCCEEDED(InitVBAndIB()))
        {
            //显示窗口
            ShowWindow(hWnd, SW_SHOWDEFAULT);
            UpdateWindow(hWnd);

            //进入消息循环
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            HANDLE hHandle[3] = { 0,0,0 };
            MessageBox(NULL, L"WASD或者方向键移动", L"玩法教学", MB_OK | MB_SYSTEMMODAL);
            MessageBox(NULL, L"空格或Enter发射", L"玩法教学", MB_OK | MB_SYSTEMMODAL);
            MessageBox(NULL, L"J选择目标", L"玩法教学", MB_OK | MB_SYSTEMMODAL);
            hHandle[0] = CreateThread(NULL, 0, kPlayer, NULL, 0, NULL);
            hHandle[1] = CreateThread(NULL, 0, part_move, NULL, 0, NULL);
            hHandle[2] = CreateThread(NULL, 0, other_thread, NULL, 0, NULL);
            hHandle[3] = CreateThread(NULL, 0, other_part_move, NULL, 0, NULL);
            game_time = clock();
            PlaySound(TEXT("music\\undertale.wav"), NULL, SND_FILENAME | SND_ASYNC| SND_LOOP);
            while (msg.message != WM_QUIT)
            {
                if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                ;
                if(live)Render();
                else {
                    Sleep(1500);
                    end_rend();
                    Sleep(500);
                    break;
                }
            }
        }
    }
    end_time = clock();
    AllocConsole();  //create console
    SetConsoleTitle(L"总结"); //set console title   
    freopen_s(&tempFile, "conin$", "r+t", stdin); //reopen the stdin, we can use std::cout.
    freopen_s(&tempFile, "conout$", "w+t", stdout);
    system("mkdir C:\\Fade");
    ofstream file("C:\\Fade\\fadelog.txt",ios::app);
    file << int(((int)end_time - (int)game_time) / 1000)-4 << endl;
    file.close();
    cout << "您的存活时间为:" << int(((int)end_time - (int)game_time) / 1000)-4 << "s"<<endl;
    cout << "游戏结果存放在C:\\Fade\\fadelog.txt";
    system("pause");
    fclose(stdin);
    fclose(stdout);
    FreeConsole();
    UnregisterClass(L"ClassName", wc.hInstance);
    return 0;
}
