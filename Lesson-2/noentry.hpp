/**
 * @file custom.hpp
 * @author M JAYANTH VARMA (jayanthvarma134@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include <cmath>

#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h>

/* Open CV headers */
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/cudafilters.hpp"

#include <cuda.h>
#include <cuda_runtime.h>
#include "nvbufsurface.h"
#include "nvbufsurftransform.h"
#include "gst-nvquery.h"
#include "gstnvdsmeta.h"
#include "custom_lib/noentry_lib.hpp"

/* Package and library details required for plugin_init */
#define PACKAGE "custom"
#define VERSION "1.0"
#define LICENSE "Proprietary"
#define DESCRIPTION "A custom beginner level Gstreamer plugin to check if a person enters a NO-ENTRY zone."
#define BINARY_PACKAGE "A DeepStream plugin for workplace safety (To check if a person enters a No-Entry zone)."
#define URL "https://github.com/jayanthvarma134/Gstreamer-Lessons"
#define AUTHOR "jayanthvarma134 - M. JAYANTH VARMA"

G_BEGIN_DECLS
/* Standard boilerplate stuff */
typedef struct _NoEntry NoEntry;
typedef struct _NoEntryClass NoEntryClass;

/* Standard boilerplate stuff */
#define GST_TYPE_NOENTRY (gst_noexample_get_type())
#define GST_NOENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_NOENTRY,NoEntry))
#define GST_NOENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_NOENTRY,NoEntryClass))
#define GST_NOENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_NOENTRY, NoEntryClass))
#define GST_IS_NOENTRY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_NOENTRY))
#define GST_IS_NOENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_NOENTRY))
#define GST_NOENTRY_CAST(obj)  ((NoEntry *)(obj))

enum HARDWARE_TYPE {CPU = 0, GPU = 1};

struct _NoEntry
{
  GstBaseTransform base_trans;
  guint unique_id;
  guint64 frame_num;
  cv::Mat *cvmat;
  gint processing_width;
  gint processing_height;
  guint batch_size;
  guint gpu_id;
};

struct _NoEntryClass
{
  GstBaseTransformClass parent_class;
};

GType gst_noentry_get_type (void);

G_END_DECLS
#endif /* __GST_NOENTRY_H__ */
