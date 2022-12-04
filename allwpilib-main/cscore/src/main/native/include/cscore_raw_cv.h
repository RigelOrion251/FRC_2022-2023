// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CSCORE_RAW_CV_H_
#define CSCORE_CSCORE_RAW_CV_H_

#ifdef CSCORE_CSCORE_CV_H_
#error "Cannot include both cscore_cv.h and cscore_raw_cv.h in the same file"
#endif

#include <opencv2/core/mat.hpp>

#include "cscore_raw.h"

namespace cs {
/**
 * A source for using the raw frame API to provide opencv images.
 *
 * If you are using the WPILib OpenCV builds, do not use this, and
 * instead include "cscore_cv.h" to get a more performant version.
 *
 * This is not dependent on any opencv binary ABI, and can be used
 * with versions of OpenCV that are not 3. If using OpenCV 3, use
 * CvSource.
 */
class RawCvSource : public RawSource {
 public:
  RawCvSource() = default;

  /**
   * Create a Raw OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  RawCvSource(std::string_view name, const VideoMode& mode);

  /**
   * Create a Raw OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  RawCvSource(std::string_view name, VideoMode::PixelFormat pixelFormat,
              int width, int height, int fps);

  /**
   * Put an OpenCV image and notify sinks.
   *
   * <p>Only 8-bit single-channel or 3-channel (with BGR channel order) images
   * are supported. If the format, depth or channel order is different, use
   * cv::Mat::convertTo() and/or cv::cvtColor() to convert it first.
   *
   * @param image OpenCV image
   */
  void PutFrame(cv::Mat& image);

 private:
  RawFrame rawFrame;
};

/**
 * A sink for user code to accept raw video frames as OpenCV images.
 *
 * If you are using the WPILib OpenCV builds, do not use this, and
 * instead include "cscore_cv.h" to get a more performant version.
 *
 * This is not dependent on any opencv binary ABI, and can be used
 * with versions of OpenCV that are not 3. If using OpenCV 3, use
 * CvSink.
 */
class RawCvSink : public RawSink {
 public:
  RawCvSink() = default;

  /**
   * Create a sink for accepting OpenCV images.
   *
   * <p>WaitForFrame() must be called on the created sink to get each new
   * image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  explicit RawCvSink(std::string_view name);

  /**
   * Create a sink for accepting OpenCV images in a separate thread.
   *
   * <p>A thread will be created that calls WaitForFrame() and calls the
   * processFrame() callback each time a new frame arrives.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param processFrame Frame processing function; will be called with a
   *        time=0 if an error occurred.  processFrame should call GetImage()
   *        or GetError() as needed, but should not call (except in very
   *        unusual circumstances) WaitForImage().
   */
  RawCvSink(std::string_view name,
            std::function<void(uint64_t time)> processFrame);

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]] uint64_t GrabFrame(cv::Mat& image, double timeout = 0.225);

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]] uint64_t GrabFrameNoTimeout(cv::Mat& image);

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]] uint64_t GrabFrameDirect(cv::Mat& image,
                                         double timeout = 0.225);

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]] uint64_t GrabFrameNoTimeoutDirect(cv::Mat& image);

 private:
  RawFrame rawFrame;
};

inline RawCvSource::RawCvSource(std::string_view name, const VideoMode& mode)
    : RawSource{name, mode} {}

inline RawCvSource::RawCvSource(std::string_view name,
                                VideoMode::PixelFormat format, int width,
                                int height, int fps)
    : RawSource{name, format, width, height, fps} {}

inline void RawCvSource::PutFrame(cv::Mat& image) {
  m_status = 0;
  rawFrame.data = reinterpret_cast<char*>(image.data);
  rawFrame.width = image.cols;
  rawFrame.height = image.rows;
  rawFrame.totalData = image.total() * image.channels();
  rawFrame.pixelFormat = image.channels() == 3 ? CS_PIXFMT_BGR : CS_PIXFMT_GRAY;
  PutSourceFrame(m_handle, rawFrame, &m_status);
}

inline RawCvSink::RawCvSink(std::string_view name) : RawSink{name} {}

inline RawCvSink::RawCvSink(std::string_view name,
                            std::function<void(uint64_t time)> processFrame)
    : RawSink{name, processFrame} {}

inline uint64_t RawCvSink::GrabFrame(cv::Mat& image, double timeout) {
  cv::Mat tmpnam;
  auto retVal = GrabFrameDirect(tmpnam);
  if (retVal <= 0) {
    return retVal;
  }
  tmpnam.copyTo(image);
  return retVal;
}

inline uint64_t RawCvSink::GrabFrameNoTimeout(cv::Mat& image) {
  cv::Mat tmpnam;
  auto retVal = GrabFrameNoTimeoutDirect(tmpnam);
  if (retVal <= 0) {
    return retVal;
  }
  tmpnam.copyTo(image);
  return retVal;
}

inline uint64_t RawCvSink::GrabFrameDirect(cv::Mat& image, double timeout) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.pixelFormat = CS_PixelFormat::CS_PIXFMT_BGR;
  m_status = RawSink::GrabFrame(rawFrame, timeout);
  if (m_status <= 0) {
    return m_status;
  }
  image = cv::Mat{rawFrame.height, rawFrame.width, CV_8UC3, rawFrame.data};
  return m_status;
}

inline uint64_t RawCvSink::GrabFrameNoTimeoutDirect(cv::Mat& image) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.pixelFormat = CS_PixelFormat::CS_PIXFMT_BGR;
  m_status = RawSink::GrabFrameNoTimeout(rawFrame);
  if (m_status <= 0) {
    return m_status;
  }
  image = cv::Mat{rawFrame.height, rawFrame.width, CV_8UC3, rawFrame.data};
  return m_status;
}

}  // namespace cs

#endif  // CSCORE_CSCORE_RAW_CV_H_
