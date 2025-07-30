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
thread_local const val ImageData = val::global("ImageData");


val process_Qrcode1(const std::string &data, int width, int height) {
    std::span span(
        reinterpret_cast<const uint8_t *>(data.data()), data.size());

    const cv::Mat rgba(height, width, CV_8UC4, const_cast<uint8_t *>(span.data()));
    cv::Mat gray;
    cv::cvtColor(rgba, gray, cv::COLOR_RGBA2GRAY);
    auto image = ZXing::ImageView(gray.data, width, height, ZXing::ImageFormat::Lum);
    auto options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::QRCode);
    auto barcodes = ZXing::ReadBarcodes(image, options);

    val entry = val::array();
    for (const auto &b: barcodes) {
        entry.call<void>("push", val(ZXing::ToString(b.format())), val(b.text()));
    }

    cv::rectangle(gray, cv::Point(10, 10), cv::Point(100, 100), cv::Scalar(255, 0, 0, 255), 3);
    cv::Mat final_rgba_image;
    cv::cvtColor(gray, final_rgba_image, cv::COLOR_GRAY2RGBA);
    const uint8_t *image_pointer = final_rgba_image.data;
    const size_t total_bytes = final_rgba_image.total() * final_rgba_image.elemSize();
    val image_data_array = ImageData.new_(Uint8ClampedArray.new_(
                                              typed_memory_view(total_bytes, image_pointer)
                                          ), width, height);
    val result_object = val::object();
    result_object.set("qr_codes", entry);
    result_object.set("processed_image", image_data_array);

    return result_object;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("process_Qrcode", &process_Qrcode1);
}
