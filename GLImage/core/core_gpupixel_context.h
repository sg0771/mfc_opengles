/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <mutex>
#include "core_framebuffer_cache.h"
#include "core_gpupixel_macros.h"
#include "core/filter.h"
#include "core_gl_program.h"

#if defined(GPUPIXEL_ANDROID)
  typedef struct _gpu_context_t {
    EGLDisplay egldisplay;
    EGLSurface eglsurface;
    EGLContext eglcontext;
  } _gpu_context_t;
#endif
 
 //使用自定线程
typedef void(*RunTask)(std::function<void(void)>func);
void SetRunTaskCb(RunTask cb);

NS_GPUPIXEL_BEGIN

class GPUPixelContext {
 public:
  static GPUPixelContext* getInstance();
  static void destroy();

  FramebufferCache* getFramebufferCache() const;
  //todo(zhaoyou)
  void setActiveShaderProgram(GLProgram* shaderProgram);
  void purge();

  void run(std::function<void(void)> func);

  void useAsCurrent(void);
  void presentBufferForDisplay();
 
#if defined(GPUPIXEL_IOS)
  EAGLContext* getEglContext() const { return _eglContext; };
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* getOpenGLContext() const { return imageProcessingContext; };
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* GetGLContext() const { return gl_context_; };
#endif
 
  // used for capturing a processed frame data
  bool isCapturingFrame;
  std::shared_ptr<Filter> captureUpToFilter;
  unsigned char* capturedFrameData;
  int captureWidth;
  int captureHeight;

 private:
  GPUPixelContext();
  ~GPUPixelContext();

  void init();

  void createContext();
  void releaseContext();
 private:
  static GPUPixelContext* _instance;
  static std::mutex _mutex;
  FramebufferCache* _framebufferCache = nullptr;
  GLProgram* _curShaderProgram = nullptr;
  //std::shared_ptr<DispatchQueue> m_task_queue;//任务队列
  
#if defined(GPUPIXEL_ANDROID)
  bool context_inited = false;
  int m_surfacewidth;
  int m_surfaceheight;
  _gpu_context_t* m_gpu_context;
#elif defined(GPUPIXEL_IOS)
  EAGLContext* _eglContext;
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* imageProcessingContext;
  NSOpenGLPixelFormat* _pixelFormat;
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* gl_context_ = nullptr;
#endif

};

NS_GPUPIXEL_END
