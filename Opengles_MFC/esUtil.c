// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 3.0 Programming Guide.
//

///
//  Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "esUtil.h"
//#include "esUtil_win.h"

#ifdef ANDROID
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
typedef AAsset esFile;
#else
typedef FILE esFile;
#endif

#ifdef __APPLE__
#include "FileWrapper.h"
#endif

///
//  Macros
//
#define INVERTED_BIT            (1 << 5)

///
//  Types
//
#ifndef __APPLE__
#pragma pack(push,x1)                            // Byte alignment (8-bit)
#pragma pack(1)
#endif

typedef struct
#ifdef __APPLE__
__attribute__ ( ( packed ) )
#endif
{
   unsigned char  IdSize,
            MapType,
            ImageType;
   unsigned short PaletteStart,
            PaletteSize;
   unsigned char  PaletteEntryDepth;
   unsigned short X,
            Y,
            Width,
            Height;
   unsigned char  ColorDepth,
            Descriptor;

} TGA_HEADER;

#ifndef __APPLE__
#pragma pack(pop,x1)
#endif

#ifndef __APPLE__

///
// GetContextRenderableType()
//
//    Check whether EGL_KHR_create_context extension is supported.  If so,
//    return EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT
//
EGLint GetContextRenderableType ( EGLDisplay eglDisplay )
{
#ifdef EGL_KHR_create_context
   const char *extensions = eglQueryString ( eglDisplay, EGL_EXTENSIONS );

   // check whether EGL_KHR_create_context is in the extension string
   if ( extensions != NULL && strstr( extensions, "EGL_KHR_create_context" ) )
   {
      // extension is supported
      return EGL_OPENGL_ES3_BIT_KHR;
   }
#endif
   // extension is not supported
   return EGL_OPENGL_ES2_BIT;
}
#endif

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  esCreateWindow()
//
//      title - name for title bar of window
//      width - width of window to create
//      height - height of window to create
//      flags  - bitwise or of window creation flags
//          ES_WINDOW_ALPHA       - specifies that the framebuffer should have alpha
//          ES_WINDOW_DEPTH       - specifies that a depth buffer should be created
//          ES_WINDOW_STENCIL     - specifies that a stencil buffer should be created
//          ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
//
//GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags )
//{
//#ifndef __APPLE__
//   EGLConfig config;
//   EGLint majorVersion;
//   EGLint minorVersion;
//   EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
//
//   if ( esContext == NULL )
//   {
//      return GL_FALSE;
//   }
//
//#ifdef ANDROID
//   // For Android, get the width/height from the window rather than what the
//   // application requested.
//   esContext->width = ANativeWindow_getWidth ( esContext->eglNativeWindow );
//   esContext->height = ANativeWindow_getHeight ( esContext->eglNativeWindow );
//#else
//   esContext->width = width;
//   esContext->height = height;
//#endif
//
//   if ( !WinCreate ( esContext, title ) )
//   {
//      return GL_FALSE;
//   }
//
//   esContext->eglDisplay = eglGetDisplay( esContext->eglNativeDisplay );
//   if ( esContext->eglDisplay == EGL_NO_DISPLAY )
//   {
//      return GL_FALSE;
//   }
//
//   // Initialize EGL
//   if ( !eglInitialize ( esContext->eglDisplay, &majorVersion, &minorVersion ) )
//   {
//      return GL_FALSE;
//   }
//
//   {
//      EGLint numConfigs = 0;
//      EGLint attribList[] =
//      {
//         EGL_RED_SIZE,       5,
//         EGL_GREEN_SIZE,     6,
//         EGL_BLUE_SIZE,      5,
//         EGL_ALPHA_SIZE,     ( flags & ES_WINDOW_ALPHA ) ? 8 : EGL_DONT_CARE,
//         EGL_DEPTH_SIZE,     ( flags & ES_WINDOW_DEPTH ) ? 8 : EGL_DONT_CARE,
//         EGL_STENCIL_SIZE,   ( flags & ES_WINDOW_STENCIL ) ? 8 : EGL_DONT_CARE,
//         EGL_SAMPLE_BUFFERS, ( flags & ES_WINDOW_MULTISAMPLE ) ? 1 : 0,
//         // if EGL_KHR_create_context extension is supported, then we will use
//         // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
//         EGL_RENDERABLE_TYPE, GetContextRenderableType ( esContext->eglDisplay ),
//         EGL_NONE
//      };
//
//      // Choose config
//      if ( !eglChooseConfig ( esContext->eglDisplay, attribList, &config, 1, &numConfigs ) )
//      {
//         return GL_FALSE;
//      }
//
//      if ( numConfigs < 1 )
//      {
//         return GL_FALSE;
//      }
//   }
//
//
//#ifdef ANDROID
//   // For Android, need to get the EGL_NATIVE_VISUAL_ID and set it using ANativeWindow_setBuffersGeometry
//   {
//      EGLint format = 0;
//      eglGetConfigAttrib ( esContext->eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format );
//      ANativeWindow_setBuffersGeometry ( esContext->eglNativeWindow, 0, 0, format );
//   }
//#endif // ANDROID
//
//   // Create a surface
//   esContext->eglSurface = eglCreateWindowSurface ( esContext->eglDisplay, config, 
//                                                    esContext->eglNativeWindow, NULL );
//
//   if ( esContext->eglSurface == EGL_NO_SURFACE )
//   {
//      return GL_FALSE;
//   }
//
//   // Create a GL context
//   esContext->eglContext = eglCreateContext ( esContext->eglDisplay, config, 
//                                              EGL_NO_CONTEXT, contextAttribs );
//
//   if ( esContext->eglContext == EGL_NO_CONTEXT )
//   {
//      return GL_FALSE;
//   }
//
//   // Make the context current
//   if ( !eglMakeCurrent ( esContext->eglDisplay, esContext->eglSurface, 
//                          esContext->eglSurface, esContext->eglContext ) )
//   {
//      return GL_FALSE;
//   }
//
//#endif // #ifndef __APPLE__
//
//   return GL_TRUE;
//}

