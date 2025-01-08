
// Opengles_MFCDlg.h: 头文件
//

#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#pragma comment(lib,"libEGL.lib")
#pragma comment(lib,"libGLESv2.lib")

#include "esUtil.h"
#include <fstream>
#include <iostream>
#include <thread>

// COpenglesMFCDlg 对话框
class COpenglesMFCDlg : public CDialogEx
{
// 构造


	class OpenglesRender {
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

		GLuint  m_programObject = 0;
		GLint   m_samplerLoc = 0;
		GLuint  m_textureId = 0;

		void    DrawRGB(uint8_t* pixels, int w, int h, int ch, int bFixed) {
			
			// Use tightly packed data
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// Generate a texture object
			glGenTextures(1, &m_textureId);

			// Bind the texture object
			glBindTexture(GL_TEXTURE_2D, m_textureId);

			// Load the texture
			if(ch == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

			// Set the filtering mode
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

			RenderRGB(w, h, bFixed);
		}

		void    RenderRGB(int w, int h, int bFixed) {
			GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,  // Position 0
							   0.0f,  0.0f,        // TexCoord 0 
							  -1.0f, -1.0f, 0.0f,  // Position 1
							   0.0f,  1.0f,        // TexCoord 1
							   1.0f, -1.0f, 0.0f,  // Position 2
							   1.0f,  1.0f,        // TexCoord 2
							   1.0f,  1.0f, 0.0f,  // Position 3
							   1.0f,  0.0f         // TexCoord 3
			};
	

			// Set the viewport
			//类似于BackBuffer Size
			if (bFixed) {
				int dx = 0;
				int dy = 0;
				GetXY(w, h, m_width, m_height, dx, dy);

				glViewport(dx, dy, m_width-2*dx, m_height- 2*dy);//设置在窗口的显示区域
			}
			else
				glViewport(0, 0, m_width, m_height);//设置在窗口的显示区域

			// Clear the color buffer
			glClear(GL_COLOR_BUFFER_BIT);

			// Use the program object
			glUseProgram(m_programObject);

			// Load the vertex position
			glVertexAttribPointer(0, 3, GL_FLOAT,
				GL_FALSE, 5 * sizeof(GLfloat), vVertices);
			// Load the texture coordinate
			glVertexAttribPointer(1, 2, GL_FLOAT,
				GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureId);
			glUniform1i(m_samplerLoc, 0);
			GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
			//考虑导出
			eglSwapBuffers(m_display, m_surface);//渲染缓存,渲染到HWND
		}

		bool    Init(HWND hwnd)
		{
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

			char vShaderStr[] =
				"#version 300 es                            \n"
				"layout(location = 0) in vec4 a_position;   \n"
				"layout(location = 1) in vec2 a_texCoord;   \n"
				"out vec2 v_texCoord;                       \n"
				"void main()                                \n"
				"{                                          \n"
				"   gl_Position = a_position;               \n"
				"   v_texCoord = a_texCoord;                \n"
				"}                                          \n";

			char fShaderStr[] =
				"#version 300 es                                     \n"
				"precision mediump float;                            \n"
				"in vec2 v_texCoord;                                 \n"
				"layout(location = 0) out vec4 outColor;             \n"
				"uniform sampler2D s_texture;                        \n"
				"void main()                                         \n"
				"{                                                   \n"
				"  outColor = texture( s_texture, v_texCoord );      \n"
				"}                                                   \n";

			// Load the shaders and get a linked program object
			m_programObject = esLoadProgram(vShaderStr, fShaderStr);

			// Get the sampler location
			m_samplerLoc = glGetUniformLocation(m_programObject, "s_texture");


			return  true;

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
			m_textureId = 0;
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
};
