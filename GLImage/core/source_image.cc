/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source_image.h"
#include "core/core_gpupixel_context.h"
#include "core/core_util.h"

#if defined(GPUPIXEL_ANDROID)
#include <android/bitmap.h>
#include "jni_helpers.h"
#endif

#define STB_IMAGE_IMPLEMENTATION

#include "core/stb_image.h"


#include <libyuv.h>

USING_NS_GPUPIXEL

std::shared_ptr<SourceImage> SourceImage::create_from_memory(int width,
                                                 int height,
                                                 int channel_count,
                                                 const unsigned char *pixels) {
    auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
    sourceImage->initBGR(width, height, channel_count, pixels);
    return sourceImage;
}

std::shared_ptr<SourceImage> SourceImage::create(const std::string name) {
    int width, height, channel_count;
    unsigned char *data = stbi_load(name.c_str(), &width, &height, &channel_count, 0);
//   todo(logo info)
    if(data == nullptr) {
        Util::Log("SourceImage", "SourceImage: input data in null! file name: %s", name.c_str());
        return nullptr;
    }
    auto image = SourceImage::create_from_memory(width, height, channel_count, data);
    stbi_image_free(data);
    return image;
}

void SourceImage::initBGR(int _width, int _height, int channel_count, const unsigned char* pixels) {
    this->setFramebuffer(0);

    if (!_framebuffer || (_framebuffer->getWidth() != _width ||
        _framebuffer->getHeight() != _height)) {
        _framebuffer =
            GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
                _width, _height, true);
        if(m_data)
            delete m_data;
        int size = _width * _height * 4;
        m_data = new unsigned char[size];
    }
    this->setFramebuffer(_framebuffer);
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, this->getFramebuffer()->getTexture()));
  if(channel_count == 3) {
    libyuv::RGB24ToARGB(pixels, _width * channel_count, m_data, _width * 4, _width, _height);
  } else if(channel_count == 4) {
    libyuv::ARGBCopy(pixels, _width * channel_count, m_data, _width * 4, _width, _height);
  }

  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, m_data));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void SourceImage::Render() {
  Source::proceed();
}

