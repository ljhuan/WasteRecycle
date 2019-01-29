
#include "compare.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <regex>
#include <sstream>
#include <windows.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <ostream>
#include <iterator>
#include <thread>
#include <Windows.h>
#include <io.h>
#include <sstream>
#include <fstream>
#include "usb_device\device_enumerator.h"
#include "baidu_face_api.h"
#include "image_base64.h"
#include "cv_help.h"
#include "json\json.h"
#include "image_buf.h"

/**
* 人脸比对类，包括1:1人脸比对，1：N人脸比对等
*/
Compare::Compare()
{
}
Compare::~Compare()
{
}

//1:1 人脸对比接口
void Compare::match(BaiduFaceApi *api)
{
    //img_type: 为1 代表base64,2为文件路径，3为facetoken
    // 通过图片文件路径对比
    // std::string token1 = "ffb0b61067361151e1a2db169a8b663f";
    // std::string token2 = "5ba0ac71355fcf25cd07d7ac71822145";
    // std::string res = api->match(token1.c_str(), 3, token2.c_str(), 3);
    std::string res1 = api->match("d:\\2.jpg", 2, "d:\\8.jpg", 2);
  //  std::string res1 = api->match("d:\\linux1.jpg", 2, "d:\\linux2.jpg", 2);
    std::cout << "---res1 is:" << res1 << std::endl;
    // base64和文件路径对比
     std::string base64 = ImageBase64::file2base64("d:\\2.jpg");
    // std::string res = api->match("d:\\23.jpg", 2, base64.c_str(), 1);
     std::string res = api->match(base64.c_str(), 1,"d:\\8.jpg", 2);
    // facetoken和文件对比
    // std::string res = api->match("d:\\23.jpg", 2, "d1c08882a146e73db5eed7f9f226945b", 3);
    // 传入二进制图片buffer
   /* std::string out_buf1;
    int buf_len1 = ImageBuf::get_buf("d:\\1.jpg", out_buf1);

    std::string out_buf2;
    int buf_len2 = ImageBuf::get_buf("d:\\2.jpg", out_buf2);
    std::string res = api->match_by_buf((unsigned char*)out_buf1.c_str(), buf_len1,
        (unsigned char*)out_buf2.c_str(), buf_len2); */

    std::cout << "---res is:" << res << std::endl;
}
// 1:N 人脸对比接口
void Compare::identify(BaiduFaceApi *api)
{
    //img_type: 为1 代表base64,2为文件路径，3为facetoken
    // 以下为传入base64格式,
    //std::string base64 = ImageBase64::file2base64("d:\\15717.jpg");
    //std::string res = api->identify(base64.c_str(), 1, "test_group2", "", 100);
    // 以下为传入文件路径
    //   std::string res = api->identify("d:\\15717.jpg", 2, "test_group2", "", 100);
    // std::string res = api->identify("d:\\15717.jpg", 2, "test_group2", "");

    // std::string res = api->identify("d:\\2.jpg", 2, "test_group", "test_user1");
     std::string res = api->identify("C:\\Users\\ljhuan\\Desktop\\tmp\\200.jpg", 2);
     std::cout << "---200 identify res is:" << res << std::endl;

	 res = api->identify("C:\\Users\\ljhuan\\Desktop\\tmp\\201.jpg", 2);
	 std::cout << "---201 identify res is:" << res << std::endl;

	 res = api->identify("C:\\Users\\ljhuan\\Desktop\\tmp\\202.jpg", 2);
	 std::cout << "---202 identify res is:" << res << std::endl;

	 res = api->identify("C:\\Users\\ljhuan\\Desktop\\tmp\\203.jpeg", 2);
	 std::cout << "---203 identify res is:" << res << std::endl;

	 res = api->identify("C:\\Users\\ljhuan\\Desktop\\tmp\\204.jpg", 2);
	 std::cout << "---204 identify res is:" << res << std::endl;
    // std::string res = api->identify("d:\\23.jpg", 2, "test_group2", "15717_face_image.jpg", 1);
    // 以下为传入facetoken
    //   std::string res = api->identify("0dab3f12a5b95bdcd37496f302a5527c", 3, "test_group2", "", 100);
    // 以下为传入opencv的视频帧
   // cv::Mat mat = cv::imread("d:\\1.jpg");
  //  std::string res = api->identify(mat, "test_group", "test_user1");

    // 传入二进制图片buffer
  //  std::string out_buf;
  //  int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
 //   std::string res1 = api->identify_by_buf((unsigned char*)out_buf.c_str(), buf_len, "test_group", "test_user1");
   // std::string res_all = api->identify_by_buf((unsigned char*)out_buf.c_str(), buf_len);
  //  std::cout << "---identify res is:" << res_all << std::endl;
  //  std::cout << "---identify by buf res is:" << res1 << std::endl;
}
// 传入人脸特征值来进行1:N识别
void Compare::identify_by_feature(BaiduFaceApi* api)
{
    // 获取feature
    const float* feature = nullptr;
    // 传入二进制图片buffer
    std::string out_buf;
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);

    int count = api->get_face_feature_by_buf((unsigned char*)out_buf.c_str(), buf_len, feature);

    using Feature = std::vector<float>;
    Feature v_feature1;
    v_feature1.assign(feature, feature + count);
    std::string res = api->identify_by_feature(v_feature1, "test_group", "test_user1");
    std::cout << "identify feature res is:" << res << std::endl;


}
// 1:1 视频帧和人脸图片比较(可能会稍有卡顿，因为1秒钟很多视频帧，每帧都比较很耗时)
// 开发者可根据需求适当修改，适配出最适合自己的，若要实时检测视频帧，可参考liveness的usb_track_info
// liveness的usb_track_info流畅不卡顿
void Compare::match_by_img_and_frame(BaiduFaceApi *api, const std::string& path)
{
    if (path.length() <= 0)
    {
        std::cout << "please check jpg path" << std::endl;
        return;
    }
    cv::Mat img_1 = cv::imread(path);
    if (img_1.empty())
    {
        std::cout << "please check jpg path" << std::endl;
        return;
    }
    // device为0默认打开笔记本电脑自带摄像头，若为0打不开外置usb摄像头
    // 请把device修改为1或2再重试，1，或2为usb插上电脑后，电脑认可的usb设备id
    // 也可通过select_usb_device_id方法获取id
    int device = select_usb_device_id();
    cv::VideoCapture cap(device);
    if (!cap.isOpened())
    {
        std::cout << "open camera error" << std::endl;
        return;
    }
    cv::Mat frame;
    bool stop = false;
    std::vector<cv::Mat> manyimg;
    while (!stop)
    {
        cap >> frame;
        cv::Mat dst;
        manyimg.clear();
        manyimg.push_back(img_1);
        manyimg.push_back(frame);
        many_images(manyimg, dst, 2);

        std::string res = api->match_by_img_and_frame(path.data(), 2, frame);
        float score = parse_score(res);
        std::string msg1 = "compare score is:" + std::to_string(score);
        cv::putText(dst, msg1, cv::Point(50, 50), CV_FONT_HERSHEY_COMPLEX,
            1, cv::Scalar(255, 100, 0), 2, 2);
        std::cout << "res is:" << res << std::endl;

        imshow("compare", dst);
        cv::waitKey(1);
    }
}

