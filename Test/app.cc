/*
输入内存数据
显示到HWND或者回调到内存上
确保内存问题正常
*/

#include <GLEW/GLEW.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "core/core_gpupixel.h"
#pragma comment(lib,"GLImage.lib")
#pragma comment(lib,"Opengl32.lib")
#include "WXBase.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

class WXFilter {
    int m_iWidth = 0;
    int m_iHeight = 0;

    GLFWwindow* m_pWindow = nullptr;
    std::shared_ptr<gpupixel::BeautyFaceFilter> m_pFilter;
    std::shared_ptr<gpupixel::SourceImage> m_pSource; //图像输入源
    std::shared_ptr<gpupixel::TargetRawDataOutput> m_pOutput;//数据输出
    std::shared_ptr<gpupixel::TargetView> m_pTarget;     //输出预览

    std::thread* m_thread = nullptr;
    std::string m_name = "";
    bool m_bSupportGL = false;
    bool m_bInit = false;

    void work() {

       gpupixel::GPUPixelContext::getInstance()->run([=] {
           m_pWindow = gpupixel::GPUPixelContext::getInstance()->GetGLContext();
           if (m_pWindow == NULL){
               m_bInit = false;
               glfwTerminate();
               return;
           }
           int ret = glewInit();
           if (ret != GLEW_OK){
               m_bInit = false;
               m_pWindow = NULL;//Error
               glfwTerminate();
               return;
           }

           glfwShowWindow(m_pWindow);
           glfwMakeContextCurrent(m_pWindow);
           glfwSetFramebufferSizeCallback(m_pWindow, framebuffer_size_callback);//縮放回調
           m_pSource = gpupixel::SourceImage::create(m_name.c_str());//输入Source
           m_pTarget = std::make_shared<gpupixel::TargetView>();//显示
           m_pFilter = gpupixel::BeautyFaceFilter::create();
           //处理链路
           m_pSource->addTarget(m_pFilter)->addTarget(m_pTarget);
           int w = m_pSource->getRotatedFramebufferWidth();
           int h = m_pSource->getRotatedFramebufferHeight();
           m_pTarget->onSizeChanged(w, h); //Change Size For Input

           m_bSupportGL = true;
           m_bInit = true;
            glfwMakeContextCurrent(m_pWindow);

            while (!glfwWindowShouldClose(m_pWindow))
            {
                m_pFilter->setBlurAlpha(2.0);//磨皮参数 0 - 1.0
                m_pFilter->setWhite(0.4);//磨皮参数 0 - 1.0
                m_pSource->Render(); //Processs Data
                glfwSwapBuffers(m_pWindow);  //Show Data
                glfwPollEvents();
            }
       });
       // glfwTerminate();
    }
public:
    WXFilter() {}
    bool init(const char* name) {
        m_name = name;
        m_thread = new std::thread(&WXFilter::work, this);
        while (true)
        {
            if (m_bInit) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return m_bSupportGL;
    }
};


//OpenGL 工作线程
class GLThread : public WXThread
{
public:
    static std::shared_ptr<GLThread> s_WorkThread;
    static void Init() {
        if (s_WorkThread.get() == nullptr)
            s_WorkThread = std::shared_ptr<GLThread>(new GLThread);
    }
    static GLThread* GetInst() {
        return s_WorkThread.get();
    }
    GLThread() {
        ThreadSetName(L"GLThread");
        ThreadStart(true);
    }
    virtual ~GLThread() {
        ThreadStop();
        return;
    }
    virtual void ThreadProcess() {
        SLEEPMS(1);
    }
};

std::shared_ptr<GLThread> GLThread::s_WorkThread;
void _RunTask(std::function<void(void)>func) {
    GLThread::GetInst()->RunTask(func);
}

int main(int argv, char** argc)
{

    GLThread::Init();
    SetRunTaskCb(_RunTask);

    WXFilter* filter = new WXFilter;
    filter->init("a.jpg");
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}



