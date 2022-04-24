#pragma once

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/time.h>

#include <gst/gst.h>
#include <glib.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include "nvbufsurface.h"
#include "nvbufsurftransform.h"
#include "gst-nvquery.h"
#include "gstnvdsmeta.h"

// OpenCV for image operations
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

const int roi_max_length = 20;
struct cur_pos {
  float x, y;
};

struct polygon {
  cur_pos points[roi_max_length];
};

typedef struct {
  int coord_pos;
  polygon coords;
  int num_coords;
  bool is_direction;
  std::tuple<int, int> direction;
  float coverage;
  float iou;
  int roi_type;
} ROIConf;

typedef struct {
  int obj_class;
  int obj_det;
  int x, y, w, h;
  int obj_confidence;
  float color_red;
  float color_green;
  float color_blue;
} OpenCVOSDParams;

enum PGIE_CLASS {CAR = 0, BICYCLE = 1, PERSON = 2, ROADSIGN = 3};
enum GIE_UID {LINEFIRE_DETECTOR = 1};
enum BBOX_TYPE {RECTANGLE = 0, CIRCLE = 1};

inline int MAX_DISPLAY_LEN = 128;
