
/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "core_gpupixel_macros.h"

// base
#include "core_framebuffer.h"
#include "core_framebuffer_cache.h"
#include "core_gl_program.h"
#include "core_gpupixel_context.h"

// utils
#include "core_math_toolbox.h"
#include "core_util.h"

// source
#include "core/source.h"
#include "core/source_camera.h"
#include "core/source_image.h"
#include "core/source_raw_data_input.h"

// target
#include "core/target.h"
#include "core/target_raw_data_output.h"
#include "core/target_view.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#include "core/gpupixel_target.h"
#include "gpupixel_view.h"
#include "objc_target.h"
#endif

// base filters
#include "core/filter.h"
#include "core/filter_group.h"

// face filters
#include "core/beauty_face_filter.h"

#include "core/box_blur_filter.h"
#include "core/box_high_pass_filter.h"

// general filters
#include "core/bilateral_filter.h"
#include "core/brightness_filter.h"
#include "core/canny_edge_detection_filter.h"
#include "core/color_invert_filter.h"
#include "core/color_matrix_filter.h"
#include "core/contrast_filter.h"
#include "core/convolution3x3_filter.h"
#include "core/crosshatch_filter.h"
#include "core/directional_non_maximum_suppression_filter.h"
#include "core/directional_sobel_edge_detection_filter.h"
#include "core/emboss_filter.h"
#include "core/exposure_filter.h"
#include "core/gaussian_blur_filter.h"
#include "core/gaussian_blur_mono_filter.h"
#include "core/glass_sphere_filter.h"
#include "core/grayscale_filter.h"
#include "core/hsb_filter.h"
#include "core/hue_filter.h"
#include "core/ios_blur_filter.h"
#include "core/luminance_range_filter.h"
#include "core/nearby_sampling3x3_filter.h"
#include "core/non_maximum_suppression_filter.h"
#include "core/pixellation_filter.h"
#include "core/posterize_filter.h"
#include "core/rgb_filter.h"
#include "core/saturation_filter.h"
#include "core/single_component_gaussian_blur_filter.h"
#include "core/single_component_gaussian_blur_mono_filter.h"
#include "core/sketch_filter.h"
#include "core/smooth_toon_filter.h"
#include "core/sobel_edge_detection_filter.h"
#include "core/sphere_refraction_filter.h"
#include "core/toon_filter.h"
#include "core/weak_pixel_inclusion_filter.h"
