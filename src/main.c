#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxsdk/d3d9.lib")
#pragma comment(lib, "dxsdk/d3dx9.lib")

#include <Windows.h>
#include <tchar.h>
#include <mmsystem.h>

#include <dxsdk/d3d9.h>
#include <dxsdk/d3dx9.h>

#include <stddef.h>

#define WINDOW_CLASS "MY_WINDOW_CLASS"
#define WINDOW_TITLE "DirectX Application!"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 768

LPDIRECT3D9 g_lpDirect3d = NULL;
LPDIRECT3DDEVICE9 g_lpDirect3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_lpDirect3dVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9 g_lpDirect3dIndexBuffer = NULL;
LPD3DXFONT g_lpDirect3dFont = NULL;
LPDIRECT3DTEXTURE9 g_lpDirect3dTexture = NULL;

D3DMATERIAL9 g_sMaterial;
D3DLIGHT9 g_sLight;

D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProject;

typedef struct {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT nx;
    FLOAT ny;
    FLOAT nz;
    FLOAT tu;
    FLOAT tv;
} XYZ_VERTEX;

#define XYZ_VERTEX_FVF \
    (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

void InitDirect3D(HWND);
void RenderDirect3D(void);
void TextDirect3D(LPTSTR, LONG, LONG, LONG, LONG, UINT, D3DCOLOR);
void UpdateDirect3D(void);
void ReleaseDirect3D(void);

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int iCmdShow) {
    (void)hPrevInstance;
    (void)lpszCmdLine;
    (void)iCmdShow;

    HICON hIcon = LoadIcon(NULL, IDI_APPLICATION);

    WNDCLASSEX sWindowClass;
    ZeroMemory(&sWindowClass, sizeof(WNDCLASSEX));
    {
        sWindowClass.cbSize = sizeof(WNDCLASSEX);
        sWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
        sWindowClass.lpfnWndProc = WindowProc;
        sWindowClass.cbClsExtra = 0;
        sWindowClass.cbWndExtra = 0;
        sWindowClass.hInstance = hInstance;
        sWindowClass.hIcon = hIcon;
        sWindowClass.hCursor = NULL;
        sWindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        sWindowClass.lpszMenuName = NULL;
        sWindowClass.lpszClassName = _T(WINDOW_CLASS);
        sWindowClass.hIconSm = hIcon;
    }
    RegisterClassEx(&sWindowClass);

    RECT sWindowRect;
    ZeroMemory(&sWindowRect, sizeof(RECT));
    {
        sWindowRect.left = 0;
        sWindowRect.top = 0;
        sWindowRect.right = WINDOW_WIDTH;
        sWindowRect.bottom = WINDOW_HEIGHT;
    }
    DWORD dwWindowExStyle = 0;
    DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

    AdjustWindowRectEx(&sWindowRect, dwWindowStyle, FALSE, dwWindowExStyle);
    int iRealWindowWidth = sWindowRect.right - sWindowRect.left;
    int iRealWindowHeight = sWindowRect.bottom - sWindowRect.top;

    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int iWindowX = (iScreenWidth - iRealWindowWidth) / 2;
    int iWindowY = (iScreenHeight - iRealWindowHeight) / 2;

    HWND hWindow = CreateWindowEx(dwWindowExStyle, _T(WINDOW_CLASS), _T(WINDOW_TITLE), dwWindowStyle, iWindowX, iWindowY, iRealWindowWidth, iRealWindowHeight, NULL, NULL, hInstance, NULL);

    InitDirect3D(hWindow);

    ShowWindow(hWindow, SW_SHOWDEFAULT);
    UpdateWindow(hWindow);

    MSG sMessage;
    ZeroMemory(&sMessage, sizeof(MSG));
    do {
        if (PeekMessage(&sMessage, NULL, 0, 0, PM_REMOVE)) {
            if (sMessage.message == WM_QUIT) {
                break;
            }

            TranslateMessage(&sMessage);
            DispatchMessage(&sMessage);
        }

        UpdateDirect3D();
        RenderDirect3D();
    } while (TRUE);

    ReleaseDirect3D();
    return sMessage.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWindow, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    switch (uMessage) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostMessage(hWindow, WM_CLOSE, 0, 0);
                return 0;
            }
            break;
        case WM_PAINT:
            PAINTSTRUCT sPaint;
            ZeroMemory(&sPaint, sizeof(PAINTSTRUCT));
            BeginPaint(hWindow, &sPaint);
            {
            }
            EndPaint(hWindow, &sPaint);
            return 0;
        case WM_ERASEBKGND:
            return 1;
    }

    return DefWindowProc(hWindow, uMessage, wParam, lParam);
}

