#include "image_geometry_design.h"
#include "image_geometry_model.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
int failures = 0;
void expect(bool condition, const std::string& message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
bool near(float a, float b, float tolerance = 0.0001f) {
    return std::isfinite(a) && std::isfinite(b) && std::fabs(a - b) <= tolerance;
}
image_geometry::Pixels gray(std::size_t width, std::size_t height,
                            std::vector<std::uint8_t> bytes) {
    return {width, height, 1, std::move(bytes)};
}

void oracleCases(const char* path) {
    std::ifstream input(path);
    expect(input.good(), "independent mask oracle opens");
    std::string line;
    int rows = 0;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        ++rows;
        std::istringstream stream(line);
        std::string name;
        std::size_t width=0, height=0, channels=0, step=0, expected=0;
        int threshold=0, dark=0;
        float min_x=0, min_y=0, max_x=0, max_y=0, center_x=0, center_y=0;
        const bool complete=static_cast<bool>(stream >> name >> width >> height >> channels >> step >> threshold >> dark >> expected >> min_x >> min_y >> max_x >> max_y >> center_x >> center_y);
        expect(complete, name + " oracle cardinality");
        if (!complete) continue;
        std::vector<std::uint8_t> values(width * height, dark ? 255 : 0);
        if (name == "cross") values = {0,255,0, 255,0,255, 0,255,0};
        else if (name == "corners") values = {0,255,0,255, 255,255,255,255, 0,255,0,255};
        else if (name == "bright-center") values = {0,0,0, 0,255,0, 0,0,0};
        image_geometry::Geometry result;
        const auto status = image_geometry::sampleMask({width,height,channels,values}, {step,static_cast<std::uint8_t>(threshold),dark != 0}, result);
        expect(status == image_geometry::Status::ok, name + " samples");
        expect(result.points.size() == expected, name + " count");
        expect(near(result.bounds.minimum.x,min_x) && near(result.bounds.minimum.y,min_y) && near(result.bounds.maximum.x,max_x) && near(result.bounds.maximum.y,max_y), name + " bounds");
        expect(near(result.centroid.x,center_x) && near(result.centroid.y,center_y), name + " centroid");
    }
    expect(rows == 3, "oracle has three known cases");
}

void indexingAndLuminanceCases() {
    std::size_t index = 99;
    expect(image_geometry::checkedPixelIndex(4,3,3,2,1,1,index) && index == 19, "row-major interleaved index");
    expect(image_geometry::checkedPixelIndex(4,3,3,3,2,2,index) && index == 35, "last byte is addressable");
    expect(!image_geometry::checkedPixelIndex(4,3,3,4,0,0,index), "x equal width rejected");
    expect(!image_geometry::checkedPixelIndex(4,3,3,0,3,0,index), "y equal height rejected");
    expect(!image_geometry::checkedPixelIndex(4,3,3,0,0,3,index), "channel equal count rejected");
    expect(!image_geometry::checkedPixelIndex(std::numeric_limits<std::size_t>::max(),2,4,0,0,0,index), "overflow dimensions rejected");
    image_geometry::Pixels rgb{2,1,3,{255,0,0, 0,255,0}};
    std::uint8_t value = 0;
    expect(image_geometry::luminanceAt(rgb,0,0,value) && value == 76, "red luminance is deterministic Rec. 601");
    expect(image_geometry::luminanceAt(rgb,1,0,value) && value == 150, "green luminance rounds deterministically");
    image_geometry::Pixels rgba{1,1,4,{10,20,30,0}};
    expect(image_geometry::luminanceAt(rgba,0,0,value) && value == 18, "alpha does not alter RGB mask luminance");
}

