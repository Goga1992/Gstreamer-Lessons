
#include "lineoffire.hpp"
using namespace LineFire;

GST_DEBUG_CATEGORY_STATIC (gst_lineoffire_debug);
#define GST_CAT_DEFAULT gst_lineoffire_debug
#define USE_EGLIMAGE 1
/* #define LINEOFFORE_DEBUG */
static GQuark _dsmeta_quark = 0;

enum LOF_PROPS
{
  PROP_0,
  PROP_UNIQUE_ID,
  PROP_FRAMES_THRESHOLD,
  PROP_DELETE_THRESHOOLD,
  PROP_ALARM_COOLDOWN_PERIOD,
};


/* Default values for properties */
#define DEFAULT_UNIQUE_ID 15
// algo
#define DEFAULT_FRAMES_THRESHOLD 50
#define DEFAULT_DELETE_THRESHOLD 1000
#define DEFAUT_ALARM_COOLDOWN_PERIOD 15000

/* By default NVIDIA Hardware allocated memory flows through the pipeline. We
 * will be processing on this type of memory only. */
#define GST_CAPS_FEATURE_MEMORY_NVMM "memory:NVMM"
static GstStaticPadTemplate gst_lineoffire_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE_WITH_FEATURES
        (GST_CAPS_FEATURE_MEMORY_NVMM,
            "{ NV12, RGBA, I420 }")));

static GstStaticPadTemplate gst_lineoffire_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE_WITH_FEATURES
        (GST_CAPS_FEATURE_MEMORY_NVMM,
            "{ NV12, RGBA, I420 }")));

/* Define our element type. Standard GObject/GStreamer boilerplate stuff */
#define gst_lineoffire_parent_class parent_class
G_DEFINE_TYPE (GstLineOfFire, gst_lineoffire, GST_TYPE_BASE_TRANSFORM);

static void gst_lineoffire_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void gst_lineoffire_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_lineoffire_set_caps (GstBaseTransform * btrans,
    GstCaps * incaps, GstCaps * outcaps);

static gboolean gst_lineoffire_start (GstBaseTransform * btrans);

static gboolean gst_lineoffire_stop (GstBaseTransform * btrans);

static GstFlowReturn gst_lineoffire_transform_ip (GstBaseTransform *
    btrans, GstBuffer * inbuf);

/* Install properties, set sink and src pad capabilities, override the required
 * functions of the base class, These are common to all instances of the
 * element.
 */
static void
gst_lineoffire_class_init (GstLineOfFireClass * klass)
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
  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_lineoffire_set_property);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_lineoffire_get_property);

  gstbasetransform_class->set_caps = GST_DEBUG_FUNCPTR (gst_lineoffire_set_caps);
  gstbasetransform_class->start = GST_DEBUG_FUNCPTR (gst_lineoffire_start);
  gstbasetransform_class->stop = GST_DEBUG_FUNCPTR (gst_lineoffire_stop);

  gstbasetransform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_lineoffire_transform_ip);

  /* Install properties */
  g_object_class_install_property (gobject_class, PROP_UNIQUE_ID,
      g_param_spec_uint ("unique-id",
          "Unique ID",
          "Unique ID for the element. Can be used to identify output of the"
          " element", 0, G_MAXUINT, DEFAULT_UNIQUE_ID, (GParamFlags)
          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_FRAMES_THRESHOLD,
      g_param_spec_int ("frames-threshold",
          "Frames Threshold",
          "Number of frames to evaluate for person in Line of Fire",
          1, G_MAXINT, DEFAULT_FRAMES_THRESHOLD, (GParamFlags)
          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_DELETE_THRESHOOLD,
      g_param_spec_int ("delete-threshold",
          "Delete Threshold",
          "Time (ms) to wait before deleting a lagging UserMeta(person) object.",
          1, G_MAXINT, DEFAULT_DELETE_THRESHOLD, (GParamFlags)
          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_ALARM_COOLDOWN_PERIOD,
      g_param_spec_int ("alarm-cooldown-period",
          "Alarm CoolDown Period",
          "Time (ms) to wait after raising an LOF alarm.",
          1, G_MAXINT, DEFAUT_ALARM_COOLDOWN_PERIOD, (GParamFlags)
          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));


  /* Set sink and src pad capabilities */
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_lineoffire_src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_lineoffire_sink_template));

  /* Set metadata describing the element */
  gst_element_class_set_details_simple (gstelement_class,
      "LineOfFire plugin",
      "LineOfFire Plugin",
      "Process a 3rdparty example algorithm on objects / full frame",
      "NVIDIA Corporation. Post on Deepstream for Tesla forum for any queries "
      "@ https://devtalk.nvidia.com/default/board/209/");
}

