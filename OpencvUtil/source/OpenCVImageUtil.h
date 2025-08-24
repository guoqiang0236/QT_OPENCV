#pragma once
#include "OpencvUtil_Export.h"
#include "pch.h"
#include <QObject>
#include <QString>
#include <opencv2/opencv.hpp>

class OPENCVUTIL_API OpenCVImageUtil : public QObject
{
    Q_OBJECT
public:
    enum class ColorConvertType {
        Gray = 0,
        HSV = 1,
        RGB = 2
    };
    explicit OpenCVImageUtil(QObject* parent = nullptr);
    bool openImage(const std::string& filePath);
    QImage getCurrentImage() const;

    cv::Mat& getCurrentMat();
	bool getHasOpenImage() const { return m_hasopenimage; }
    void showImageFromRawData(uchar* pixData8, int rows, int columns, int samplesPerPixel, const std::string& winName);
    // 颜色空间转换接口
    bool convertColor(int type);

    void pixel_visit_demo(cv::Mat &image);

	void operator_demo(cv::Mat& image);

private:
    cv::Mat m_mat;
	bool m_hasopenimage = false; // 是否有打开的图像
};
