#pragma once
#include "lineoffire_libs.hpp"

/* Package and library details required for plugin_init */
#define PACKAGE "lineoffire"
#define VERSION "1.0"
#define LICENSE "Proprietary"
#define DESCRIPTION "Plugin for Line Of Fire usecase."
#define BINARY_PACKAGE "Line Of Fire plugin"
#define URL "https://github.com/jayanthvarma134/"


G_BEGIN_DECLS
/* Standard boilerplate stuff */
typedef struct _GstLineOfFire GstLineOfFire;
typedef struct _GstLineOfFireClass GstLineOfFireClass;

/* Standard boilerplate stuff */
#define GST_TYPE_LINEOFFIRE (gst_lineoffire_get_type())
#define GST_LINEOFFIRE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_LINEOFFIRE,GstLineOfFire))
#define GST_LINEOFFIRE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_LINEOFFIRE,GstLineOfFireClass))
#define GST_LINEOFFIRE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_LINEOFFIRE, GstLineOfFireClass))
#define GST_IS_LINEOFFIRE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_LINEOFFIRE))
#define GST_IS_LINEOFFIRE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_LINEOFFIRE))
#define GST_LINEOFFIRE_CAST(obj)  ((GstLineOfFire *)(obj))

struct _GstLineOfFire
{
  GstBaseTransform base_trans;
  guint unique_id;
  guint64 frame_num;
  GstVideoInfo video_info;

  cv::Mat *curr_image;
  guint frames_threshold;
  guint delete_threshold;
  guint alarm_cooldown_period;
  std::chrono::system_clock::time_point alarm_time;

};

// Boiler plate stuff
struct _GstLineOfFireClass
{
  GstBaseTransformClass parent_class;
};

GType gst_lineoffire_get_type (void);

namespace LineFire {
    class UserMeta {
        private:
            int tracker_id;
            int x, y, w, h;
            std::chrono::system_clock::time_point first_clock;
            std::chrono::system_clock::time_point update_clock;
            int person_count;
            bool alarm;
            std::chrono::system_clock::time_point alarm_clock;

        public:
            void
            setTrackerId(int id);

            int
            getTrackerId();

            void
            setCoordinates(int x_val, int y_val, int w_val, int h_val);

            std::tuple<int, int, int, int>
            getCoordinates();

            void
            setFirstTimer(std::chrono::system_clock::time_point update_timer);

            std::chrono::system_clock::time_point
            getFirstTimer();

            void
            setUpdateTimer(std::chrono::system_clock::time_point update_timer);

            std::chrono::system_clock::time_point
            getUpdateTimer();   

            void
            addPersonCount();

            int
            getPersonCount();

            void
            setAlarmStatus(bool flag);

            bool
            getAlarmStatus();

            UserMeta() {
                tracker_id = 0;
                x = 0, y = 0, w = 0, h = 0;
                person_count = 0;
                alarm = false;
            }
            ~UserMeta() {}
    };

    class Ingressium {
    private:

        // Data stuctures to store detections
        inline static std::vector<LineFire::UserMeta*> user_id_tracker;

        static void
        addPersonData(NvDsObjectMeta* obj_meta);

        static void
        LineFireDisplayMeta(NvDsBatchMeta* batch_meta, NvDsFrameMeta* frame_meta, GstLineOfFire* lineoffire );

        static std::tuple<bool, int, std::tuple<int, int, int, int>>
        LineFireResult(int frames_threshold, int delete_threshold);

        static void
        changeBBoxColor(gpointer obj_meta_data, int has_bg_color,
        float red, float green, float blue, float alpha);

    public :

        static GstPadProbeReturn
        LineFireValidation(NvDsBatchMeta* batch_meta, GstLineOfFire* lineoffire);

        Ingressium() {}
        ~Ingressium () {}
    };
}

G_END_DECLS