// 选择usb视频摄像头id,方法里面有获取连接的摄像头列表，包括id，名称和路径等
int Compare::select_usb_device_id()
{
    std::map<int, Device> devs = DeviceEnumerator::get_usb_devices();
    int device_id = 0;
    for (auto const &device : devs) {
        std::cout << "== VIDEO DEVICE (id:" << device.first << ") ==" << std::endl;
        std::cout << "Name: " << device.second.deviceName << std::endl;
        std::cout << "Path: " << device.second.devicePath << std::endl;
    }
    if (devs.size() > 0)
    {
        device_id = devs[0].id;
        std::cout << "select device id is:" << device_id << std::endl;
    }
    return device_id;
}
// opencv绘多图
void Compare::many_images(std::vector<cv::Mat> images, cv::Mat& dst, int img_rows)
{
    int width = 400;
    int num = images.size();//得到Vector容器中图片个数
                            //设定包含这些图片的窗口大小，这里都是BGR3通道，如果做灰度单通道，稍微改一下下面这行代码就可以
    cv::Mat window(width * ((num - 1) / img_rows + 1), width * img_rows, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat std_image;//存放标准大小的图片
    cv::Mat image_roi;//图片放置区域
    cv::Size std_size = cv::Size(width, width);//每个图片显示大小400*400
    int x_begin = 0;
    int y_begin = 0;
    for (int i = 0; i < num; i++)
    {
        x_begin = (i % img_rows)*std_size.width;//每张图片起始坐标
        y_begin = (i / img_rows)*std_size.height;
        resize(images[i], std_image, std_size, 0, 0, cv::INTER_LINEAR);//将图像设为标准大小
                                                                   //将其贴在Window上
        image_roi = window(cv::Rect(x_begin, y_begin, std_size.width, std_size.height));
        std_image.copyTo(image_roi);
    }
    dst = window;
}
// 解析分数
float Compare::parse_score(const std::string& res)
{
    float score = 0;
    if (res.length() > 0)
    {
        // 若出现json::reader编译不过，请用下面代码
        Json::Value root;
        Json::Reader reader;
        if (reader.parse(res, root))
        {
            int err = root["errno"].asInt();
            if (err == 0)
            {
                Json::Value result = root["data"];
                if (result.size() > 0)
                {
                    std::string sss = result["score"].asString();
                    score = atof(sss.c_str());
                }
            }
            else
            {
                score = 0;
            }
        }

        // 若json编译不过，请用下面代码（json库兼容性问题)
     /* Json::CharReaderBuilder b;
        Json::CharReader* reader(b.newCharReader());
        const char* str = res.c_str();

        JSONCPP_STRING errs;
        bool ok = reader->parse(str, str + std::strlen(str), &root, &errs);
        if (ok&&errs.size() == 0)
        {
            int err = root["errno"].asInt();
            if (err == 0)
            {
                Json::Value result = root["data"];
                if (result.size() > 0)
                {
                    std::string sss = result["score"].asString();
                    score = atof(sss.c_str());
                }
            }
            else
            {
                score = 0;
            }
        } */
    }
    return score;
}

// 提取人脸特征值(一般为512个浮点值已加密)(通过图片)
void Compare::get_face_feature_by_img(BaiduFaceApi *api)
{
    const float* feature = nullptr;
    int count = api->get_face_feature("d:\\1.jpg", 2, feature);
    // 返回值为512表示取到了特征值
    if (count != 512)
    {
        return ;
    }
    for (int i = 0; i < count; i++)
    {
        std::cout << "feature" << i << "is:" << feature[i] << std::endl;
    }
    std::cout << "feature count is:" << count << std::endl;
}
// 提取人脸特征值(一般为512个浮点值已加密)(通过opencv视频帧)
void Compare::get_face_feature_by_frame(BaiduFaceApi *api)
{
    const float* feature = nullptr;
    cv::Mat mat = cv::imread("d:\\1.jpg");
    int count = api->get_face_feature(mat, feature);
    // 返回值为512表示取到了特征值
    if (count != 512)
    {
        return;
    }
    for (int i = 0; i < count; i++)
    {
        std::cout << "feature" << i << "is:" << feature[i] << std::endl;
    }
    std::cout << "feature count is:" << count << std::endl;
}

// 提取人脸特征值(一般为512个浮点值已加密)(传入二进制图片buffer)
void Compare::get_face_feature_by_buf(BaiduFaceApi *api)
{
    const float* feature = nullptr;
    // 传入二进制图片buffer
    std::string out_buf;
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);

    int count = api->get_face_feature_by_buf((unsigned char*)out_buf.c_str(),buf_len,feature);
    // 返回值为512表示取到了特征值
    if (count != 512)
    {
        return;
    }
    for (int i = 0; i < count; i++)
    {
        std::cout << "feature" << i << "is:" << feature[i] << std::endl;
    }
    std::cout << "feature count is:" << count << std::endl;
}

