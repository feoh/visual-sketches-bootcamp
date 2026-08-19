#pragma once

#if __cplusplus < 201703L
#error "visual-sketches-bootcamp requires C++17 or newer"
#endif

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace course::test {

struct NearResult {
    bool passed = false;
    std::string message;
};

inline NearResult expectNear(double actual, double expected, double absoluteTolerance,
                             double relativeTolerance) {
    const double difference = std::abs(actual - expected);
    const double allowed = std::max(absoluteTolerance,
                                    relativeTolerance * std::max(std::abs(actual), std::abs(expected)));
    const bool finite = std::isfinite(actual) && std::isfinite(expected) &&
                        std::isfinite(absoluteTolerance) && std::isfinite(relativeTolerance);
    NearResult result;
    result.passed = finite && absoluteTolerance >= 0.0 && relativeTolerance >= 0.0 &&
                    difference <= allowed;
    std::ostringstream detail;
    detail << std::setprecision(17) << "actual=" << actual << ", expected=" << expected
           << ", difference=" << difference << ", allowed=" << allowed
           << " (absTol=" << absoluteTolerance << ", relTol=" << relativeTolerance << ')';
    if (!finite) {
        detail << "; all values and tolerances must be finite";
    }
    result.message = detail.str();
    return result;
}

}  // namespace course::test
