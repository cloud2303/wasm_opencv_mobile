#include <iostream>
#include <emscripten/bind.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ZXing/ReadBarcode.h"
#include <print>
#include <span>


using namespace emscripten;


thread_local const val Uint8ClampedArray = val::global("Uint8ClampedArray");


val process_Qrcode(const std::string& encodedData) {
    std::span inputSpan(
        reinterpret_cast<const uint8_t*>(encodedData.data()),
        encodedData.size()
    );

    const cv::Mat encodedMat(1, static_cast<int>(inputSpan.size()), CV_8UC1, const_cast<uint8_t*>(inputSpan.data()));
    const cv::Mat colorImg = cv::imdecode(encodedMat, cv::IMREAD_COLOR);

    cv::Mat grayImg;
    cv::cvtColor(colorImg, grayImg, cv::COLOR_BGR2GRAY);

    const ZXing::ImageView imageView(
        grayImg.data, colorImg.cols, colorImg.rows, ZXing::ImageFormat::Lum
    );

    const auto decodeOptions = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::QRCode);
    const auto results = ZXing::ReadBarcodes(imageView, decodeOptions);

    const val qrArray = val::array();
    for (const auto& result : results) {
        qrArray.call<void>("push", val(result.text()));
    }

    val output = val::object();
    output.set("qr_codes", qrArray);

    return output;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("process_Qrcode", &process_Qrcode);
}