// 测试获取特征值
void Compare::get_face_feature_and_match(BaiduFaceApi *api)
{
    std::string de_str;
    int len = 0;
    FILE *fp = fopen("d:\\1.jpg", "rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        de_str.resize(len);
        fread((void*)de_str.data(), 1, len, fp);
        fclose(fp);
    }
    else
    {
        std::cout << "image not exist" << std::endl;
    }

    const float* feature = nullptr;
    int lens = de_str.length();
    std::cout << "lens is:" << lens << std::endl;

    int count = api->get_face_feature_by_buf((unsigned char*)de_str.c_str(), len, feature);
    // 返回值为512表示取到了特征值
    if (count != 512)
    {
        return;
    }
    // 获取到返回的feature为512个浮点值（已加密）
    /* for (int i = 0; i < count; i++)
    {
    std::cout << "feature" << i << "is:" << feature[i] << std::endl;
    } */
    std::cout << "feature count is:" << count << std::endl;

    using Feature = std::vector<float>;
    Feature v_feature1;
    v_feature1.assign(feature, feature + count);
    // 图片和特征值的比较
    std::string res = api->match_by_feature(v_feature1, "d:\\2.jpg", 2);

    std::cout << "res is:" << res << std::endl;
    // 图片和图片的比较
    std::string res2 = api->match("d:\\1.jpg", 2, "d:\\2.jpg", 2);
    std::cout << "res2 is:" << res << std::endl;
    // 特征值和图片的二进制buffer比较
    std::string res3 = api->match_by_feature(v_feature1, (unsigned char*)de_str.c_str(), len);
    std::cout << "res3 is:" << res << std::endl;

    /*  std::vector<unsigned char> vectordata(de_str.begin(), de_str.end());

    Mat data_mat(vectordata, true);
    Mat mat = imdecode(data_mat, CV_LOAD_IMAGE_COLOR); //PGM image
    if (mat.empty())
    {
    std::cout << "exe mat is empty" << std::endl;
    return ;
    }
    imshow("face", mat);
    waitKey(2); */
}

//特征值比对(通过提取图片的特征值，比对2个图片的相似分值--百分制)
void Compare::compare_feature(BaiduFaceApi *api)
{
    using Feature = std::vector<float>;
    Feature v_feature1;
    Feature v_feature2;
    const float* f1 = nullptr;
    const float* f2 = nullptr;

    cv::Mat m1 = cv::imread("d:\\1.jpg");
    if (m1.empty())
    {
        std::cout << "mat1 is empty,please check img path" << std::endl;
    }
    int count1 = api->get_face_feature(m1, f1);
    if (count1 != 512)
    {
        std::cout << "get feature1 error" << std::endl;
        return;
    }

    v_feature1.assign(f1, f1 + count1);

    cv::Mat m2 = cv::imread("d:\\2.jpg");
    if (m2.empty())
    {
        std::cout << "mat2 is empty,please check img path" << std::endl;
    }
    int count2 = api->get_face_feature(m2, f2);
    if (count2!=512)
    {
        std::cout << "get feature2 error" << std::endl;
        return;
    }
    v_feature2.assign(f2, f2 + count2);

    float score = api->compare_feature(v_feature1, v_feature2);
    std::cout << "compare score is:" << score << std::endl;
}