///
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void ( ESCALLBACK *drawFunc ) ( ESContext * ) )
{
   esContext->drawFunc = drawFunc;
}

///
//  esRegisterShutdownFunc()
//
void ESUTIL_API esRegisterShutdownFunc ( ESContext *esContext, void ( ESCALLBACK *shutdownFunc ) ( ESContext * ) )
{
   esContext->shutdownFunc = shutdownFunc;
}

///
//  esRegisterUpdateFunc()
//
void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void ( ESCALLBACK *updateFunc ) ( ESContext *, float ) )
{
   esContext->updateFunc = updateFunc;
}


///
//  esRegisterKeyFunc()
//
void ESUTIL_API esRegisterKeyFunc ( ESContext *esContext,
                                    void ( ESCALLBACK *keyFunc ) ( ESContext *, unsigned char, int, int ) )
{
   esContext->keyFunc = keyFunc;
}


///
// esLogMessage()
//
//    Log an error message to the debug output for the platform
//
void ESUTIL_API esLogMessage ( const char *formatStr, ... )
{
   va_list params;
   char buf[BUFSIZ];

   va_start ( params, formatStr );
   vsprintf ( buf, formatStr, params );

#ifdef ANDROID
   __android_log_print ( ANDROID_LOG_INFO, "esUtil" , "%s", buf );
#else
   printf ( "%s", buf );
#endif

   va_end ( params );
}

///
// esFileRead()
//
//    Wrapper for platform specific File open
//
static esFile *esFileOpen ( void *ioContext, const char *fileName )
{
   esFile *pFile = NULL;

#ifdef ANDROID

   if ( ioContext != NULL )
   {
      AAssetManager *assetManager = ( AAssetManager * ) ioContext;
      pFile = AAssetManager_open ( assetManager, fileName, AASSET_MODE_BUFFER );
   }

#else
#ifdef __APPLE__
   // iOS: Remap the filename to a path that can be opened from the bundle.
   fileName = GetBundleFileName ( fileName );
#endif

   pFile = fopen ( fileName, "rb" );
#endif

   return pFile;
}

///
// esFileRead()
//
//    Wrapper for platform specific File close
//
static void esFileClose ( esFile *pFile )
{
   if ( pFile != NULL )
   {
#ifdef ANDROID
      AAsset_close ( pFile );
#else
      fclose ( pFile );
      pFile = NULL;
#endif
   }
}

///
// esFileRead()
//
//    Wrapper for platform specific File read
//
static int esFileRead ( esFile *pFile, int bytesToRead, void *buffer )
{
   int bytesRead = 0;

   if ( pFile == NULL )
   {
      return bytesRead;
   }

#ifdef ANDROID
   bytesRead = AAsset_read ( pFile, buffer, bytesToRead );
#else
   bytesRead = fread ( buffer, bytesToRead, 1, pFile );
#endif

   return bytesRead;
}

