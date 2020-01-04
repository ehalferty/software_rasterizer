#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>

#define CLASS_NAME "NVWorldEdit"
#define WINDOW_TITLE "NVWorldEdit v0.0.1subalpha"
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN

#define FILE_EXIT 100

HWND window = NULL;
HMENU menubar = NULL;
HMENU fileMenu = NULL;

INT filled = 0;
INT winWidth = 0, winHeight = 0;
FLOAT ax = 0, ay = 0, bx = 0, by = 0, cx = 0, cy = 0;
HDC memoryDC;

void Interpolator(
     float Px, float Py
    ,float Ax, float Ay, float Bx, float By, float Cx, float Cy
    ,float A0, float A1, float A2, float A3
    ,float B0, float B1, float B2, float B3
    ,float C0, float C1, float C2, float C3
    ,float &u, float &v
    ,bool &inside
    ,float &I0, float &I1, float &I2, float &I3
) {
    float cx, cy, bx, by, px, py, cc, bc, pc, bb, pb, denom, a, w;
    // 1
    cx = 1.0f * Cx - Ax;
    cy = 1.0f * Cy - Ay;
    bx = 1.0f * Bx - Ax;
    by = 1.0f * By - Ay;
    px = 1.0f * Px - Ax;
    // printf("======  1  ======\n");
    // printf("cx = %f\n", cx);
    // printf("cy = %f\n", cy);
    // printf("bx = %f\n", bx);
    // printf("by = %f\n", by);
    // printf("px = %f\n", px);
    // printf("py = %f\n", py);

    // 2
    py = 1.0f * Py - Ay;
    // printf("======  2  ======\n");
    // printf("py = %f\n", py);
    // printf("bc = %f\n", bc);
    // printf("pc = %f\n", pc);
    // printf("bb = %f\n", bb);
    // printf("pb = %f\n", pb);

    // 3
    cc = cx * cx + 0.0f;
    bc = cx * bx + 0.0f;
    pc = cx * px + 0.0f;
    bb = bx * bx + 0.0f;
    pb = px * bx + 0.0f;
    // printf("======  3  ======\n");
    // printf("cc = %f\n", cc);
    // printf("bc = %f\n", bc);
    // printf("pc = %f\n", pc);
    // printf("bb = %f\n", bb);
    // printf("pb = %f\n", pb);

    // 4
    cc = cy * cy + cc;
    bc = cy * by + bc;
    pc = cy * py + pc;
    bb = by * by + bb;
    pb = py * by + pb;
    // printf("======  4  ======\n");
    // printf("cc = %f\n", cc);
    // printf("bc = %f\n", bc);
    // printf("pc = %f\n", pc);
    // printf("bb = %f\n", bb);
    // printf("pb = %f\n", pb);

    // 5
    denom = cc * bb + 0.0f;
    u = bb * pc + 0.0f;
    v = cc * pb + 0.0f;
    // printf("======  5  ======\n");
    // printf("denom = %f\n", denom);
    // printf("u = %f\n", u);
    // printf("v = %f\n", v);

    // 6
    denom = -(bc * bc) + denom;
    u = -(bc * pb) + u;
    v = -(bc * pc) + v;
    // printf("======  6  ======\n");
    // printf("denom = %f\n", denom);
    // printf("u = %f\n", u);
    // printf("v = %f\n", v);

    // 7
    denom = 1.0f / denom;
    // printf("======  7  ======\n");
    // printf("denom = %f\n", denom);

    // 8
    u = u * denom;
    v = v * denom;
    // printf("======  8  ======\n");
    // printf("u = %f\n", u);
    // printf("v = %f\n", v);

    // 9
    a = 1.0f * 1.0f - u;
    I0 = A0 * u + 0.0f;
    I1 = A1 * u + 0.0f;
    I2 = A2 * u + 0.0f;
    I3 = A3 * u + 0.0f;
    // printf("======  9  ======\n");
    // printf("a = %f\n", a);
    // printf("I0 = %f\n", I0);
    // printf("I1 = %f\n", I1);
    // printf("I2 = %f\n", I2);
    // printf("I3 = %f\n", I3);

    // 10
    w = 1.0f * a - v;
    I0 = B0 * v + I0;
    I1 = B1 * v + I1;
    I2 = B2 * v + I2;
    I3 = B3 * v + I3;
    // printf("======  10  ======\n");
    // printf("w = %f\n", w);
    // printf("I0 = %f\n", I0);
    // printf("I1 = %f\n", I1);
    // printf("I2 = %f\n", I2);
    // printf("I3 = %f\n", I3);

    // 11
    I0 = C0 * w + I0;
    I1 = C1 * w + I1;
    I2 = C2 * w + I2;
    I3 = C3 * w + I3;
    // printf("======  11  ======\n");
    // printf("I0 = %f\n", I0);
    // printf("I1 = %f\n", I1);
    // printf("I2 = %f\n", I2);
    // printf("I3 = %f\n", I3);
    inside = u >= 0.0f && v >= 0.0f && w >= 0.0f;
}