void InitDirect3D(HWND hWindow) {
    g_lpDirect3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DDISPLAYMODE sDisplayMode;
    ZeroMemory(&sDisplayMode, sizeof(D3DDISPLAYMODE));
    g_lpDirect3d->lpVtbl->GetAdapterDisplayMode(g_lpDirect3d, D3DADAPTER_DEFAULT, &sDisplayMode);

    D3DPRESENT_PARAMETERS sPresentParameters;
    ZeroMemory(&sPresentParameters, sizeof(D3DPRESENT_PARAMETERS));
    {
        sPresentParameters.Windowed = TRUE;
        sPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        sPresentParameters.BackBufferFormat = sDisplayMode.Format;
        sPresentParameters.EnableAutoDepthStencil = TRUE;
        sPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    }
    g_lpDirect3d->lpVtbl->CreateDevice(g_lpDirect3d, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &sPresentParameters, &g_lpDirect3dDevice);

    g_lpDirect3dDevice->lpVtbl->SetRenderState(g_lpDirect3dDevice, D3DRS_CULLMODE, D3DCULL_CW);
    g_lpDirect3dDevice->lpVtbl->SetRenderState(g_lpDirect3dDevice, D3DRS_ZENABLE, D3DZB_TRUE);
    g_lpDirect3dDevice->lpVtbl->SetRenderState(g_lpDirect3dDevice, D3DRS_LIGHTING, TRUE);
    g_lpDirect3dDevice->lpVtbl->SetRenderState(g_lpDirect3dDevice, D3DRS_AMBIENT, 0);

    D3DXCreateTextureFromFile(g_lpDirect3dDevice, _T("res/texture.png"), &g_lpDirect3dTexture);

    XYZ_VERTEX aVertices[] = {
        {
            .x = 1.f,
            .y = -1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = -1.f,
            .tu = 1.f,
            .tv = 1.f
        }, // A
        {
            .x = 1.f,
            .y = 1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = -1.f,
            .tu = 0.f,
            .tv = 1.f
        }, // B
        {
            .x = -1.f,
            .y = 1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = -1.f,
            .tu = 0.f,
            .tv = 0.f
        }, // C
        {
            .x = -1.f,
            .y = -1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = -1.f,
            .tu = 1.f,
            .tv = 0.f
        }, // D
        {
            .x = -1.f,
            .y = -1.f,
            .z = -1.f,
            .nx = -1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 1.f
        }, // A2
        {
            .x = -1.f,
            .y = 1.f,
            .z = -1.f,
            .nx = -1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 1.f
        }, // B2
        {
            .x = -1.f,
            .y = 1.f,
            .z = 1.f,
            .nx = -1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 0.f
        }, // C2
        {
            .x = -1.f,
            .y = -1.f,
            .z = 1.f,
            .nx = -1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 0.f
        }, // D2
        {
            .x = -1.f,
            .y = -1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = 1.f,
            .tu = 1.f,
            .tv = 1.f
        }, // A3
        {
            .x = -1.f,
            .y = 1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = 1.f,
            .tu = 0.f,
            .tv = 1.f
        }, // B3
        {
            .x = 1.f,
            .y = 1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = 1.f,
            .tu = 0.f,
            .tv = 0.f
        }, // C3
        {
            .x = 1.f,
            .y = -1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = 0.f,
            .nz = 1.f,
            .tu = 1.f,
            .tv = 0.f
        }, // D3
        {
            .x = 1.f,
            .y = -1.f,
            .z = 1.f,
            .nx = 1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 1.f
        }, // A4
        {
            .x = 1.f,
            .y = 1.f,
            .z = 1.f,
            .nx = 1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 1.f
        }, // B4
        {
            .x = 1.f,
            .y = 1.f,
            .z = -1.f,
            .nx = 1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 0.f
        }, // C4
        {
            .x = 1.f,
            .y = -1.f,
            .z = -1.f,
            .nx = 1.f,
            .ny = 0.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 0.f
        }, // D4
        {
            .x = 1.f,
            .y = -1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = -1.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 1.f
        }, // A5
        {
            .x = -1.f,
            .y = -1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = -1.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 1.f
        }, // B5
        {
            .x = -1.f,
            .y = -1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = -1.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 0.f
        }, // C5
        {
            .x = 1.f,
            .y = -1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = -1.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 0.f
        }, // D5
        {
            .x = 1.f,
            .y = 1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = 1.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 1.f
        }, // A6
        {
            .x = -1.f,
            .y = 1.f,
            .z = 1.f,
            .nx = 0.f,
            .ny = 1.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 1.f
        }, // B6
        {
            .x = -1.f,
            .y = 1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = 1.f,
            .nz = 0.f,
            .tu = 0.f,
            .tv = 0.f
        }, // C6
        {
            .x = 1.f,
            .y = 1.f,
            .z = -1.f,
            .nx = 0.f,
            .ny = 1.f,
            .nz = 0.f,
            .tu = 1.f,
            .tv = 0.f
        } // D6
    };
    g_lpDirect3dDevice->lpVtbl->CreateVertexBuffer(g_lpDirect3dDevice, sizeof(aVertices), 0, XYZ_VERTEX_FVF, D3DPOOL_DEFAULT, &g_lpDirect3dVertexBuffer, NULL);
    LPVOID lpVertexBuffer = NULL;
    g_lpDirect3dVertexBuffer->lpVtbl->Lock(g_lpDirect3dVertexBuffer, 0, sizeof(aVertices), (LPVOID *)&lpVertexBuffer, 0);
    {
        CopyMemory(lpVertexBuffer, aVertices, sizeof(aVertices));
    }
    g_lpDirect3dVertexBuffer->lpVtbl->Unlock(g_lpDirect3dVertexBuffer);

    WORD aIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
    g_lpDirect3dDevice->lpVtbl->CreateIndexBuffer(g_lpDirect3dDevice, sizeof(aIndices), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_lpDirect3dIndexBuffer, NULL);
    LPVOID lpIndexBuffer = NULL;
    g_lpDirect3dIndexBuffer->lpVtbl->Lock(g_lpDirect3dIndexBuffer, 0, sizeof(aIndices), (LPVOID *)&lpIndexBuffer, 0);
    {
        CopyMemory(lpIndexBuffer, aIndices, sizeof(aIndices));
    }
    g_lpDirect3dIndexBuffer->lpVtbl->Unlock(g_lpDirect3dIndexBuffer);

    D3DXCreateFont(g_lpDirect3dDevice, 30, 20, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("AcPlus IBM BIOS"), &g_lpDirect3dFont);

    ZeroMemory(&g_sMaterial, sizeof(D3DMATERIAL9));
    {
        g_sMaterial.Diffuse.r = 1.f;
        g_sMaterial.Diffuse.g = 1.f;
        g_sMaterial.Diffuse.b = 1.f;
        g_sMaterial.Diffuse.a = 1.f;

        g_sMaterial.Ambient.r = 1.f;
        g_sMaterial.Ambient.g = 1.f;
        g_sMaterial.Ambient.b = 1.f;
        g_sMaterial.Ambient.a = 1.f;
    }

    ZeroMemory(&g_sLight, sizeof(D3DLIGHT9));
    {
        g_sLight.Type = D3DLIGHT_DIRECTIONAL;
        g_sLight.Diffuse.r = 1.f;
        g_sLight.Diffuse.g = 1.f;
        g_sLight.Diffuse.b = 1.f;
    }
    D3DXVECTOR3 vLightDirection;
    ZeroMemory(&vLightDirection, sizeof(D3DXVECTOR3));
    {
        vLightDirection.x = 0.f;
        vLightDirection.y = 0.f;
        vLightDirection.z = 1.f;
    }
    D3DXVec3Normalize((LPD3DXVECTOR3)&g_sLight.Direction, &vLightDirection);

    ZeroMemory(&g_mWorld, sizeof(D3DXMATRIX));
    ZeroMemory(&g_mView, sizeof(D3DXMATRIX));
    ZeroMemory(&g_mProject, sizeof(D3DXMATRIX));

    D3DXVECTOR3 vEye;
    ZeroMemory(&vEye, sizeof(D3DXVECTOR3));
    {
        vEye.x = 0.f;
        vEye.y = 0.f;
        vEye.z = -8.f;
    }
    D3DXVECTOR3 vAt;
    ZeroMemory(&vAt, sizeof(D3DXVECTOR3));
    {
        vAt.x = 0.f;
        vAt.y = 0.f;
        vAt.z = 0.f;
    }
    D3DXVECTOR3 vUp;
    ZeroMemory(&vUp, sizeof(D3DXVECTOR3));
    {
        vUp.x = 0.f;
        vUp.y = 1.f;
        vUp.z = 0.f;
    }
    D3DXMatrixLookAtLH(&g_mView, &vEye, &vAt, &vUp);

    D3DXMatrixPerspectiveFovLH(&g_mProject, D3DX_PI / 4.f, 1.f, 1.f, 100.f);
}