///
// esLoadTGA()
//
//    Loads a 8-bit, 24-bit or 32-bit TGA image from a file
//
char *ESUTIL_API esLoadTGA ( void *ioContext, const char *fileName, int *width, int *height )
{
   char        *buffer;
   esFile      *fp;
   TGA_HEADER   Header;
   int          bytesRead;

   // Open the file for reading
   fp = esFileOpen ( ioContext, fileName );

   if ( fp == NULL )
   {
      // Log error as 'error in opening the input file from apk'
      esLogMessage ( "esLoadTGA FAILED to load : { %s }\n", fileName );
      return NULL;
   }

   bytesRead = esFileRead ( fp, sizeof ( TGA_HEADER ), &Header );

   *width = Header.Width;
   *height = Header.Height;

   if ( Header.ColorDepth == 8 ||
         Header.ColorDepth == 24 || Header.ColorDepth == 32 )
   {
      int bytesToRead = sizeof ( char ) * ( *width ) * ( *height ) * Header.ColorDepth / 8;

      // Allocate the image data buffer
      buffer = ( char * ) malloc ( bytesToRead );

      if ( buffer )
      {
         bytesRead = esFileRead ( fp, bytesToRead, buffer );
         esFileClose ( fp );

         return ( buffer );
      }
   }

   return ( NULL );
}

// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ESShader.c
//
//    Utility functions for loading shaders and creating program objects.
//

///
//  Includes
//
#include "esUtil.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////
//
//  Private Functions
//
//



