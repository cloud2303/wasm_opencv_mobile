#include <emscripten.h>
#include <emscripten/bind.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace emscripten;


extern "C" {
EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}


EMSCRIPTEN_KEEPALIVE
void process_data(uint8_t *data, int len, int width, int height) {
    cv::Mat rgba(height, width, CV_8UC4, data);
    if (rgba.empty()) {
        return;
    }
    // 转灰度图
    cv::Mat gray;
    cv::cvtColor(rgba, gray, cv::COLOR_RGBA2GRAY);

    // 再转回 RGBA
    cv::cvtColor(gray, rgba, cv::COLOR_GRAY2RGBA);
}
}


EMSCRIPTEN_BINDINGS(myModule) {
    function("add", &add);
}