LONG WINAPI WindowMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    static PAINTSTRUCT paintStruct;
    FLOAT xTemp = 0, yTemp = 0;
    RECT winDimsTemp;
    HDC dc;
    switch (message) {
        case WM_CREATE:
            menubar = CreateMenu();
            fileMenu = CreateMenu();
            AppendMenu(fileMenu, MF_STRING, FILE_EXIT, "E&xit");
            AppendMenu(menubar, MF_POPUP, (UINT_PTR)fileMenu, "&File");
            SetMenu(window, menubar);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case FILE_EXIT:
                    PostQuitMessage(0);
                    break;
                default:
                    break;
            }
            break;
        case WM_SIZE:
          winWidth = LOWORD(lParam);
          winHeight = HIWORD(lParam);
          break;
        case WM_PAINT:
            dc = BeginPaint(window, &paintStruct);
            // Outputs
            float u;
            float v;
            bool inside;
            float ir;
            float ig;
            float ib;
            float iz;
            for (int y = 0; y < winHeight; y++) {
                for (int x = 0; x < winWidth; x++) {
                    if (filled == 3) {
                        Interpolator(
                            x, y, ax, ay, bx, by, cx, cy,
                            1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                            u, v,
                            inside,
                            ir, ig, ib, iz
                        );
                        if (inside) {
                            SetPixel(dc, x, y, RGB(ir * 255, ig * 255, ib * 255));
                        } else {
                            SetPixel(dc, x, y, RGB(0, 0, 0));
                        }
                    } else {
                        SetPixel(dc, x, y, RGB(0, 0, 0));
                    }
                }
            }
            EndPaint(window, &paintStruct);
            break;
        case WM_LBUTTONDOWN:
            xTemp = GET_X_LPARAM(lParam);
            yTemp = GET_Y_LPARAM(lParam);
            if (filled == 0 || filled == 3) {
                ax = xTemp;
                ay = yTemp;
                filled = 1;
            } else if (filled == 1) {
                bx = xTemp;
                by = yTemp;
                filled = 2;
            } else if (filled == 2) {
                cx = xTemp;
                cy = yTemp;
                filled = 3;
            }
            RECT rectTemp;
            rectTemp.left = 0;
            rectTemp.top = 0;
            rectTemp.right = winWidth;
            rectTemp.bottom = winHeight;
            InvalidateRect(window, &rectTemp, TRUE);
            RedrawWindow(window, NULL, NULL, NULL);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return (LONG)DefWindowProc(window, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPTSTR commandLine, int commandShow) {
    MSG message;
    WNDCLASS windowClass;
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = (WNDPROC)WindowMessageHandler;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = instance;
    windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = CLASS_NAME;
    RegisterClass(&windowClass);
    window = CreateWindow(CLASS_NAME, WINDOW_TITLE, WINDOW_STYLE, 0, 0, 640, 480, NULL, NULL, instance, NULL);
    ShowWindow(window, commandShow);
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return (int)message.wParam;
}



            // RECT rectTemp;
            // rectTemp.left = 0;
            // rectTemp.top = 0;
            // rectTemp.right = winWidth;
            // rectTemp.bottom = winHeight;
            // InvalidateRect(window, &rectTemp, TRUE);
            // RedrawWindow(window, NULL, NULL, NULL);
            // MessageBox(0, "And text here", "MessageBox caption", MB_OK);
    // while (!done && message.message != WM_QUIT) {
    //     if (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
    //         TranslateMessage(&message);
    //         DispatchMessage(&message);
    //     } else {
    //         Sleep(50);
    //     }
    // }