//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//
GLuint ESUTIL_API esLoadShader(GLenum type, const char* shaderSrc)
{
    GLuint shader;
    GLint compiled = 0;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
    {
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            esLogMessage("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}


//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
//         Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//
GLuint ESUTIL_API esLoadProgram(const char* vertShaderSrc, const char* fragShaderSrc)
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = esLoadShader(GL_VERTEX_SHADER, vertShaderSrc);

    if (vertexShader == 0)
    {
        return 0;
    }

    fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);

    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    // Create the program object
    programObject = glCreateProgram();

    if (programObject == 0)
    {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            esLogMessage("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(programObject);
        return 0;
    }

    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ESShapes.c
//
//    Utility functions for generating shapes
//

///
//  Includes
//
#include "esUtil.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

///
// Defines
//
#define ES_PI  (3.14159265f)

//////////////////////////////////////////////////////////////////
//
//  Private Functions
//
//



//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

//
/// \brief Generates geometry for a sphere.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list for a TRIANGLE_STRIP
/// \param numSlices The number of slices in the sphere
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLE_STRIP
//
int ESUTIL_API esGenSphere(int numSlices, float radius, GLfloat** vertices, GLfloat** normals,
    GLfloat** texCoords, GLuint** indices)
{
    int i;
    int j;
    int numParallels = numSlices / 2;
    int numVertices = (numParallels + 1) * (numSlices + 1);
    int numIndices = numParallels * numSlices * 6;
    float angleStep = (2.0f * ES_PI) / ((float)numSlices);

    // Allocate memory for buffers
    if (vertices != NULL)
    {
        *vertices = malloc(sizeof(GLfloat) * 3 * numVertices);
    }

    if (normals != NULL)
    {
        *normals = malloc(sizeof(GLfloat) * 3 * numVertices);
    }

    if (texCoords != NULL)
    {
        *texCoords = malloc(sizeof(GLfloat) * 2 * numVertices);
    }

    if (indices != NULL)
    {
        *indices = malloc(sizeof(GLuint) * numIndices);
    }

    for (i = 0; i < numParallels + 1; i++)
    {
        for (j = 0; j < numSlices + 1; j++)
        {
            int vertex = (i * (numSlices + 1) + j) * 3;

            if (vertices)
            {
                (*vertices)[vertex + 0] = radius * sinf(angleStep * (float)i) *
                    sinf(angleStep * (float)j);
                (*vertices)[vertex + 1] = radius * cosf(angleStep * (float)i);
                (*vertices)[vertex + 2] = radius * sinf(angleStep * (float)i) *
                    cosf(angleStep * (float)j);
            }

            if (normals)
            {
                (*normals)[vertex + 0] = (*vertices)[vertex + 0] / radius;
                (*normals)[vertex + 1] = (*vertices)[vertex + 1] / radius;
                (*normals)[vertex + 2] = (*vertices)[vertex + 2] / radius;
            }

            if (texCoords)
            {
                int texIndex = (i * (numSlices + 1) + j) * 2;
                (*texCoords)[texIndex + 0] = (float)j / (float)numSlices;
                (*texCoords)[texIndex + 1] = (1.0f - (float)i) / (float)(numParallels - 1);
            }
        }
    }

    // Generate the indices
    if (indices != NULL)
    {
        GLuint* indexBuf = (*indices);

        for (i = 0; i < numParallels; i++)
        {
            for (j = 0; j < numSlices; j++)
            {
                *indexBuf++ = i * (numSlices + 1) + j;
                *indexBuf++ = (i + 1) * (numSlices + 1) + j;
                *indexBuf++ = (i + 1) * (numSlices + 1) + (j + 1);

                *indexBuf++ = i * (numSlices + 1) + j;
                *indexBuf++ = (i + 1) * (numSlices + 1) + (j + 1);
                *indexBuf++ = i * (numSlices + 1) + (j + 1);
            }
        }
    }

    return numIndices;
}

//
/// \brief Generates geometry for a cube.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list for a TRIANGLES
/// \param scale The size of the cube, use 1.0 for a unit cube.
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLE_STRIP
//
int ESUTIL_API esGenCube(float scale, GLfloat** vertices, GLfloat** normals,
    GLfloat** texCoords, GLuint** indices)
{
    int i;
    int numVertices = 24;
    int numIndices = 36;

    GLfloat cubeVerts[] =
    {
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f,  0.5f,
       0.5f, -0.5f,  0.5f,
       0.5f, -0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f,  0.5f,  0.5f,
       0.5f,  0.5f,  0.5f,
       0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,
       0.5f,  0.5f, -0.5f,
       0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, 0.5f,
       -0.5f,  0.5f, 0.5f,
       0.5f,  0.5f, 0.5f,
       0.5f, -0.5f, 0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f,
       0.5f, -0.5f, -0.5f,
       0.5f, -0.5f,  0.5f,
       0.5f,  0.5f,  0.5f,
       0.5f,  0.5f, -0.5f,
    };

    GLfloat cubeNormals[] =
    {
       0.0f, -1.0f, 0.0f,
       0.0f, -1.0f, 0.0f,
       0.0f, -1.0f, 0.0f,
       0.0f, -1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, -1.0f,
       0.0f, 0.0f, -1.0f,
       0.0f, 0.0f, -1.0f,
       0.0f, 0.0f, -1.0f,
       0.0f, 0.0f, 1.0f,
       0.0f, 0.0f, 1.0f,
       0.0f, 0.0f, 1.0f,
       0.0f, 0.0f, 1.0f,
       -1.0f, 0.0f, 0.0f,
       -1.0f, 0.0f, 0.0f,
       -1.0f, 0.0f, 0.0f,
       -1.0f, 0.0f, 0.0f,
       1.0f, 0.0f, 0.0f,
       1.0f, 0.0f, 0.0f,
       1.0f, 0.0f, 0.0f,
       1.0f, 0.0f, 0.0f,
    };

    GLfloat cubeTex[] =
    {
       0.0f, 0.0f,
       0.0f, 1.0f,
       1.0f, 1.0f,
       1.0f, 0.0f,
       1.0f, 0.0f,
       1.0f, 1.0f,
       0.0f, 1.0f,
       0.0f, 0.0f,
       0.0f, 0.0f,
       0.0f, 1.0f,
       1.0f, 1.0f,
       1.0f, 0.0f,
       0.0f, 0.0f,
       0.0f, 1.0f,
       1.0f, 1.0f,
       1.0f, 0.0f,
       0.0f, 0.0f,
       0.0f, 1.0f,
       1.0f, 1.0f,
       1.0f, 0.0f,
       0.0f, 0.0f,
       0.0f, 1.0f,
       1.0f, 1.0f,
       1.0f, 0.0f,
    };

    // Allocate memory for buffers
    if (vertices != NULL)
    {
        *vertices = malloc(sizeof(GLfloat) * 3 * numVertices);
        memcpy(*vertices, cubeVerts, sizeof(cubeVerts));

        for (i = 0; i < numVertices * 3; i++)
        {
            (*vertices)[i] *= scale;
        }
    }

    if (normals != NULL)
    {
        *normals = malloc(sizeof(GLfloat) * 3 * numVertices);
        memcpy(*normals, cubeNormals, sizeof(cubeNormals));
    }

    if (texCoords != NULL)
    {
        *texCoords = malloc(sizeof(GLfloat) * 2 * numVertices);
        memcpy(*texCoords, cubeTex, sizeof(cubeTex));
    }


    // Generate the indices
    if (indices != NULL)
    {
        GLuint cubeIndices[] =
        {
           0, 2, 1,
           0, 3, 2,
           4, 5, 6,
           4, 6, 7,
           8, 9, 10,
           8, 10, 11,
           12, 15, 14,
           12, 14, 13,
           16, 17, 18,
           16, 18, 19,
           20, 23, 22,
           20, 22, 21
        };

        *indices = malloc(sizeof(GLuint) * numIndices);
        memcpy(*indices, cubeIndices, sizeof(cubeIndices));
    }

    return numIndices;
}

//
/// \brief Generates a square grid consisting of triangles.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list as TRIANGLES.
/// \param size create a grid of size by size (number of triangles = (size-1)*(size-1)*2)
/// \param vertices If not NULL, will contain array of float3 positions
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLES
//
int ESUTIL_API esGenSquareGrid(int size, GLfloat** vertices, GLuint** indices)
{
    int i, j;
    int numIndices = (size - 1) * (size - 1) * 2 * 3;

    // Allocate memory for buffers
    if (vertices != NULL)
    {
        int numVertices = size * size;
        float stepSize = (float)size - 1;
        *vertices = malloc(sizeof(GLfloat) * 3 * numVertices);

        for (i = 0; i < size; ++i) // row
        {
            for (j = 0; j < size; ++j) // column
            {
                (*vertices)[3 * (j + i * size)] = i / stepSize;
                (*vertices)[3 * (j + i * size) + 1] = j / stepSize;
                (*vertices)[3 * (j + i * size) + 2] = 0.0f;
            }
        }
    }

    // Generate the indices
    if (indices != NULL)
    {
        *indices = malloc(sizeof(GLuint) * numIndices);

        for (i = 0; i < size - 1; ++i)
        {
            for (j = 0; j < size - 1; ++j)
            {
                // two triangles per quad
                (*indices)[6 * (j + i * (size - 1))] = j + (i) * (size);
                (*indices)[6 * (j + i * (size - 1)) + 1] = j + (i) * (size)+1;
                (*indices)[6 * (j + i * (size - 1)) + 2] = j + (i + 1) * (size)+1;

                (*indices)[6 * (j + i * (size - 1)) + 3] = j + (i) * (size);
                (*indices)[6 * (j + i * (size - 1)) + 4] = j + (i + 1) * (size)+1;
                (*indices)[6 * (j + i * (size - 1)) + 5] = j + (i + 1) * (size);
            }
        }
    }

    return numIndices;
}


// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 3.0 Programming Guide.
//

///
//  Includes
//
#include "esUtil.h"
#include <math.h>
#include <string.h>

#define PI 3.1415926535897932384626433832795f

void ESUTIL_API
esScale(ESMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

void ESUTIL_API
esTranslate(ESMatrix* result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void ESUTIL_API
esRotate(ESMatrix* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat sinAngle, cosAngle;
    GLfloat mag = sqrtf(x * x + y * y + z * z);

    sinAngle = sinf(angle * PI / 180.0f);
    cosAngle = cosf(angle * PI / 180.0f);

    if (mag > 0.0f)
    {
        GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
        GLfloat oneMinusCos;
        ESMatrix rotMat;

        x /= mag;
        y /= mag;
        z /= mag;

        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * sinAngle;
        ys = y * sinAngle;
        zs = z * sinAngle;
        oneMinusCos = 1.0f - cosAngle;

        rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMat.m[0][1] = (oneMinusCos * xy) - zs;
        rotMat.m[0][2] = (oneMinusCos * zx) + ys;
        rotMat.m[0][3] = 0.0F;

        rotMat.m[1][0] = (oneMinusCos * xy) + zs;
        rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMat.m[1][2] = (oneMinusCos * yz) - xs;
        rotMat.m[1][3] = 0.0F;

        rotMat.m[2][0] = (oneMinusCos * zx) - ys;
        rotMat.m[2][1] = (oneMinusCos * yz) + xs;
        rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMat.m[2][3] = 0.0F;

        rotMat.m[3][0] = 0.0F;
        rotMat.m[3][1] = 0.0F;
        rotMat.m[3][2] = 0.0F;
        rotMat.m[3][3] = 1.0F;

        esMatrixMultiply(result, &rotMat, result);
    }
}

void ESUTIL_API
esFrustum(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    frust;

    if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
        (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
    {
        return;
    }

    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    esMatrixMultiply(result, &frust, result);
}


void ESUTIL_API
esPerspective(ESMatrix* result, float fovy, float aspect, float nearZ, float farZ)
{
    GLfloat frustumW, frustumH;

    frustumH = tanf(fovy / 360.0f * PI) * nearZ;
    frustumW = frustumH * aspect;

    esFrustum(result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

void ESUTIL_API
esOrtho(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    ortho;

    if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
    {
        return;
    }

    esMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

    esMatrixMultiply(result, &ortho, result);
}


void ESUTIL_API
esMatrixMultiply(ESMatrix* result, ESMatrix* srcA, ESMatrix* srcB)
{
    ESMatrix    tmp;
    int         i;

    for (i = 0; i < 4; i++)
    {
        tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) +
            (srcA->m[i][1] * srcB->m[1][0]) +
            (srcA->m[i][2] * srcB->m[2][0]) +
            (srcA->m[i][3] * srcB->m[3][0]);

        tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) +
            (srcA->m[i][1] * srcB->m[1][1]) +
            (srcA->m[i][2] * srcB->m[2][1]) +
            (srcA->m[i][3] * srcB->m[3][1]);

        tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) +
            (srcA->m[i][1] * srcB->m[1][2]) +
            (srcA->m[i][2] * srcB->m[2][2]) +
            (srcA->m[i][3] * srcB->m[3][2]);

        tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) +
            (srcA->m[i][1] * srcB->m[1][3]) +
            (srcA->m[i][2] * srcB->m[2][3]) +
            (srcA->m[i][3] * srcB->m[3][3]);
    }

    memcpy(result, &tmp, sizeof(ESMatrix));
}


void ESUTIL_API
esMatrixLoadIdentity(ESMatrix* result)
{
    memset(result, 0x0, sizeof(ESMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void ESUTIL_API
esMatrixLookAt(ESMatrix* result,
    float posX, float posY, float posZ,
    float lookAtX, float lookAtY, float lookAtZ,
    float upX, float upY, float upZ)
{
    float axisX[3], axisY[3], axisZ[3];
    float length;

    // axisZ = lookAt - pos
    axisZ[0] = lookAtX - posX;
    axisZ[1] = lookAtY - posY;
    axisZ[2] = lookAtZ - posZ;

    // normalize axisZ
    length = sqrtf(axisZ[0] * axisZ[0] + axisZ[1] * axisZ[1] + axisZ[2] * axisZ[2]);

    if (length != 0.0f)
    {
        axisZ[0] /= length;
        axisZ[1] /= length;
        axisZ[2] /= length;
    }

    // axisX = up X axisZ
    axisX[0] = upY * axisZ[2] - upZ * axisZ[1];
    axisX[1] = upZ * axisZ[0] - upX * axisZ[2];
    axisX[2] = upX * axisZ[1] - upY * axisZ[0];

    // normalize axisX
    length = sqrtf(axisX[0] * axisX[0] + axisX[1] * axisX[1] + axisX[2] * axisX[2]);

    if (length != 0.0f)
    {
        axisX[0] /= length;
        axisX[1] /= length;
        axisX[2] /= length;
    }

    // axisY = axisZ x axisX
    axisY[0] = axisZ[1] * axisX[2] - axisZ[2] * axisX[1];
    axisY[1] = axisZ[2] * axisX[0] - axisZ[0] * axisX[2];
    axisY[2] = axisZ[0] * axisX[1] - axisZ[1] * axisX[0];

    // normalize axisY
    length = sqrtf(axisY[0] * axisY[0] + axisY[1] * axisY[1] + axisY[2] * axisY[2]);

    if (length != 0.0f)
    {
        axisY[0] /= length;
        axisY[1] /= length;
        axisY[2] /= length;
    }

    memset(result, 0x0, sizeof(ESMatrix));

    result->m[0][0] = -axisX[0];
    result->m[0][1] = axisY[0];
    result->m[0][2] = -axisZ[0];

    result->m[1][0] = -axisX[1];
    result->m[1][1] = axisY[1];
    result->m[1][2] = -axisZ[1];

    result->m[2][0] = -axisX[2];
    result->m[2][1] = axisY[2];
    result->m[2][2] = -axisZ[2];

    // translate (-posX, -posY, -posZ)
    result->m[3][0] = axisX[0] * posX + axisX[1] * posY + axisX[2] * posZ;
    result->m[3][1] = -axisY[0] * posX - axisY[1] * posY - axisY[2] * posZ;
    result->m[3][2] = axisZ[0] * posX + axisZ[1] * posY + axisZ[2] * posZ;
    result->m[3][3] = 1.0f;
}

