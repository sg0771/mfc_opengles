
// Opengles_MFCDlg.h: 头文件
//

#pragma once

#define GL_GLEXT_PROTOTYPES 1
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>

#pragma comment(lib,"wxEGL.lib")
#pragma comment(lib,"wxGLESv2.lib")

#include "esUtil.h"
#include <fstream>
#include <iostream>
#include <thread>

// COpenglesMFCDlg 对话框
class COpenglesMFCDlg : public CDialogEx
{
// 构造

	class OpenglesRender {
		
	const std::string strVSH = R"(
    attribute vec4 position; 
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      textureCoordinate = (inputTextureCoordinate).xy;
      gl_Position = position;
    })";
            const std::string strFSH = R"(
    varying mediump vec2 textureCoordinate;
    uniform sampler2D SamplerY;
    uniform sampler2D SamplerU;
    uniform sampler2D SamplerV;
    uniform int texture_type;
    mediump mat3 trans =
        mat3(1.0, 1.0, 1.0,
             0, -0.34414, 1.772,
             1.402, -0.71414, 0);

    void main() {
      mediump vec3 yuv;
      if (texture_type == 0) {  // yuv
        yuv.x = texture2D(SamplerY, textureCoordinate).r;
        yuv.y = texture2D(SamplerU, textureCoordinate).r - 0.5;
        yuv.z = texture2D(SamplerV, textureCoordinate).r - 0.5;
        gl_FragColor = vec4(trans * yuv, 1.0);
      } else {
        gl_FragColor = texture2D(SamplerY, textureCoordinate);
      }
    })";
		void GetXY(int srcWidth, int srcHeight, int dstWidth, int dstHeight, int& desX, int& desY) {
			desX = 0;
			desY = 0;
			int sw1 = (dstHeight * srcWidth / srcHeight) / 2 * 2;
			int sh1 = (dstWidth * srcHeight / srcWidth) / 2 * 2;
			if (sw1 <= dstWidth) {
				desX = (dstWidth - sw1) / 4 * 2;
				if (desX <= 8)
					desX = 0;
			}
			else {
				desY = (dstHeight - sh1) / 4 * 2;
				if (desY <= 8)
					desY = 0;
			}
		}
	public:
		//! 窗口句柄
		HWND        m_hWnd = nullptr;
		//! 窗口的高度
		int         m_width = 0;
		//! 窗口的宽度
		int         m_height = 0;

		/// for gles2.0
		EGLConfig   m_config = nullptr;
		EGLSurface  m_surface = EGL_NO_SURFACE;
		EGLContext  m_context = EGL_NO_CONTEXT;
		EGLDisplay  m_display = EGL_NO_DISPLAY;


		GLuint  m_program = 0;
		GLint   m_samplerY = 0;
		GLint   m_samplerU = 0;
		GLint   m_samplerV = 0;
		GLint   m_texture_type = 0;
		enum {
			TEX_Y = 0,
			TEX_U = 1,
			TEX_V = 2,
		};
		GLuint  m_textureId[4] = { 0 };

		//加载RGB/RGBA 数据到 texture 并渲染到HWND
		//OK RGB
		void    DrawRGB(uint8_t* pixels, int w, int h, int ch, int bFixed) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_Y]);

			// Load the texture
			if(ch == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			int e = glGetError();
			// Set the viewport
			//类似于BackBuffer Size
			if (bFixed) {
				int dx = 0;
				int dy = 0;
				GetXY(w, h, m_width, m_height, dx, dy);
				glViewport(dx, dy, m_width - 2 * dx, m_height - 2 * dy);//设置在窗口的显示区域
			}
			else {
				glViewport(0, 0, m_width, m_height);//设置在窗口的显示区域
			}
			e = glGetError();

			// Clear the color buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use the program object
			glUseProgram(m_program);
			glUniform1i(m_texture_type, 1);
			GLfloat vecPos[]{
				-1.0, -1.0,  // left down
				1.0,  -1.0,  // right down
				-1.0, 1.0,   // left up
				1.0,  1.0    // right up
			};
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vecPos);

			e = glGetError();

			static const GLfloat vexTex[] = {
				0.0f, 1.0f,
				1.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
			};
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, vexTex);

			e = glGetError();

			glActiveTexture(GL_TEXTURE0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			e = glGetError();

			 e = glGetError();
			eglSwapBuffers(m_display, m_surface);//渲染缓存,渲染到HWND
		}

		//加载RGB/RGBA 数据到 texture 并渲染到HWND
		void    DrawYUV(uint8_t* pixels, int w, int h,  int bFixed) {
			//加载数据
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_Y]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_U]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w/2, h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels + w*h);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_V]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w/2, h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels + w*h *5/4);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Set the viewport
			//类似于BackBuffer Size
			if (bFixed) {
				int dx = 0;
				int dy = 0;
				GetXY(w, h, m_width, m_height, dx, dy);
				glViewport(dx, dy, m_width - 2 * dx, m_height - 2 * dy);//设置在窗口的显示区域
			}
			else {
				glViewport(0, 0, m_width, m_height);//设置在窗口的显示区域
			}


			// Clear the color buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use the program object
			glUseProgram(m_program);
			glUniform1i(m_texture_type, 0);  //YUV mode
			GLfloat vecPos[]{
				-1.0, -1.0,  // left down
				1.0,  -1.0,  // right down
				-1.0, 1.0,   // left up
				1.0,  1.0    // right up
			};
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vecPos);

			static const GLfloat vexTex[] = {
				0.0f, 1.0f,
				1.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
			};
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, vexTex);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_Y]);
			glUniform1i(m_samplerY, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_U]);
			glUniform1i(m_samplerU, 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_textureId[TEX_V]);
			glUniform1i(m_samplerV, 2);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			eglSwapBuffers(m_display, m_surface);//渲染缓存,渲染到HWND
		}

		bool    Init(HWND hwnd)
		{
			m_hWnd = hwnd;
			const EGLint attribs[] =
			{
				EGL_SURFACE_TYPE,
				EGL_WINDOW_BIT,
				EGL_BLUE_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_RED_SIZE, 8,
				EGL_DEPTH_SIZE,24,
				EGL_NONE
			};
			EGLint 	format(0);
			EGLint	numConfigs(0);
			EGLint  major;
			EGLint  minor;

			//! 1
			m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			//! 2init
			eglInitialize(m_display, &major, &minor);

			//! 3
			eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs);

			eglGetConfigAttrib(m_display, m_config, EGL_NATIVE_VISUAL_ID, &format);
			//! 4 
			m_surface = eglCreateWindowSurface(m_display, m_config, hwnd, NULL);

			//! 5
			EGLint attr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
			m_context = eglCreateContext(m_display, m_config, 0, attr);
			//! 6
			if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
			{
				return false;
			}

			//缓存分辨率
			eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_width);
			eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_height);


			m_program = esLoadProgram(strVSH.c_str(), strFSH.c_str());
			if (m_program > 0) {
				m_samplerY = glGetUniformLocation(m_program, "SamplerY");
				m_samplerU = glGetUniformLocation(m_program, "SamplerU");
				m_samplerV = glGetUniformLocation(m_program, "SamplerV");
				m_texture_type = glGetUniformLocation(m_program, "texture_type");

				glGenTextures(4, m_textureId);

				return  true;
			}
			return false;
		}

		void    Deinit()
		{
			if (m_display != EGL_NO_DISPLAY)
			{
				eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
				if (m_context != EGL_NO_CONTEXT)
				{
					eglDestroyContext(m_display, m_context);
				}
				if (m_surface != EGL_NO_SURFACE)
				{
					eglDestroySurface(m_display, m_surface);
				}
				eglTerminate(m_display);
			}
			m_display = EGL_NO_DISPLAY;
			m_context = EGL_NO_CONTEXT;
			m_surface = EGL_NO_SURFACE;
			glDeleteTextures(4, m_textureId);
		}
	};

	OpenglesRender m_inst;

public:
	COpenglesMFCDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENGLES_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadfile();
	afx_msg void OnBnClickedButton1();
};
