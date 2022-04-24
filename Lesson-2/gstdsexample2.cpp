/**
 * @file gstdsexample2.cpp
 * @author M JAYANTH VARMA (jayanthvaram134@gmail.com)
 * @brief
 * @version 1.0
 * @date 2022-04-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include "gstdsexample2.h"

GST_DEBUG_CATEGORY_STATIC (gst_dsexample2_debug);
#define GST_CAT_DEFAULT gst_dsexample2_debug
#define USE_EGLIMAGE 1
/* enable to write transformed cvmat to files */
/* #define DSEXAMPLE2_DEBUG */
static GQuark _dsmeta_quark = 0;

/* Enum to identify properties */
enum
{
  PROP_0,
  PROP_UNIQUE_ID,
  PROP_GPU_DEVICE_ID
};

/* Default values for properties */
#define DEFAULT_UNIQUE_ID 15
#define DEFAULT_GPU_ID 0

/* By default NVIDIA Hardware allocated memory flows through the pipeline. We
 * will be processing on this type of memory only. */
#define GST_CAPS_FEATURE_MEMORY_NVMM "memory:NVMM"
static GstStaticPadTemplate gst_dsexample2_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE_WITH_FEATURES
        (GST_CAPS_FEATURE_MEMORY_NVMM,
            "{ NV12, RGBA, I420 }")));

static GstStaticPadTemplate gst_dsexample2_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE_WITH_FEATURES
        (GST_CAPS_FEATURE_MEMORY_NVMM,
            "{ NV12, RGBA, I420 }")));

/* Define our element type. Standard GObject/GStreamer boilerplate stuff */
#define gst_dsexample2_parent_class parent_class
G_DEFINE_TYPE (GstDsExample2, gst_dsexample2, GST_TYPE_BASE_TRANSFORM);

static void gst_dsexample2_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_dsexample2_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_dsexample2_set_caps (GstBaseTransform * btrans,
    GstCaps * incaps, GstCaps * outcaps);
static gboolean gst_dsexample2_start (GstBaseTransform * btrans);
static gboolean gst_dsexample2_stop (GstBaseTransform * btrans);

static GstFlowReturn gst_dsexample2_transform_ip (GstBaseTransform *
    btrans, GstBuffer * inbuf);

/* Install properties, set sink and src pad capabilities, override the required
 * functions of the base class, These are common to all instances of the
 * element.
 */
static void
gst_dsexample2_class_init (GstDsExample2Class * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseTransformClass *gstbasetransform_class;

  /* Indicates we want to use DS buf api */
  g_setenv ("DS_NEW_BUFAPI", "1", TRUE);

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasetransform_class = (GstBaseTransformClass *) klass;

  /* Overide base class functions */
  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_dsexample2_set_property);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_dsexample2_get_property);

  gstbasetransform_class->set_caps = GST_DEBUG_FUNCPTR (gst_dsexample2_set_caps);
  gstbasetransform_class->start = GST_DEBUG_FUNCPTR (gst_dsexample2_start);
  gstbasetransform_class->stop = GST_DEBUG_FUNCPTR (gst_dsexample2_stop);

  gstbasetransform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_dsexample2_transform_ip);

  /* Install properties */
  g_object_class_install_property (gobject_class, PROP_UNIQUE_ID,
      g_param_spec_uint ("unique-id",
          "Unique ID",
          "Unique ID for the element. Can be used to identify output of the"
          " element", 0, G_MAXUINT, DEFAULT_UNIQUE_ID, (GParamFlags)
          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_GPU_DEVICE_ID,
      g_param_spec_uint ("gpu-id",
          "Set GPU Device ID",
          "Set GPU Device ID", 0,
          G_MAXUINT, 0,
          GParamFlags
          (G_PARAM_READWRITE |
              G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_READY)));
  /* Set sink and src pad capabilities */
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_dsexample2_src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_dsexample2_sink_template));

  /* Set metadata describing the element */
  gst_element_class_set_details_simple (gstelement_class,
      "DsExample2 plugin",
      "DsExample2 Plugin",
      "Plugin to print metadata from upstream 'nvinfer' element",
      "A stupid plugin that just prints metadata from upstream element"
      "M JAYANTH VARMA | jayanthvarma134@gmail.com");
}

static void
gst_dsexample2_init (GstDsExample2 * dsexample2)
{
  GstBaseTransform *btrans = GST_BASE_TRANSFORM (dsexample2);
  gst_base_transform_set_in_place (GST_BASE_TRANSFORM (btrans), TRUE);
  gst_base_transform_set_passthrough (GST_BASE_TRANSFORM (btrans), TRUE);

  dsexample2->unique_id = DEFAULT_UNIQUE_ID;
  dsexample2->gpu_id = DEFAULT_GPU_ID;

  if (!_dsmeta_quark)
    _dsmeta_quark = g_quark_from_static_string (NVDS_META_STRING);
}