void samplingBoundsAndFailureCases() {
    image_geometry::Geometry output;
    auto cross = gray(3,3,{0,255,0, 255,0,255, 0,255,0});
    expect(image_geometry::sampleMask(cross,{1,127,true},output) == image_geometry::Status::ok, "known mask samples");
    const auto before = output.points;
    expect(image_geometry::sampleMask({}, {1,127,true}, output) == image_geometry::Status::invalid_asset && output.points.size() == before.size() && near(output.points.front().x, before.front().x), "missing asset fails explicitly and transactionally");
    auto truncated = cross; truncated.bytes.pop_back();
    expect(image_geometry::sampleMask(truncated,{1,127,true},output) == image_geometry::Status::invalid_asset, "truncated asset rejected");
    expect(image_geometry::sampleMask(cross,{0,127,true},output) == image_geometry::Status::invalid_design, "zero step rejected");
    expect(image_geometry::sampleMask(cross,{1,0,false},output) == image_geometry::Status::ok && output.points.size() == 9, "inclusive bright threshold boundary");
    expect(image_geometry::sampleMask(gray(2,2,{255,255,255,255}),{1,0,true},output) == image_geometry::Status::no_samples, "empty threshold result explicit");
    image_geometry::Pixels too_large{image_geometry::maximum_source_pixels + 1,1,1,{}};
    expect(image_geometry::sampleMask(too_large,{1,127,true},output) == image_geometry::Status::invalid_asset, "source pixel hard limit enforced before traversal");
    image_geometry::Pixels visit_limit{401,251,1,std::vector<std::uint8_t>(401*251)};
    expect(image_geometry::sampleMask(visit_limit,{1,127,true},output) == image_geometry::Status::work_limit, "sampling work limit enforced");
    expect(image_geometry::sampleMask(visit_limit,{2,127,true},output) == image_geometry::Status::ok, "density can reduce bounded work");
}

float distance(image_geometry::Vec2 a, image_geometry::Vec2 b) { return std::hypot(a.x-b.x,a.y-b.y); }
void transformCases() {
    image_geometry::Geometry source;
    expect(image_geometry::sampleMask(gray(3,3,{0,255,0, 255,0,255, 0,255,0}),{1,127,true},source) == image_geometry::Status::ok, "transform source created");
    image_geometry::Geometry moved;
    constexpr float half_pi = 1.57079632679f;
    expect(image_geometry::transformGeometry(source,{{10,-4},half_pi,2},moved) == image_geometry::Status::ok, "finite transform succeeds");
    expect(moved.points.size() == source.points.size(), "transform preserves sample count");
    expect(near(moved.centroid.x,source.centroid.x+10) && near(moved.centroid.y,source.centroid.y-4), "centroid follows translation around itself");
    for (std::size_t i=1; i<source.points.size(); ++i)
        expect(near(distance(moved.points[0],moved.points[i]),2*distance(source.points[0],source.points[i]),0.001f), "rotation preserves distance and scale multiplies it");
    expect(near(moved.bounds.maximum.x-moved.bounds.minimum.x,4) && near(moved.bounds.maximum.y-moved.bounds.minimum.y,4), "rotated scaled symmetric bounds known case");
    const auto saved = moved.points;
    expect(image_geometry::transformGeometry(source,{{0,0},0,0},moved) == image_geometry::Status::invalid_transform && moved.points.size() == saved.size() && near(moved.points.front().x, saved.front().x), "zero scale rejected transactionally");
    expect(image_geometry::transformGeometry(source,{{0,0},std::numeric_limits<float>::quiet_NaN(),1},moved) == image_geometry::Status::invalid_transform, "NaN rotation rejected");
}

void designSeamCase() {
    const auto design = makeImageGeometryDesign();
    expect(design.sample.step > 0 && design.point_radius > 0 && std::isfinite(design.motion_amplitude), "learner-owned design seam is technically usable");
}
}

int main(int argc, char** argv) {
    expect(argc == 2, "usage supplies mask oracle");
    if (argc == 2) oracleCases(argv[1]);
    indexingAndLuminanceCases();
    samplingBoundsAndFailureCases();
    transformCases();
    designSeamCase();
    if (failures) { std::cerr << "image_geometry_model_test: " << failures << " failure(s)\n"; return 1; }
    std::cout << "image_geometry_model_test: oracle counts/bounds/centroids, checked interleaved indexing, threshold boundaries, RGB luminance, explicit transactional asset failures, source/sample hard limits, sampling density, transform count/centroid/distance/bounds invariants, and learner seam passed\n";
}
