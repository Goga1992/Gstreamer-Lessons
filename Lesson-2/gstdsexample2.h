/**
 * @file gstdsexample2.h
 * @author M JAYANTH VARMA (jayanthvarma134@gmail.com)
 * @brief
 * @version 1.0
 * @date 2022-04-24
 *
 * @copyright Copyright (c) 2022
 *
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

/* Package and library details required for plugin_init */
#define PACKAGE "dsexample2"
#define VERSION "1.0"
#define LICENSE "Proprietary"
#define DESCRIPTION "A stupid plugin that just prints metadata from upstream 'nvinfer' element."
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

  guint unique_id;
  GstVideoInfo video_info;
  guint gpu_id;
};

// Boiler plate stuff
struct _GstDsExample2Class
{
  GstBaseTransformClass parent_class;
};

GType gst_dsexample2_get_type (void);

G_END_DECLS
#endif /* __GST_DSEXAMPLE2_H__ */