/* Function called when a property of the element is set. Standard boilerplate.
 */
static void
gst_dsexample2_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstDsExample2 *dsexample2 = GST_DSEXAMPLE2 (object);
  switch (prop_id) {
    case PROP_UNIQUE_ID:
      dsexample2->unique_id = g_value_get_uint (value);
      break;
    case PROP_GPU_DEVICE_ID:
      dsexample2->gpu_id = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Function called when a property of the element is requested. Standard
 * boilerplate.
 */
static void
gst_dsexample2_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstDsExample2 *dsexample2 = GST_DSEXAMPLE2 (object);

  switch (prop_id) {
    case PROP_UNIQUE_ID:
      g_value_set_uint (value, dsexample2->unique_id);
      break;
    case PROP_GPU_DEVICE_ID:
      g_value_set_uint (value, dsexample2->gpu_id);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/**
 * Initialize all resources and start the output thread
 */
static gboolean
gst_dsexample2_start (GstBaseTransform * btrans)
{
  return TRUE;
}

/**
 * Stop the output thread and free up all the resources
 */
static gboolean
gst_dsexample2_stop (GstBaseTransform * btrans)
{
  return TRUE;
}

/**
 * Called when source / sink pad capabilities have been negotiated.
 */
static gboolean
gst_dsexample2_set_caps (GstBaseTransform * btrans, GstCaps * incaps,
    GstCaps * outcaps)
{
  GstDsExample2 *dsexample2 = GST_DSEXAMPLE2 (btrans);
  /* Save the input video information, since this will be required later. */
  gst_video_info_from_caps (&dsexample2->video_info, incaps);

  /* requires RGBA format for blurring the objects in opencv */
    if (dsexample2->video_info.finfo->format != GST_VIDEO_FORMAT_RGBA) {
    GST_ELEMENT_ERROR (dsexample2, STREAM, FAILED,
        ("input format should be RGBA when using blur-objects property"), (NULL));
    return FALSE;
    }

  return TRUE;

}

/**
 * Called when element recieves an input buffer from upstream element.
 */
static GstFlowReturn
gst_dsexample2_transform_ip (GstBaseTransform * btrans, GstBuffer * inbuf)
{
  GstDsExample2 *dsexample2 = GST_DSEXAMPLE2 (btrans);
  GstFlowReturn flow_ret = GST_FLOW_ERROR;

  NvDsBatchMeta *batch_meta = NULL;
  NvDsFrameMeta *frame_meta = NULL;
  NvDsMetaList * l_frame = NULL;
  guint i = 0;

  batch_meta = gst_buffer_get_nvds_batch_meta (inbuf);
  if (batch_meta == nullptr) {
    GST_ELEMENT_ERROR (dsexample2, STREAM, FAILED,
        ("NvDsBatchMeta not found for input buffer."), (NULL));
    return GST_FLOW_ERROR;
  }

  /* Using object crops as input to the algorithm. The objects are detected by
     * the primary detector */
    NvDsMetaList * l_obj = NULL;
    NvDsObjectMeta *obj_meta = NULL;

    for (l_frame = batch_meta->frame_meta_list; l_frame != NULL;
      l_frame = l_frame->next)
    {
      frame_meta = (NvDsFrameMeta *) (l_frame->data);
      int object_count = 0;

      std::cout<< "******  ******"<<std::endl;
      std::cout<< "[frame_num : " << frame_meta->frame_num<<" ]"<<std::endl;

      for (l_obj = frame_meta->obj_meta_list; l_obj != NULL;
          l_obj = l_obj->next)
      {
        obj_meta = (NvDsObjectMeta *) (l_obj->data);
        object_count+= 1;
      }
      std::cout<< "[objs_in_frame : "<<object_count <<" ]"<<std::endl;

    }
  flow_ret = GST_FLOW_OK;
  return flow_ret;

}


/**
 * Boiler plate for registering a plugin and an element.
 */
static gboolean
dsexample2_plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_dsexample2_debug, "dsexample2", 0,
      "dsexample2 plugin");

  return gst_element_register (plugin, "dsexample2", GST_RANK_PRIMARY,
      GST_TYPE_DSEXAMPLE2);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    nvdsgst_dsexample2,
    DESCRIPTION, dsexample2_plugin_init, "1.0", LICENSE, BINARY_PACKAGE, URL)
