// -*- C++ -*-
/*!
 * @file  RealSenseToPC.cpp
 * @brief Intel RealSense3D Grabber
 * @date $Date$
 *
 * $Id$
 */

#include "RealSenseToPC.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

typedef pcl::RealSenseGrabber::TemporalFilteringType TFT;

// Module specification
// <rtc-template block="module_spec">
static const char* realsensetopc_spec[] =
  {
    "implementation_id", "RealSenseToPC",
    "type_name",         "RealSenseToPC",
    "description",       "Intel RealSense3D Grabber",
    "version",           "1.0.0",
    "vendor",            "OECU",
    "category",          "PointCloud",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.window", "3",
    "conf.default.threshold", "6",
    "conf.default.temporal_filtering", "0",
    "conf.default.with_bilateral", "0",
    "conf.default.bilateral_sigma_s", "5",
    "conf.default.bilateral_sigma_r", "0.05",
    "conf.default.mode", "0",
    "conf.default.device_id", " ",

    // Widget
    "conf.__widget__.window", "slider.1",
    "conf.__widget__.threshold", "slider.1",
    "conf.__widget__.temporal_filtering", "radio",
    "conf.__widget__.with_bilateral", "radio",
    "conf.__widget__.bilateral_sigma_s", "slider.1",
    "conf.__widget__.bilateral_sigma_r", "slider.0.01",
    "conf.__widget__.mode", "text",
    "conf.__widget__.device_id", "text",
    // Constraints
    "conf.__constraints__.window", "1<=x<=100",
    "conf.__constraints__.threshold", "0<=x<=15",
    "conf.__constraints__.temporal_filtering", "(0,1,2)",
    "conf.__constraints__.with_bilateral", "(0,1)",
    "conf.__constraints__.bilateral_sigma_s", "1<=x<=100",
    "conf.__constraints__.bilateral_sigma_r", "0.01<=x<=1",

    "conf.__type__.window", "short",
    "conf.__type__.threshold", "short",
    "conf.__type__.temporal_filtering", "short",
    "conf.__type__.with_bilateral", "short",
    "conf.__type__.bilateral_sigma_s", "short",
    "conf.__type__.bilateral_sigma_r", "float",
    "conf.__type__.mode", "short",
    "conf.__type__.device_id", "string",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
RealSenseToPC::RealSenseToPC(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_pcOut("pc", m_pc)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
RealSenseToPC::~RealSenseToPC()
{
}



RTC::ReturnCode_t RealSenseToPC::onInitialize()
{
  cout << "RealSenseToPC::onInitialize()" << endl;
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("pc", m_pcOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("window", m_window, "3");
  bindParameter("threshold", m_threshold, "6");
  bindParameter("temporal_filtering", m_temporal_filtering, "0");
  bindParameter("with_bilateral", m_with_bilateral, "0");
  bindParameter("bilateral_sigma_s", m_bilateral_sigma_s, "5");
  bindParameter("bilateral_sigma_r", m_bilateral_sigma_r, "0.05");
  bindParameter("mode", m_mode, "0");
  bindParameter("device_id", m_device_id, " ");
  // </rtc-template>
  
  printDeviceList();

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RealSenseToPC::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSenseToPC::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSenseToPC::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t RealSenseToPC::onActivated(RTC::UniqueId ec_id)
{
  cout << "RealSenseToPC::onActivated()" << endl;
  try {
    string device_id = m_device_id;
    if (device_id == " ") device_id = "";
    m_interface = boost::make_shared<pcl::RealSenseGrabber>(device_id);
    vector<pcl::RealSenseGrabber::Mode> xyz_modes = m_interface->getAvailableModes(true);
    vector<pcl::RealSenseGrabber::Mode> xyzrgba_modes = m_interface->getAvailableModes(false);
    if (m_mode <= 0) {
      cout << "Default mode" << endl;
    } else if (m_mode && m_mode <= xyz_modes.size()) {
      cerr << "Depth only is not supported." << endl;
      return RTC::RTC_ERROR;
    } else if (m_mode <= xyz_modes.size() + xyzrgba_modes.size()) {
      cout << "mode: " << m_mode << endl;
      m_interface->setMode(xyzrgba_modes[m_mode-xyz_modes.size ()-1], true);
    } else {
      cerr << "mode: " << m_mode << " is not supported." << endl;
      return RTC::RTC_ERROR;
    }

    m_pc.type = "xyzrgb";
    m_pc.fields.length(6);
    m_pc.fields[0].name = "x";
    m_pc.fields[0].offset = 0;
    m_pc.fields[0].data_type = PointCloudTypes::FLOAT32;
    m_pc.fields[0].count = 4;
    m_pc.fields[1].name = "y";
    m_pc.fields[1].offset = 4;
    m_pc.fields[1].data_type = PointCloudTypes::FLOAT32;
    m_pc.fields[1].count = 4;
    m_pc.fields[2].name = "z";
    m_pc.fields[2].offset = 8;
    m_pc.fields[2].data_type = PointCloudTypes::FLOAT32;
    m_pc.fields[2].count = 4;
    m_pc.fields[3].name = "b";
    m_pc.fields[3].offset = 12;
    m_pc.fields[3].data_type = PointCloudTypes::UINT8;
    m_pc.fields[3].count = 1;
    m_pc.fields[4].name = "g";
    m_pc.fields[4].offset = 13;
    m_pc.fields[4].data_type = PointCloudTypes::UINT8;
    m_pc.fields[4].count = 1;
    m_pc.fields[5].name = "r";
    m_pc.fields[5].offset = 14;
    m_pc.fields[5].data_type = PointCloudTypes::UINT8;
    m_pc.fields[5].count = 1;
    m_pc.is_bigendian = false;
    m_pc.point_step = 16;
    m_pc.is_dense = false;
    boost::function<void (const pcl::PointCloud<PointT>::ConstPtr&)> f = boost::bind(&RealSenseToPC::cloud_cb, this, _1);
    m_interface->registerCallback(f);
    m_interface->start();
    pcl::RealSenseGrabber::Mode mode = m_interface->getMode();
#define print(x) cout << #x << ": " << x << endl
    print(mode.fps);
    print(mode.depth_width);
    print(mode.depth_height);
    print(mode.color_width);
    print(mode.color_height);
  } catch (pcl::io::IOException& e) {
    cerr << "Failed to create a grabber: " << e.what () << endl; //ここでプログラムが終了してしまう問題あり
    return RTC::RTC_ERROR;
  } catch (...) {
    cerr << "An exception occurred while starting grabber" << endl;
    return RTC::RTC_ERROR;
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t RealSenseToPC::onDeactivated(RTC::UniqueId ec_id)
{
  cout << "RealSenseToPC::onDeactivated()" << endl;
  m_interface->stop();
  m_interface = nullptr;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t RealSenseToPC::onExecute(RTC::UniqueId ec_id)
{
  if (m_window != m_window_prev ) {
    m_window_prev = m_window;
    if (m_window_prev < 1) {
      m_window_prev = 1;
    }
    cout << "Temporal filtering window size: " << m_window_prev << endl;
    m_interface->enableTemporalFiltering(TFT(m_temporal_filtering_prev), m_window_prev);
  }

  if (m_threshold != m_threshold_prev) {
    m_threshold_prev = m_threshold;
    if (m_threshold_prev < 0) {
      m_threshold_prev = 0;
    }
    if (m_threshold_prev > 15) {
      m_threshold_prev = 15;
    }
    cout << "Confidence threshold: " << m_threshold_prev << endl;
    m_interface->setConfidenceThreshold(m_threshold_prev);
  }

  if (m_temporal_filtering != m_temporal_filtering_prev) {
    m_temporal_filtering_prev = m_temporal_filtering;
    cout << "Temporal filtering: ";
    switch (m_temporal_filtering_prev) {
    case pcl::RealSenseGrabber::RealSense_None:
      cout <<"none" << endl;
      break;
    case pcl::RealSenseGrabber::RealSense_Median:
      cout << "median" << endl;
      break;
    case pcl::RealSenseGrabber::RealSense_Average:
      cout << "average" << endl;
      break;
    }
    m_interface->enableTemporalFiltering(TFT(m_temporal_filtering_prev), m_window_prev);
  }

  if (m_with_bilateral != m_with_bilateral_prev) {
    m_with_bilateral_prev = m_with_bilateral;
    cout << "Bilateral filtering: " << (m_with_bilateral_prev ? "ON" : "OFF") << endl;
  }

  if (m_bilateral_sigma_s != m_bilateral_sigma_s_prev) {
    m_bilateral_sigma_s_prev = m_bilateral_sigma_s;
    if (m_bilateral_sigma_s_prev < 1) {
      m_bilateral_sigma_s_prev = 1;
    }
    cout << "Bilateral filter spatial sigma: " << m_bilateral_sigma_s_prev << endl;
    m_bilateral.setSigmaS (m_bilateral_sigma_s_prev);
  }

  if (m_bilateral_sigma_r != m_bilateral_sigma_r_prev) {
    m_bilateral_sigma_r_prev = m_bilateral_sigma_r;
    if (m_bilateral_sigma_r_prev < 0.01) {
      m_bilateral_sigma_r_prev = 0.01;
    }
    cout << "Bilateral filter range sigma: " << m_bilateral_sigma_r_prev << endl;
    m_bilateral.setSigmaR(m_bilateral_sigma_r_prev);
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RealSenseToPC::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSenseToPC::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSenseToPC::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSenseToPC::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSenseToPC::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

void RealSenseToPC::cloud_cb(const pcl::PointCloud<PointT>::ConstPtr &cloudOrg)
{
  pcl::PointCloud<PointT>::ConstPtr cloud;
  if (m_with_bilateral_prev) {
    m_bilateral.setInputCloud (cloudOrg);
    pcl::PointCloud<PointT>::Ptr filtered (new pcl::PointCloud<PointT>);
    m_bilateral.filter (*filtered);
    cloud = filtered;
  } else {
    cloud = cloudOrg;
  }

  setTimestamp(m_pc);
  m_pc.width = cloud->width;
  m_pc.height = cloud->height;
  m_pc.row_step = m_pc.point_step*m_pc.width;
  m_pc.data.length(m_pc.height*m_pc.row_step);
  float *dst_cloud = (float *)m_pc.data.get_buffer();
  for (unsigned int i=0; i<cloud->points.size(); i++){
#if 1
    dst_cloud[0] = -cloud->points[i].x;
    dst_cloud[1] =  cloud->points[i].y;
    dst_cloud[2] = -cloud->points[i].z;
#else
    dst_cloud[0] =  cloud->points[i].x;
    dst_cloud[1] =  cloud->points[i].y;
    dst_cloud[2] =  cloud->points[i].z;
#endif
    dst_cloud[3] = -cloud->points[i].rgb;
    dst_cloud += 4;
  }
  m_pcOut.write();

  static int count = 0;
  static double last = 0;

  if (++count == 30)
  {
    double now = pcl::getTime();
#if 1
    cout << "Average framerate: " << double(count)/double(now - last) << " Hz" <<  endl;
#endif
    count = 0;
    last = now;
  }
}

void RealSenseToPC::printDeviceList()
{
  typedef boost::shared_ptr<pcl::RealSenseGrabber> RealSenseGrabberPtr;
  std::vector<RealSenseGrabberPtr> grabbers;
  std::cout << "Connected devices: ";
  boost::format fmt ("\n  #%i  %s");
  boost::format fmt_dm ("\n        %2i) %d Hz  %dx%d Depth");
  boost::format fmt_dcm ("\n        %2i) %d Hz  %dx%d Depth  %dx%d Color");
  while (true)
  {
    try
    {
      grabbers.push_back (RealSenseGrabberPtr (new pcl::RealSenseGrabber));
      std::cout << boost::str (fmt % grabbers.size () % grabbers.back ()->getDeviceSerialNumber ());
      std::vector<pcl::RealSenseGrabber::Mode> xyz_modes = grabbers.back ()->getAvailableModes (true);
      std::cout << "\n      Depth modes:";
      if (xyz_modes.size ())
        for (size_t i = 0; i < xyz_modes.size (); ++i)
          std::cout << boost::str (fmt_dm % (i + 1) % xyz_modes[i].fps % xyz_modes[i].depth_width % xyz_modes[i].depth_height);
      else
      {
        std::cout << " none";
      }
      std::vector<pcl::RealSenseGrabber::Mode> xyzrgba_modes = grabbers.back ()->getAvailableModes (false);
      std::cout << "\n      Depth + color modes:";
      if (xyz_modes.size ())
        for (size_t i = 0; i < xyzrgba_modes.size (); ++i)
        {
          const pcl::RealSenseGrabber::Mode& m = xyzrgba_modes[i];
          std::cout << boost::str (fmt_dcm % (i + xyz_modes.size () + 1) % m.fps % m.depth_width % m.depth_height % m.color_width % m.color_height);
        }
      else
        std::cout << " none";
    }
    catch (pcl::io::IOException& e)
    {
      break;
    }
  }
  if (grabbers.size ())
    std::cout << std::endl;
  else
    std::cout << "none" << std::endl;
}

extern "C"
{
 
  void RealSenseToPCInit(RTC::Manager* manager)
  {
    coil::Properties profile(realsensetopc_spec);
    manager->registerFactory(profile,
                             RTC::Create<RealSenseToPC>,
                             RTC::Delete<RealSenseToPC>);
  }
  
};


