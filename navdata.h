#ifndef __ARDRONE_PROXY_NAVDATA_H__
#define __ARDRONE_PROXY_NAVDATA_H__

#define NAVDATA_HEADER            0x55667788

struct navdata_option_t {
    uint16_t tag;
    uint16_t size;
    uint8_t data[];
} __attribute__ ((packed));

struct navdata_t {
  uint32_t header;
  uint32_t mykonos_state;
  uint32_t sequence;
  uint32_t vision_defined;
  navdata_option_t  options[1];
} __attribute__ ((packed));


enum navdata_tag_t {
    NAVDATA_DEMO_TAG = 0,
    NAVDATA_VISION_DETECT_TAG = 16,
    NAVDATA_IPHONE_ANGLES_TAG = 18,
    NAVDATA_CKS_TAG = 0xFFFF
};


typedef struct _matrix33_t
{ 
  float m11;
  float m12;
  float m13;
  float m21;
  float m22;
  float m23;
  float m31;
  float m32;
  float m33;
} matrix33_t;

typedef struct _vector31_t {
  union {
    float v[3];
    struct    
    {
      float x;
      float y;
      float z;
    };
  };
} vector31_t; 

typedef struct _navdata_demo_t {
    uint16_t    tag;
    uint16_t    size;

    uint32_t    ctrl_state;             /*!< instance of #def_mykonos_state_mask_t */
    uint32_t    vbat_flying_percentage; /*!< battery voltage filtered (mV) */

    float   theta;                  /*!< UAV's attitude */
    float   phi;                    /*!< UAV's attitude */
    float   psi;                    /*!< UAV's attitude */

    int32_t     altitude;               /*!< UAV's altitude */

    float  vx;                     /*!< UAV's estimated linear velocity */
    float  vy;                     /*!< UAV's estimated linear velocity */
    float  vz;                     /*!< UAV's estimated linear velocity */

    uint32_t    num_frames;			  /*!< streamed frame index */
    
    matrix33_t  detection_camera_rot;   
    vector31_t  detection_camera_trans; 
    uint32_t    detection_tag_index;    
    uint32_t    detection_camera_type; 

    // Camera parameters compute by drone
    matrix33_t  drone_camera_rot;         
    vector31_t  drone_camera_trans;    
} __attribute__ ((packed)) navdata_demo_t;

typedef struct _navdata_cks_t {
    uint16_t  tag;
    uint16_t  size;

    // Checksum for all navdatas (including options)
    uint32_t  cks;
} __attribute__ ((packed)) navdata_cks_t;

