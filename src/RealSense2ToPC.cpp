// -*- C++ -*-
/*!
 * @file  RealSense2ToPC.cpp
 * @brief Point Cloud Grabber RTC Using Intel RealSense SDK 2 
 * @date $Date$
 *
 * $Id$
 */

#define _USE_MATH_DEFINES
#include "RealSense2ToPC.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;
using namespace Eigen;

#define print(x) cout << #x " = " << x << endl

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
    "conf.default.colorResolution", "640x480",
    "conf.default.depthResolution", "640x480",

    // Widget
    "conf.__widget__.transX", "text",
    "conf.__widget__.transY", "text",
    "conf.__widget__.transZ", "text",
    "conf.__widget__.rotX", "text",
    "conf.__widget__.rotY", "text",
    "conf.__widget__.rotZ", "text",
    "conf.__widget__.colorResolution", "radio",
    "conf.__widget__.depthResolution", "radio",
    // Constraints
    "conf.__constraints__.colorResolution", "(320x180,320x240,424x240,640x360,640x480,848x480,960x540,1280x720,1920x1080)",
    "conf.__constraints__.depthResolution", "(424x240,480x270,640x360,640x400,640x480,848x480,1280x720,1280x800)",

    "conf.__type__.transX", "double",
    "conf.__type__.transY", "double",
    "conf.__type__.transZ", "double",
    "conf.__type__.rotX", "double",
    "conf.__type__.rotY", "double",
    "conf.__type__.rotZ", "double",
    "conf.__type__.colorResolution", "string",
    "conf.__type__.depthResolution", "string",

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
    m_pcOut("pc", m_pc),
    m_palign(nullptr)

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
  bindParameter("colorResolution", m_colorResolution, "640x480");
  bindParameter("depthResolution", m_depthResolution, "640x480");
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
  if (m_rotX == 0 && m_rotY == 0 && m_rotZ == 0 && m_transX == 0 && m_transY == 0 && m_transZ == 0) {
    m_coordinateTransformation = false;
  } else {
    m_coordinateTransformation = true;
    double radX = m_rotX*M_PI / 180;
    double radY = m_rotY*M_PI / 180;
    double radZ = m_rotZ*M_PI / 180;
    m_transform
      = Translation3f(m_transX, m_transY, m_transZ)
      *AngleAxisf(radZ, Vector3f::UnitZ())
      *AngleAxisf(radY, Vector3f::UnitY())
      *AngleAxisf(radX, Vector3f::UnitX());
    cout << "m_transform:" << endl << m_transform.matrix() << endl;
  }

  try {
    char dummy;
    istringstream isc(m_colorResolution);
    int wc, hc;
    isc >> wc >> dummy >> hc;
    istringstream isd(m_depthResolution);
    int wd, hd;
    isd >> wd >> dummy >> hd;
    RTC_INFO(("wc: %d, hc: %d, wd: %d, hd: %d", wc, hc, wd, hd));

    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, wc, hc, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, wd, hd, RS2_FORMAT_Z16, 30);

    rs2::pipeline_profile profile = m_pipe.start(cfg);

    if (wc == wd && hc == hd) {
      m_palign = nullptr;
    } else {
      m_palign = new rs2::align(RS2_STREAM_DEPTH);
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
    m_steadyStart = chrono::steady_clock::now();
    m_fpsCounter = 0;
  } catch (const rs2::error & e) {
    RTC_ERROR((("RealSense error calling " + e.get_failed_function() + "(" + e.get_failed_args() + "):").c_str()));
    RTC_ERROR(("    %s", e.what()));
    return RTC::RTC_ERROR;
  } catch (const std::exception& e) {
    RTC_ERROR((e.what()));
    return RTC::RTC_ERROR;
  } catch (...) {
    RTC_ERROR(("An exception occurred in onActivated()"));
    return RTC::RTC_ERROR;
  }
  RTC_INFO(("onActivated() done"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t RealSense2ToPC::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onDeactivated()"));
  m_pipe.stop();
  delete m_palign;
  return RTC::RTC_OK;
}

RTC::ReturnCode_t RealSense2ToPC::onExecute(RTC::UniqueId ec_id)
{
  try {
    rs2::frameset frameset;
    if (!m_pipe.poll_for_frames(&frameset)) {
      return RTC::RTC_OK;
    }
    setTimestamp(m_pc);

    rs2::frameset aligned_frameset;
    if (m_palign == nullptr) {
      aligned_frameset = frameset;
    } else {
      aligned_frameset = m_palign->process(frameset);
    }

    rs2::depth_frame depthFrame = aligned_frameset.get_depth_frame();
    rs2::video_frame colorFrame = aligned_frameset.get_color_frame();
    rs2::pointcloud pc;
    pc.map_to(colorFrame);
    rs2::points points = pc.calculate(depthFrame);

    rs2::video_stream_profile sp = points.get_profile().as<rs2::video_stream_profile>();
    m_pc.width = static_cast<uint32_t>(sp.width());
    m_pc.height = static_cast<uint32_t>(sp.height());
    m_pc.row_step = m_pc.point_step*m_pc.width;
    m_pc.data.length(m_pc.height*m_pc.row_step);

    const rs2::texture_coordinate *textureCoordinates = points.get_texture_coordinates();
    const rs2::vertex* vertices = points.get_vertices();
    int width = colorFrame.get_width();  // Frame width in pixels
    int height = colorFrame.get_height(); // Frame height in pixels
    int bytesPerPixel = colorFrame.get_bytes_per_pixel();   // Get # of bytes per pixel
    int stridesInBytes = colorFrame.get_stride_in_bytes(); // Get line width in bytes
    const uint8_t* texture = reinterpret_cast<const uint8_t*>(colorFrame.get_data());

    float *dst_cloud = (float *)m_pc.data.get_buffer();

    for (int i = 0; i < points.size(); i++) {
      //XYZ
      //座標変換の前にy軸とz軸を入れ替え．
      Vector3f tmp(vertices[i].x, -vertices[i].y, -vertices[i].z);
      //座標変換
      if (m_coordinateTransformation) {
        tmp = m_transform*tmp;
      }

      //RGB
      rs2::texture_coordinate uv = textureCoordinates[i];
      int u = min(max(int(uv.u * width + .5f), 0), width - 1);
      int v = min(max(int(uv.v * height + .5f), 0), height - 1);
      int index = u*bytesPerPixel + v*stridesInBytes;
      uint32_t ui = (texture[index + 2] << 16) | (texture[index + 1] << 8) | texture[index];
      float rgb = *reinterpret_cast<float *>(&ui);

      dst_cloud[0] = tmp(0);
      dst_cloud[1] = tmp(1);
      dst_cloud[2] = tmp(2);
      dst_cloud[3] = rgb;
      dst_cloud += 4;
    }

    m_pcOut.write();

    m_fpsCounter++;
    m_steadyEnd = chrono::steady_clock::now();
    float timeSec = std::chrono::duration<double>(m_steadyEnd - m_steadyStart).count();
    if (timeSec >= 1) {
      RTC_INFO(("%f fps", m_fpsCounter / timeSec));
      m_steadyStart = m_steadyEnd;
      m_fpsCounter = 0;
    }

  } catch (const rs2::error & e) {
    RTC_ERROR((("RealSense error calling " + e.get_failed_function() + "(" + e.get_failed_args() + "):").c_str()));
    RTC_ERROR(("    %s", e.what()));
    return RTC::RTC_ERROR;
  } catch (const std::exception& e) {
    RTC_ERROR((e.what()));
    return RTC::RTC_ERROR;
  }

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