static void
gst_lineoffire_init (GstLineOfFire * lineoffire)
{
  GstBaseTransform *btrans = GST_BASE_TRANSFORM (lineoffire);

  gst_base_transform_set_in_place (GST_BASE_TRANSFORM (btrans), TRUE);
  gst_base_transform_set_passthrough (GST_BASE_TRANSFORM (btrans), TRUE);

  /* Initialize all property variables to default values */
  lineoffire->unique_id = DEFAULT_UNIQUE_ID;
  lineoffire->frames_threshold = DEFAULT_FRAMES_THRESHOLD;
  lineoffire->delete_threshold = DEFAULT_DELETE_THRESHOLD;
  lineoffire->alarm_cooldown_period = DEFAUT_ALARM_COOLDOWN_PERIOD;

  /* This quark is required to identify NvDsMeta when iterating through
   * the buffer metadatas */
  if (!_dsmeta_quark)
    _dsmeta_quark = g_quark_from_static_string (NVDS_META_STRING);
}

/* Function called when a property of the element is set. Standard boilerplate.
 */
static void
gst_lineoffire_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstLineOfFire *lineoffire = GST_LINEOFFIRE (object);
  switch (prop_id) {
    case PROP_UNIQUE_ID:
      lineoffire->unique_id = g_value_get_uint (value);
      break;
    case PROP_FRAMES_THRESHOLD:
      lineoffire->frames_threshold = g_value_get_uint (value);
      break;
    case PROP_DELETE_THRESHOOLD:
      lineoffire->delete_threshold = g_value_get_uint (value);
      break;
    case PROP_ALARM_COOLDOWN_PERIOD:
      lineoffire->alarm_cooldown_period = g_value_get_uint (value);
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
gst_lineoffire_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstLineOfFire *lineoffire = GST_LINEOFFIRE (object);

  switch (prop_id) {
    case PROP_UNIQUE_ID:
      g_value_set_uint (value, lineoffire->unique_id);
      break;
    case PROP_FRAMES_THRESHOLD:
      g_value_set_boolean (value, lineoffire->frames_threshold);
      break;
    case PROP_DELETE_THRESHOOLD:
      g_value_set_boolean (value, lineoffire->delete_threshold);
      break;
    case PROP_ALARM_COOLDOWN_PERIOD:
      g_value_set_uint (value, lineoffire->alarm_cooldown_period);
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
gst_lineoffire_start (GstBaseTransform * btrans)
{
  GstLineOfFire *lineoffire = GST_LINEOFFIRE (btrans);

  return TRUE;
}

/**
 * Stop the output thread and free up all the resources
 */
static gboolean
gst_lineoffire_stop (GstBaseTransform * btrans)
{
  GstLineOfFire *lineoffire = GST_LINEOFFIRE (btrans);

  delete lineoffire->curr_image;
  lineoffire->curr_image = NULL;

  GST_DEBUG_OBJECT (lineoffire, "deleted Alarm Image \n");

  return TRUE;
}

/**
 * Called when source / sink pad capabilities have been negotiated.
 */
static gboolean
gst_lineoffire_set_caps (GstBaseTransform * btrans, GstCaps * incaps,
    GstCaps * outcaps)
{
    GstLineOfFire *lineoffire = GST_LINEOFFIRE (btrans);
    gst_video_info_from_caps (&lineoffire->video_info, incaps);

    /* requires RGBA format for performing operations in opencv */
    if (lineoffire->video_info.finfo->format != GST_VIDEO_FORMAT_RGBA) {
        GST_ELEMENT_ERROR (lineoffire, STREAM, FAILED,
            ("input format should be RGBA when using blur-objects property"), (NULL));
        goto error;
    }

    return TRUE;

    error:
    return FALSE;
}

static GstFlowReturn
gst_lineoffire_transform_ip (GstBaseTransform *btrans, GstBuffer * inbuf)
{
    GstLineOfFire *lineoffire = GST_LINEOFFIRE (btrans);
    // GstMapInfo in_map_info;
    GstFlowReturn flow_ret = GST_FLOW_ERROR;

    NvBufSurface *surface = NULL;
    NvDsBatchMeta *batch_meta = NULL;
    lineoffire->frame_num++;

    // memset (&in_map_info, 0, sizeof (in_map_info));
    // if (!gst_buffer_map (inbuf, &in_map_info, GST_MAP_READ)) {
    //     g_print ("Error: Failed to map gst buffer\n");
    //     goto error;
    // }

    // nvds_set_input_system_timestamp (inbuf, GST_ELEMENT_NAME (lineoffire));
    // surface = (NvBufSurface *) in_map_info.data;
    // GST_DEBUG_OBJECT (lineoffire,
    //     "Processing Frame %" G_GUINT64_FORMAT " Surface %p\n",
    //     lineoffire->frame_num, surface);

    batch_meta = gst_buffer_get_nvds_batch_meta (inbuf);
    if (batch_meta == nullptr) {
        GST_ELEMENT_ERROR (lineoffire, STREAM, FAILED,
            ("NvDsBatchMeta not found for input buffer."), (NULL));
        return GST_FLOW_ERROR;
    }

    GstPadProbeReturn val_ret = Ingressium::LineFireValidation(batch_meta, &(*lineoffire));
    if (val_ret != GST_PAD_PROBE_OK)
        return GST_FLOW_ERROR;

    return GST_FLOW_OK;

    // error:
    //     nvds_set_output_system_timestamp (inbuf, GST_ELEMENT_NAME (lineoffire));
    //     gst_buffer_unmap (inbuf, &in_map_info);
    //     return flow_ret;
}

void
UserMeta::setTrackerId(int id) {
    tracker_id = id;
}

int
UserMeta::getTrackerId() {
    return tracker_id;
}

void
UserMeta::setCoordinates(int x_val, int y_val, int w_val, int h_val) {
    x = x_val;
    y = y_val;
    w = w_val;
    h = h_val;
}

std::tuple<int, int, int, int>
UserMeta::getCoordinates() {
    return std::make_tuple(x, y, w, h);
}

void
UserMeta::setFirstTimer(std::chrono::system_clock::time_point first_timer) {
    first_clock = first_timer;
}

std::chrono::system_clock::time_point
UserMeta::getFirstTimer() {
    return first_clock;
}

void
UserMeta::setUpdateTimer(std::chrono::system_clock::time_point update_timer) {
    update_clock = update_timer;
}

std::chrono::system_clock::time_point
UserMeta::getUpdateTimer() {
    return update_clock;
}

void
UserMeta::addPersonCount() {
    person_count++;
}

int
UserMeta::getPersonCount() {
    return person_count;
}

void
UserMeta::setAlarmStatus(bool flag) {
    alarm = flag;
}

bool
UserMeta::getAlarmStatus() {
    return alarm;
}

void
Ingressium::changeBBoxColor(gpointer obj_meta_data, int has_bg_color,
float red, float green, float blue, float alpha) {
    NvDsObjectMeta *obj_meta = (NvDsObjectMeta *)obj_meta_data;
    obj_meta->rect_params.border_width = 8;
    obj_meta->rect_params.border_color.red = red;
    obj_meta->rect_params.border_color.green = green;
    obj_meta->rect_params.border_color.blue = blue;
    obj_meta->rect_params.border_color.alpha = alpha;
    obj_meta->text_params.font_params.font_size = 14;
}

void
Ingressium::LineFireDisplayMeta(NvDsBatchMeta* batch_meta, NvDsFrameMeta* frame_meta, GstLineOfFire *lineoffire) {

    NvDsDisplayMeta *display_meta_roi = NULL;
    NvOSD_LineParams *line_params = NULL;
    NvOSD_TextParams *text_params = NULL;
    display_meta_roi = nvds_acquire_display_meta_from_pool(batch_meta);
    line_params = display_meta_roi->line_params;
    text_params = display_meta_roi->text_params;

    // Loop to set colors
    for (NvDsMetaList * l_obj = frame_meta->obj_meta_list; l_obj != NULL;
        l_obj = l_obj->next) {

        NvDsObjectMeta *obj = (NvDsObjectMeta *) l_obj->data;

        if (obj == NULL) {
            // Ignore Null object.
            continue;
        }

        int class_index = obj->class_id;
        int unique_component_id = obj->unique_component_id;
        int cur_obj_id = obj->object_id;

        if(class_index == PERSON && unique_component_id == LINEFIRE_DETECTOR) {
            std::vector<LineFire::UserMeta*>::iterator iter;
            iter = std::find_if(user_id_tracker.begin(), user_id_tracker.end(),
            [&cur_obj_id](LineFire::UserMeta *e) { return e->getTrackerId() == cur_obj_id; });
            if ((*iter)->getPersonCount() > lineoffire->frames_threshold) {
                changeBBoxColor(obj, 1, 1.0, 0.0, 0.0, 0.25);
            }
            else {
                changeBBoxColor(obj, 1, 1.0, 1.0, 1.0, 0.25);
            }
        }
        else {}
    }
}


void
Ingressium::addPersonData(NvDsObjectMeta* obj_meta) {
    int cur_obj_id = obj_meta->object_id;

    std::vector<LineFire::UserMeta*>::iterator iter;

    int x, y, w, h;

    x = obj_meta->rect_params.left;
    y = obj_meta->rect_params.top;
    w = obj_meta->rect_params.width;
    h = obj_meta->rect_params.height;

    iter = std::find_if(user_id_tracker.begin(), user_id_tracker.end(),
    [&cur_obj_id](LineFire::UserMeta *e) { return e->getTrackerId() == cur_obj_id; });

    if (iter != user_id_tracker.end()) {
        int index = std::distance(user_id_tracker.begin(), iter);
        // Update count
        user_id_tracker[index]->addPersonCount();
        user_id_tracker[index]->setCoordinates(x, y, w, h);
    }
    else {
        LineFire::UserMeta *u = new LineFire::UserMeta();

        u->setTrackerId(cur_obj_id);
        u->addPersonCount();
        std::chrono::system_clock::time_point time_now =
        std::chrono::system_clock::now();
        u->setFirstTimer(time_now);
        u->setCoordinates(x, y, w, h);
        user_id_tracker.emplace_back(u);
    }
}

std::tuple<bool, int, std::tuple<int, int, int, int>>
Ingressium::LineFireResult(int frames_threshold, int delete_threshold) {
    bool alarm = false;
    int tracker = -1;
    std::tuple<int, int, int, int> person_coords;

    for(auto iter = user_id_tracker.begin(); iter != user_id_tracker.end(); ++iter) {
        bool flag = false;
        int track = 0;

        int person_count = 0;

        person_count = (*iter)->getPersonCount();
        if(person_count > frames_threshold) {
            if(!(*iter)->getAlarmStatus()) {
                flag = true;
                (*iter)->setAlarmStatus(true);
                track = (*iter)->getTrackerId();
            }
            else {
                continue;
            }
        }
        else {
            flag = false;
            (*iter)->setAlarmStatus(false);
            continue;
        }

        std::cout << "[TrackerID:" << (*iter)->getTrackerId()  << "]"
        << "[Algorithm:" << "LINE_OF_FIRE" << "]"
        << "[Person Dets: " << person_count  << "]" << std::endl;

        if(!alarm) {
            alarm = flag;
            tracker = track;
            person_coords = (*iter)->getCoordinates();
            break;
        }
    }

    // Clear lagging values
    for(auto iter = user_id_tracker.begin(); iter != user_id_tracker.end(); ++iter) {
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now() - (*iter)->getUpdateTimer()).count();
        if (diff > delete_threshold) {
            user_id_tracker.erase(iter);
            --iter;
        }
    }
    user_id_tracker.shrink_to_fit();
    return std::make_tuple(alarm, tracker, person_coords);
}

GstPadProbeReturn
Ingressium::LineFireValidation(NvDsBatchMeta* batch_meta, GstLineOfFire* lineoffire) {

    NvDsFrameMeta *frame_meta = NULL;
    NvDsMetaList * l_frame = NULL;

    for (NvDsMetaList * l_frame = batch_meta->frame_meta_list; l_frame != NULL;
    l_frame = l_frame->next) {

        frame_meta = (NvDsFrameMeta *)l_frame->data;
        if (frame_meta == NULL) {
            continue;
        }

        for (NvDsMetaList * l_obj = frame_meta->obj_meta_list; l_obj != NULL;
            l_obj = l_obj->next) {
            NvDsObjectMeta *obj = (NvDsObjectMeta *) l_obj->data;

            if (obj == NULL) {
                // Ignore Null object.
                continue;
            }

            gint class_index = obj->class_id;
            int unique_component_id = obj->unique_component_id;

            if(class_index == PERSON && unique_component_id == LINEFIRE_DETECTOR) {
                addPersonData(obj);
            }
        }

        // Set Object Colors
        LineFireDisplayMeta(batch_meta, frame_meta, &(*lineoffire));

        // Run logic
        auto[alarm, tracker, person_coords] = LineFireResult(
            lineoffire->frames_threshold,
            lineoffire->delete_threshold
        );

        if(alarm) {
            std::chrono::system_clock::time_point alarm_time;
            alarm_time = lineoffire->alarm_time;
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now() - alarm_time).count();

            if(!(diff >= lineoffire->alarm_cooldown_period)) {
                continue;
            }

            std::cout<< "LINE OF FIRE raised"<<std::endl;

            lineoffire->alarm_time = std::chrono::system_clock::now();

        }
    }

    return GST_PAD_PROBE_OK;
}

/**
 * Boiler plate for registering a plugin and an element.
 */
static gboolean
lineoffire_plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_lineoffire_debug, "lineoffire", 0,
      "lineoffire plugin");

  return gst_element_register (plugin, "lineoffire", GST_RANK_PRIMARY,
      GST_TYPE_LINEOFFIRE);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    nvdsgst_lineoffire,
    DESCRIPTION, lineoffire_plugin_init, "5.1", LICENSE, BINARY_PACKAGE, URL)
