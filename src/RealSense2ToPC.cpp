// -*- C++ -*-
/*!
 * @file  RealSense2ToPC.cpp
 * @brief Point Cloud Grabber RTC Using Intel RealSense SDK 2 
 * @date $Date$
 *
 * $Id$
 */

#include "RealSense2ToPC.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;
using namespace Eigen;


// Module specification
// <rtc-template block="module_spec">
static const char* realsense2topc_spec[] =
  {
    "implementation_id", "RealSense2ToPC",
    "type_name",         "RealSense2ToPC",
    "description",       "Point Cloud Grabber RTC Using Intel RealSense SDK 2 ",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "PointCloud",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.transX", "0.0",
    "conf.default.transY", "0.0",
    "conf.default.transZ", "0.0",
    "conf.default.rotX", "0.0",
    "conf.default.rotY", "0.0",
    "conf.default.rotZ", "0.0",

    // Widget
    "conf.__widget__.transX", "text",
    "conf.__widget__.transY", "text",
    "conf.__widget__.transZ", "text",
    "conf.__widget__.rotX", "text",
    "conf.__widget__.rotY", "text",
    "conf.__widget__.rotZ", "text",
    // Constraints

    "conf.__type__.transX", "double",
    "conf.__type__.transY", "double",
    "conf.__type__.transZ", "double",
    "conf.__type__.rotX", "double",
    "conf.__type__.rotY", "double",
    "conf.__type__.rotZ", "double",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
RealSense2ToPC::RealSense2ToPC(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_pcOut("pc", m_pc)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
RealSense2ToPC::~RealSense2ToPC()
{
}



RTC::ReturnCode_t RealSense2ToPC::onInitialize()
{
  RTC_INFO(("onInitialize()"));
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
  bindParameter("transX", m_transX, "0.0");
  bindParameter("transY", m_transY, "0.0");
  bindParameter("transZ", m_transZ, "0.0");
  bindParameter("rotX", m_rotX, "0.0");
  bindParameter("rotY", m_rotY, "0.0");
  bindParameter("rotZ", m_rotZ, "0.0");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RealSense2ToPC::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSense2ToPC::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSense2ToPC::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t RealSense2ToPC::onActivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onActivated()"));
  double radX = m_rotX*M_PI / 180;
  double radY = m_rotY*M_PI / 180;
  double radZ = m_rotZ*M_PI / 180;
  m_transform
    = Translation3f(m_transX, m_transY, m_transZ)
    *AngleAxisf(radZ, Vector3f::UnitZ())
    *AngleAxisf(radY, Vector3f::UnitY())
    *AngleAxisf(radX+M_PI, Vector3f::UnitX());
  cout << "m_transform:" << endl << m_transform.matrix() << endl;

  try {
    m_interface = boost::make_shared<pcl::RealSense2Grabber>();
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
    boost::function<void(const pcl::PointCloud<PointT>::ConstPtr&)> f = boost::bind(&RealSense2ToPC::cloud_cb, this, _1);
    m_interface->registerCallback(f);
    m_interface->start();
  } catch (exception& e) {
    RTC_ERROR(("Failed to create a grabber: %s", e.what()));
    return RTC::RTC_ERROR;
  } catch (...) {
    RTC_ERROR(("An exception occurred while starting grabber"));
    return RTC::RTC_ERROR;
  }
  return RTC::RTC_OK;
}


RTC::ReturnCode_t RealSense2ToPC::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onDeactivated()"));
  m_interface->stop();
  m_interface.reset();

  return RTC::RTC_OK;
}


RTC::ReturnCode_t RealSense2ToPC::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RealSense2ToPC::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSense2ToPC::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSense2ToPC::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSense2ToPC::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RealSense2ToPC::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

void RealSense2ToPC::cloud_cb(const pcl::PointCloud<PointT>::ConstPtr &cloudOrg)
{
  pcl::PointCloud<PointT>::Ptr cloud(new pcl::PointCloud<PointT>());
  pcl::transformPointCloud(*cloudOrg, *cloud, m_transform);
  setTimestamp(m_pc);
  m_pc.width = cloud->width;
  m_pc.height = cloud->height;
  m_pc.row_step = m_pc.point_step*m_pc.width;
  m_pc.data.length(m_pc.height*m_pc.row_step);
  float *dst_cloud = (float *)m_pc.data.get_buffer();
  for (unsigned int i = 0; i<cloud->points.size(); i++) {
    dst_cloud[0] = cloud->points[i].x;
    dst_cloud[1] = cloud->points[i].y;
    dst_cloud[2] = cloud->points[i].z;
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
    RTC_INFO(("Average framerate: %lf Hz", double(count) / double(now - last)));
#endif
    count = 0;
    last = now;
  }
}


extern "C"
{
 
  void RealSense2ToPCInit(RTC::Manager* manager)
  {
    coil::Properties profile(realsense2topc_spec);
    manager->registerFactory(profile,
                             RTC::Create<RealSense2ToPC>,
                             RTC::Delete<RealSense2ToPC>);
  }
  
};


