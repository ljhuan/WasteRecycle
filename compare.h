#ifndef TESTFACEAPI_TESTFACEAPI_COMPARE_H
#define TESTFACEAPI_TESTFACEAPI_COMPARE_H
#include <string>
#include <vector>
#include "opencv2\opencv.hpp"
class BaiduFaceApi;
// 该类可作为人脸比对类，包括1:1人脸比对，1：N人脸比对等
class Compare
{
public:
    Compare();
    ~Compare();
public:
    //1:1 人脸对比接口
    void match(BaiduFaceApi *api);
    // 1:N 人脸对比接口
    void identify(BaiduFaceApi *api);
    // 传入人脸特征值进行1:N识别
    void identify_by_feature(BaiduFaceApi* api);
    //视频帧和图片1:1对比
    void match_by_img_and_frame(BaiduFaceApi *api, const std::string& path);
    // 提取人脸特征值(通过图片)
    void get_face_feature_by_img(BaiduFaceApi *api);
    // 提取人脸特征值(通过视频帧)
    void get_face_feature_by_frame(BaiduFaceApi *api);
    // 提取人脸特征值(传入二进制图片buffer)
    void get_face_feature_by_buf(BaiduFaceApi *api);
    // 提取特征值及用特征值和图片比较等综合示例
    void get_face_feature_and_match(BaiduFaceApi *api);
    //特征值比对
    void compare_feature(BaiduFaceApi *api);
private:
    void many_images(std::vector<cv::Mat> images, cv::Mat& dst, int img_rows);
    int select_usb_device_id();
    float parse_score(const std::string& res);
};

#endif
