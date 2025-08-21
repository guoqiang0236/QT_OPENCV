#include "OpenCVImageUtil.h"
#include <QImage>
#include <QFileInfo>

OpenCVImageUtil::OpenCVImageUtil(QObject* parent)
    : QObject(parent)
{
}

bool OpenCVImageUtil::openImage(const std::string& filePath)
{
    
    m_mat = cv::imread(filePath, cv::IMREAD_UNCHANGED);
    if (m_mat.empty()) {
        return false;
    }
	m_hasopenimage = true; // 标记图像已打开
    return true;
}

QImage OpenCVImageUtil::getCurrentImage() const
{
    if (m_mat.empty())
        return QImage();

    if (m_mat.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(m_mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (m_mat.channels() == 1) {
        return QImage(m_mat.data, m_mat.cols, m_mat.rows, m_mat.step, QImage::Format_Grayscale8).copy();
    } else if (m_mat.channels() == 4) {
        cv::Mat rgba;
        cv::cvtColor(m_mat, rgba, cv::COLOR_BGRA2RGBA);
        return QImage(rgba.data, rgba.cols, rgba.rows, rgba.step, QImage::Format_RGBA8888).copy();
    }
    return QImage();
}

cv::Mat& OpenCVImageUtil::getCurrentMat()
{
    return m_mat;
}

void OpenCVImageUtil::showImageFromRawData(uchar* pixData8, int rows, int columns, int samplesPerPixel, const std::string& winName)
{
    if (samplesPerPixel == 3) // RGB
    {
        // DICOM的RGB数据通常是连续的R1G1B1R2G2B2...
        cv::Mat imag_rgb(rows, columns, CV_8UC3, pixData8);
        cv::Mat imag_bgr;
        cv::cvtColor(imag_rgb, imag_bgr, cv::COLOR_RGB2BGR);
        cv::imshow(winName, imag_bgr);
    }
    else
    {
        cv::Mat imag(rows, columns, CV_8U, pixData8);
        cv::imshow(winName, imag);
    }
    cv::waitKey(1);
}


// 颜色空间转换实现
bool OpenCVImageUtil::convertColor(int type)
{
    if (m_mat.empty())
        return false;

    // 枚举类型与int的对应
    // 0: Gray, 1: HSV, 2: RGB
    switch (static_cast<ColorConvertType>(type)) {
    case ColorConvertType::Gray:
        if (m_mat.channels() == 3 || m_mat.channels() == 4) {
            cv::Mat gray;
            cv::cvtColor(m_mat, gray, cv::COLOR_BGR2GRAY);
            m_mat = gray;
            return true;
        }
        break;
    case ColorConvertType::HSV:
        if (m_mat.channels() == 3) {
            cv::Mat hsv;
            cv::cvtColor(m_mat, hsv, cv::COLOR_BGR2HSV);
            m_mat = hsv;
            return true;
        }
        break;
    case ColorConvertType::RGB:
        if (m_mat.channels() == 3) {
            cv::Mat rgb;
            cv::cvtColor(m_mat, rgb, cv::COLOR_BGR2RGB);
            m_mat = rgb;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void OpenCVImageUtil::pixel_visit_demo(cv::Mat& image)
{
    int w = image.cols;
    int h = image.rows;
    int dims = image.channels();

    //值
   /* for (int row = 0; row < h; row++){
        for (int col = 0; col < w; col++) {
            if (dims == 1)
            {
                int pv = image.at<uchar>(row, col);
                image.at<uchar>(row, col) = 255 - pv;
            }
            if (dims == 3)
            {
                cv::Vec3b bgr = image.at<cv::Vec3b>(row, col);
                image.at<cv::Vec3b>(row, col) = 255 - bgr[0];
                image.at<cv::Vec3b>(row, col) = 255 - bgr[1];
                image.at<cv::Vec3b>(row, col) = 255 - bgr[2];

            }
        }
    }*/

    //指针速度快
    for (int row = 0; row < h; row++) {
		uchar* current_row = image.ptr<uchar>(row); 
        for (int col = 0; col < w; col++) {
            if (dims == 1)
            {
                int pv = *current_row;
                *current_row++ = 255 - pv;
            }
            if (dims == 3)
            {
      
                *current_row++ = 255 - *current_row;
                *current_row++ = 255 - *current_row;
                *current_row++ = 255 - *current_row;

            }
        }
    }
    std::string windowName = "PixelDemo";
    


    cv::imshow(windowName,image);
}