//HWND window;
//HDC deviceContext;
//HGLRC openGLRenderingContext;
//POINT windowSize = { 4, 3 };
//BOOL perspectiveChanged = FALSE;
//BOOL active = FALSE;
//BOOL done = FALSE;
//GLuint vertexArrayObject;
//GLuint vertexBufferObject;
//GLfloat points[] = {
//        0.0f, 0.5f, 0.0f,
//        0.5f, -0.5f, 0.0f,
//        -0.5f, -0.5f, 0.0f
//};
//const char* vertex_shader = ""
//                            "#version 410\n"
//                            "in vec3 vp;"
//                            "void main () {"
//                            "	gl_Position = vec4 (vp, 1.0);"
//                            "}";
//const char* fragment_shader = ""
//                              "#version 410\n"
//                              "out vec4 frag_colour;"
//                              "void main () {"
//                              "   frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
//                              "}";
//GLuint vertexShader, fragmentShader;
//GLuint shaderProgram;
//
//void (*glGenBuffers)(GLsizei n, GLuint * buffers);
//void (*glBindBuffer)(GLenum target, GLuint buffer);
//void (*glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data,
//                     GLenum usage);
//void (*glGenVertexArrays)(GLsizei n, GLuint *arrays);
//void (*glBindVertexArray)(GLuint array);
//void (*glEnableVertexAttribArray)(GLuint index);
//void (*glVertexAttribPointer)(GLuint index, GLint size, GLenum type,
//                              GLboolean normalized, GLsizei stride,
//                              const GLvoid * pointer);
//GLuint (*glCreateShader)(GLenum shaderType);
//void (*glShaderSource)(GLuint shader, GLsizei count, const GLchar **string,
//                       const GLint *length);
//void (*glCompileShader)(GLuint shader);
//GLuint (*glCreateProgram)();
//void (*glAttachShader)(GLuint program, GLuint shader);
//void (*glLinkProgram)(GLuint program);
//void (*glUseProgram)(GLuint program);
//
//void LoadOpenGLFunctions() {
//    glGenBuffers = (void (*)(GLsizei, GLuint *))
//            wglGetProcAddress("glGenBuffers");
//    glBindBuffer = (void (*)(GLenum, GLuint))
//            wglGetProcAddress("glBindBuffer");
//    glBufferData = (void (*)(GLenum, GLsizeiptr, const GLvoid *, GLenum))
//            wglGetProcAddress("glBufferData");
//    glGenVertexArrays = (void (*)(GLsizei, GLuint *))
//            wglGetProcAddress("glGenVertexArrays");
//    glBindVertexArray = (void (*)(GLuint))
//            wglGetProcAddress("glBindVertexArray");
//    glEnableVertexAttribArray = (void (*)(GLuint))
//            wglGetProcAddress("glEnableVertexAttribArray");
//    glVertexAttribPointer = (void (*)(GLuint, GLint, GLenum, GLboolean, GLsizei,
//                                      const GLvoid *))
//            wglGetProcAddress("glVertexAttribPointer");
//    glCreateShader = (GLuint (*)(GLenum))
//            wglGetProcAddress("glCreateShader");
//    glShaderSource = (void (*)(GLuint, GLsizei, const GLchar **, const GLint *))
//            wglGetProcAddress("glShaderSource");
//    glCompileShader = (void (*)(GLuint))
//            wglGetProcAddress("glCompileShader");
//    glCreateProgram = (GLuint (*)())
//            wglGetProcAddress("glCreateProgram");
//    glAttachShader = (void (*)(GLuint, GLuint))
//            wglGetProcAddress("glAttachShader");
//    glLinkProgram = (void (*)(GLuint))
//            wglGetProcAddress("glLinkProgram");
//    glUseProgram = (void (*)(GLuint))
//            wglGetProcAddress("glUseProgram");
//}
//
//
//
//LONG WINAPI WindowProc(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//    static PAINTSTRUCT paintStruct;
//    switch (uMsg) {
//        case WM_PAINT:
////            Render();
//            BeginPaint(window, &paintStruct);
//            EndPaint(window, &paintStruct);
//            return 0;
//        case WM_SIZE:
//            windowSize.x = LOWORD(lParam);
//            windowSize.y = HIWORD(lParam);
//            PostMessage(window, WM_PAINT, 0, 0);
//            perspectiveChanged = TRUE;
////            glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
////            PostMessage(window, WM_PAINT, 0, 0);
//            return 0;
//        case WM_SYSCOMMAND:
//            if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER) {
//                return 0;
//            }
//            break;
//        case WM_ACTIVATE:
//            active = !HIWORD(wParam);
//            return 0;
//        case WM_KEYDOWN:
//        case WM_SYSKEYDOWN:
////            keys[wParam] = TRUE;
//            return 0;
//        case WM_KEYUP:
//        case WM_SYSKEYUP:
////            keys[wParam] = FALSE;
//            return 0;
//        case WM_CLOSE:
//            PostQuitMessage(0);
//            return 0;
//        default:
//            break;
//    }
//    return (LONG)DefWindowProc(window, uMsg, wParam, lParam);
//}
//
//int WINAPI WinMain(
//        HINSTANCE hInstance,
//        HINSTANCE hPrevInstance,
//        LPTSTR lpCmdLine,
//        int nCmdShow) {
//    MSG   message;
//    WNDCLASS wc;
//    int pf;
//    PIXELFORMATDESCRIPTOR pfd;
//    hInstance = GetModuleHandle(NULL);
//    wc.style = CS_OWNDC;
//    wc.lpfnWndProc = (WNDPROC)WindowProc;
//    wc.cbClsExtra = 0;
//    wc.cbWndExtra = 0;
//    wc.hInstance = hInstance;
//    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
//    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wc.hbrBackground = NULL;
//    wc.lpszMenuName = NULL;
//    wc.lpszClassName = "WinParabolicRealms";
//    RegisterClass(&wc);
//    window = CreateWindow("WinParabolicRealms", "Hi there", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);
//    deviceContext = GetDC(window);
//    memset(&pfd, 0, sizeof(pfd));
//    pfd.nSize = sizeof(pfd);
//    pfd.nVersion = 1;
//    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
//    pfd.iPixelType = PFD_TYPE_RGBA;
//    pfd.cColorBits = 32;
//    pf = ChoosePixelFormat(deviceContext, &pfd);
//    SetPixelFormat(deviceContext, pf, &pfd);
//    DescribePixelFormat(deviceContext, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
//    ReleaseDC(window, deviceContext);
//    deviceContext = GetDC(window);
//    openGLRenderingContext = wglCreateContext(deviceContext);
//    wglMakeCurrent(deviceContext, openGLRenderingContext);
//    ShowWindow(window, nCmdShow);
//
//    LoadOpenGLFunctions();
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);
//    glGenBuffers(1, &vertexBufferObject);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
//    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof (GLfloat), points, GL_STATIC_DRAW);
//    glGenVertexArrays(1, &vertexArrayObject);
//    glBindVertexArray(vertexArrayObject);
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//    vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
//    glCompileShader(vertexShader);
//    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
//    glCompileShader(fragmentShader);
//    shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, fragmentShader);
//    glAttachShader(shaderProgram, vertexShader);
//    glLinkProgram(shaderProgram);
//
//    while (!done && message.message != WM_QUIT) {
//        if(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
//            TranslateMessage(&message);
//            DispatchMessage(&message);
//        } else {
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            glUseProgram(shaderProgram);
//            glBindVertexArray(vertexArrayObject);
//            glDrawArrays(GL_TRIANGLES, 0, 3);
//            glFlush();
//            SwapBuffers(deviceContext);
//            /*
//            if (active) {
//                if (keys[VK_ESCAPE]) {
//                    done = true;
//                }
////                Render();
//
//                if (perspectiveChanged) {
//                    // Adjust projectionMatrix
//                    // TODO: GLM clone in pure c
//                    perspectiveChanged = FALSE;
//                }
//            }*/
//            // Wait what is this for again?
//            // TODO: Recover your timer stuff.
//            Sleep(50);
//        }
//    }
//
//    wglMakeCurrent(NULL, NULL);
//    ReleaseDC(window, deviceContext);
//    wglDeleteContext(openGLRenderingContext);
//    DestroyWindow(window);
//    return (int)message.wParam;
//}
