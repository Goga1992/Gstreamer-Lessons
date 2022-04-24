/**
 * Copyright (c) 2017-2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __GST_DSEXAMPLE2_H__
#define __GST_DSEXAMPLE2_H__

#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h>

/* Open CV headers */
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cuda.h>
#include <cuda_runtime.h>
#include "nvbufsurface.h"
#include "nvbufsurftransform.h"
#include "gst-nvquery.h"
#include "gstnvdsmeta.h"
#include "dsexample2_lib/dsexample2_lib.h"

/* Package and library details required for plugin_init */
#define PACKAGE "dsexample2"
#define VERSION "1.0"
#define LICENSE "Proprietary"
#define DESCRIPTION "A useless plugin to print metadata from upstream nvinfer element."
#define BINARY_PACKAGE "Indie plugin"
#define URL "https://github.com/jayanthvarma134"


G_BEGIN_DECLS
/* Standard boilerplate stuff */
typedef struct _GstDsExample2 GstDsExample2;
typedef struct _GstDsExample2Class GstDsExample2Class;

/* Standard boilerplate stuff */
#define GST_TYPE_DSEXAMPLE2 (gst_dsexample2_get_type())
#define GST_DSEXAMPLE2(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DSEXAMPLE2,GstDsExample2))
#define GST_DSEXAMPLE2_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DSEXAMPLE2,GstDsExample2Class))
#define GST_DSEXAMPLE2_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_DSEXAMPLE2, GstDsExample2Class))
#define GST_IS_DSEXAMPLE2(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DSEXAMPLE2))
#define GST_IS_DSEXAMPLE2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DSEXAMPLE2))
#define GST_DSEXAMPLE2_CAST(obj)  ((GstDsExample2 *)(obj))

struct _GstDsExample2
{
  GstBaseTransform base_trans;

  // Context of the custom algorithm library
  DsExample2Ctx *dsexample2lib_ctx;

  guint unique_id;
  guint64 frame_num;
  cudaStream_t cuda_stream;
  NvBufSurface *inter_buf;
  GstVideoInfo video_info;
  guint gpu_id;
  gboolean process_full_frame;
};

// Boiler plate stuff
struct _GstDsExample2Class
{
  GstBaseTransformClass parent_class;
};

GType gst_dsexample2_get_type (void);

G_END_DECLS
#endif /* __GST_DSEXAMPLE2_H__ */