void RenderDirect3D(void) {
    g_lpDirect3dDevice->lpVtbl->Clear(g_lpDirect3dDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(16, 16, 16), 1.f, 0);
    g_lpDirect3dDevice->lpVtbl->BeginScene(g_lpDirect3dDevice);
    {
        g_lpDirect3dDevice->lpVtbl->SetMaterial(g_lpDirect3dDevice, &g_sMaterial);

        g_lpDirect3dDevice->lpVtbl->SetLight(g_lpDirect3dDevice, 0, &g_sLight);
        g_lpDirect3dDevice->lpVtbl->LightEnable(g_lpDirect3dDevice, 0, TRUE);

        g_lpDirect3dDevice->lpVtbl->SetTransform(g_lpDirect3dDevice, D3DTS_WORLD, &g_mWorld);
        g_lpDirect3dDevice->lpVtbl->SetTransform(g_lpDirect3dDevice, D3DTS_VIEW, &g_mView);
        g_lpDirect3dDevice->lpVtbl->SetTransform(g_lpDirect3dDevice, D3DTS_PROJECTION, &g_mProject);

        g_lpDirect3dDevice->lpVtbl->SetStreamSource(g_lpDirect3dDevice, 0, g_lpDirect3dVertexBuffer, 0, sizeof(XYZ_VERTEX));
        g_lpDirect3dDevice->lpVtbl->SetFVF(g_lpDirect3dDevice, XYZ_VERTEX_FVF);
        g_lpDirect3dDevice->lpVtbl->SetIndices(g_lpDirect3dDevice, g_lpDirect3dIndexBuffer);
        g_lpDirect3dDevice->lpVtbl->SetTexture(g_lpDirect3dDevice, 0, (LPDIRECT3DBASETEXTURE9)g_lpDirect3dTexture);
        g_lpDirect3dDevice->lpVtbl->SetTextureStageState(g_lpDirect3dDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g_lpDirect3dDevice->lpVtbl->SetTextureStageState(g_lpDirect3dDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        g_lpDirect3dDevice->lpVtbl->DrawIndexedPrimitive(g_lpDirect3dDevice, D3DPT_TRIANGLELIST, 0, 0, 36, 0, 12);

        TextDirect3D(_T("Hey, Cutie! ^^"), 10, 10, WINDOW_WIDTH, WINDOW_HEIGHT, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 193, 203));
    }
    g_lpDirect3dDevice->lpVtbl->EndScene(g_lpDirect3dDevice);
    g_lpDirect3dDevice->lpVtbl->Present(g_lpDirect3dDevice, NULL, NULL, NULL, NULL);
}

void TextDirect3D(LPWSTR text, LONG x, LONG  y, LONG x1, LONG y1, UINT format, D3DCOLOR color) {
    RECT sTextArea;
    ZeroMemory(&sTextArea, sizeof(RECT));
    {
        sTextArea.left = x;
        sTextArea.top = y;
        sTextArea.right = x1;
        sTextArea.bottom = y1;
    }
    g_lpDirect3dFont->lpVtbl->DrawText(g_lpDirect3dFont, NULL, text, -1, &sTextArea, format, color);
}

void UpdateDirect3D(void) {
    D3DXMATRIX mWorldX;
    ZeroMemory(&mWorldX, sizeof(D3DXMATRIX));
    D3DXMATRIX mWorldY;
    ZeroMemory(&mWorldY, sizeof(D3DXMATRIX));

    DWORD dwTime = timeGetTime() % 5000;
    FLOAT fAngle = dwTime * (2.f * D3DX_PI) / 5000.f;

    D3DXMatrixRotationX(&mWorldX, fAngle);
    D3DXMatrixRotationY(&mWorldY, fAngle);
    D3DXMatrixMultiply(&g_mWorld, &mWorldX, &mWorldY);
}

void ReleaseDirect3D(void) {
    if (g_lpDirect3dFont) {
        g_lpDirect3dFont->lpVtbl->Release(g_lpDirect3dFont);
    }
    if (g_lpDirect3dIndexBuffer) {
        g_lpDirect3dIndexBuffer->lpVtbl->Release(g_lpDirect3dIndexBuffer);
    }
    if (g_lpDirect3dVertexBuffer) {
        g_lpDirect3dVertexBuffer->lpVtbl->Release(g_lpDirect3dVertexBuffer);
    }
    if (g_lpDirect3dTexture) {
        g_lpDirect3dTexture->lpVtbl->Release(g_lpDirect3dTexture);
    }
    if (g_lpDirect3dDevice) {
        g_lpDirect3dDevice->lpVtbl->Release(g_lpDirect3dDevice);
    }
    if (g_lpDirect3d) {
        g_lpDirect3d->lpVtbl->Release(g_lpDirect3d);
    }
}