enum {
  MYKONOS_FLY_MASK            = 1 << 0, /*!< FLY MASK : (0) mykonos is landed, (1) mykonos is flying */
  MYKONOS_VIDEO_MASK          = 1 << 1, /*!< VIDEO MASK : (0) video disable, (1) video enable */
  MYKONOS_VISION_MASK         = 1 << 2, /*!< VISION MASK : (0) vision disable, (1) vision enable */
  MYKONOS_CONTROL_MASK        = 1 << 3, /*!< CONTROL ALGO : (0) euler angles control, (1) angular speed control */
  MYKONOS_ALTITUDE_MASK       = 1 << 4, /*!< ALTITUDE CONTROL ALGO : (0) altitude control inactive (1) altitude control active */
  MYKONOS_USER_FEEDBACK_START = 1 << 5, /*!< USER feedback : Start button state */
  MYKONOS_COMMAND_MASK        = 1 << 6, /*!< Control command ACK : (0) None, (1) one received */
  MYKONOS_TRIM_COMMAND_MASK   = 1 << 7, /*!< Trim command ACK : (0) None, (1) one received */
  MYKONOS_TRIM_RUNNING_MASK   = 1 << 8, /*!< Trim running : (0) none, (1) running */
  MYKONOS_TRIM_RESULT_MASK    = 1 << 9, /*!< Trim result : (0) failed, (1) succeeded */
  MYKONOS_NAVDATA_DEMO_MASK   = 1 << 10, /*!< Navdata demo : (0) All navdata, (1) only navdata demo */
  MYKONOS_NAVDATA_BOOTSTRAP   = 1 << 11, /*!< Navdata bootstrap : (0) options sent in all or demo mode, (1) no navdata options sent */
  MYKONOS_MOTORS_BRUSHED      = 1 << 12, /*!< Motors brushed : (0) brushless, (1) brushed */
  MYKONOS_COM_LOST_MASK   = 1 << 13, /*!< Communication Lost : (1) com problem, (0) Com is ok */
  MYKONOS_GYROS_ZERO          = 1 << 14, /*!< Bit means that there's an hardware problem with gyrometers */
  MYKONOS_VBAT_LOW            = 1 << 15, /*!< VBat low : (1) too low, (0) Ok */
  MYKONOS_VBAT_HIGH           = 1 << 16, /*!< VBat high (US mad) : (1) too high, (0) Ok */
  MYKONOS_TIMER_ELAPSED       = 1 << 17, /*!< Timer elapsed : (1) elapsed, (0) not elapsed */
  MYKONOS_NOT_ENOUGH_POWER    = 1 << 18, /*!< Power : (0) Ok, (1) not enough to fly */
  MYKONOS_ANGLES_OUT_OF_RANGE = 1 << 19, /*!< Angles : (0) Ok, (1) out of range */
  MYKONOS_WIND_MASK           = 1 << 20, /*!< Wind : (0) Ok, (1) too much to fly */
  MYKONOS_ULTRASOUND_MASK     = 1 << 21, /*!< Ultrasonic sensor : (0) Ok, (1) deaf */
  MYKONOS_CUTOUT_MASK         = 1 << 22, /*!< Cutout system detection : (0) Not detected, (1) detected */
  MYKONOS_PIC_VERSION_MASK    = 1 << 23, /*!< PIC Version number OK : (0) a bad version number, (1) version number is OK */
  MYKONOS_ATCODEC_THREAD_ON   = 1 << 24, /*!< ATCodec thread ON : (0) thread OFF (1) thread ON */
  MYKONOS_NAVDATA_THREAD_ON   = 1 << 25, /*!< Navdata thread ON : (0) thread OFF (1) thread ON */
  MYKONOS_VIDEO_THREAD_ON     = 1 << 26, /*!< Video thread ON : (0) thread OFF (1) thread ON */
  MYKONOS_ACQ_THREAD_ON       = 1 << 27, /*!< Acquisition thread ON : (0) thread OFF (1) thread ON */
  MYKONOS_CTRL_WATCHDOG_MASK  = 1 << 28, /*!< CTRL watchdog : (1) delay in control execution (> 5ms), (0) control is well scheduled */
  MYKONOS_ADC_WATCHDOG_MASK   = 1 << 29, /*!< ADC Watchdog : (1) delay in uart2 dsr (> 5ms), (0) uart2 is good */
  MYKONOS_COM_WATCHDOG_MASK   = 1 << 30, /*!< Communication Watchdog : (1) com problem, (0) Com is ok */
  MYKONOS_EMERGENCY_MASK      = 1 << 31  /*!< Emergency landing : (0) no emergency, (1) emergency */
};

enum {
  NO_CONTROL_MODE = 0,          // Doing nothing
  MYKONOS_UPDATE_CONTROL_MODE,  // Mykonos software update reception (update is done next run)
                                // After event completion, card should power off
  PIC_UPDATE_CONTROL_MODE,      // Mykonos pic software update reception (update is done next run)
                                // After event completion, card should power off
  LOGS_GET_CONTROL_MODE,        // Send previous run's logs
  CFG_GET_CONTROL_MODE,         // Send activ configuration
  ACK_CONTROL_MODE              // Reset command mask in navdata
};


#endif
