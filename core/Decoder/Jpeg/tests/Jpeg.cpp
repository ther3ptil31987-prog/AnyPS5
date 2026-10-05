#include "Decoder/Jpeg.hpp"

#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <vector>

static void Require(bool value) { if (!value) std::abort(); }

template<typename TFunction>
static bool ThrowsInvalidArgument(TFunction function) {
    try {
        function();
    } catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

static bool IsJpeg(const std::vector<std::uint8_t>& data) {
    return data.size() > 4 && data[0] == 0xFF && data[1] == 0xD8 && data[data.size() - 2] == 0xFF && data.back() == 0xD9;
}

static int Difference(std::uint8_t left, std::uint8_t right) {
    return left > right ? left - right : right - left;
}

int main() {
    constexpr std::uint32_t width = 32;
    constexpr std::uint32_t height = 24;

    std::vector<std::uint8_t> rgb(width * height * 3);
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) {
            std::uint8_t* pixel = &rgb[(y * width + x) * 3];
            pixel[0] = static_cast<std::uint8_t>(x * 8);
            pixel[1] = static_cast<std::uint8_t>(y * 10);
            pixel[2] = 128;
        }
    }

    const std::vector<std::uint8_t> rgbJpeg = Decoder::Jpeg::Encode(rgb, width, height, 3, 90);
    Require(IsJpeg(rgbJpeg));
    const auto rgbImage = Decoder::Jpeg::Decode(rgbJpeg);
    Require(rgbImage.has_value());
    Require(rgbImage->width == width && rgbImage->height == height && rgbImage->channels == 3);
    Require(rgbImage->pixels.size() == rgb.size());
    long totalError = 0;
    for (std::size_t i = 0; i < rgb.size(); ++i) totalError += Difference(rgb[i], rgbImage->pixels[i]);
    Require(totalError / static_cast<long>(rgb.size()) < 4);

    std::vector<std::uint8_t> gray(width * height);
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) gray[y * width + x] = static_cast<std::uint8_t>((x + y) * 4);
    }
    const std::vector<std::uint8_t> grayJpeg = Decoder::Jpeg::Encode(gray, width, height, 1, 90);
    Require(IsJpeg(grayJpeg));
    const auto grayImage = Decoder::Jpeg::Decode(grayJpeg);
    Require(grayImage.has_value());
    Require(grayImage->width == width && grayImage->height == height);
    totalError = 0;
    for (std::size_t i = 0; i < gray.size(); ++i) totalError += Difference(gray[i], grayImage->pixels[i * grayImage->channels]);
    Require(totalError / static_cast<long>(gray.size()) < 4);

    Require(ThrowsInvalidArgument([&] { Decoder::Jpeg::Encode(rgb, width, height, 4, 90); }));
    Require(ThrowsInvalidArgument([&] { Decoder::Jpeg::Encode(rgb, width, height, 3, 0); }));
    Require(ThrowsInvalidArgument([&] { Decoder::Jpeg::Encode(rgb, width, height, 3, 101); }));
    Require(ThrowsInvalidArgument([&] { Decoder::Jpeg::Encode(rgb, 0, height, 3, 90); }));
    Require(ThrowsInvalidArgument([&] { Decoder::Jpeg::Encode(rgb, 0x10000, 1, 3, 90); }));
    Require(ThrowsInvalidArgument([&] { Decoder::Jpeg::Encode(rgb, width, height + 1, 3, 90); }));

    Require(!Decoder::Jpeg::Decode({}).has_value());
    const std::vector<std::uint8_t> garbage{1, 2, 3, 4, 5, 6, 7, 8};
    Require(!Decoder::Jpeg::Decode(garbage).has_value());

    return 0;
}
