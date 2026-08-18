#include "course_probe.h"
#include "learner_known_case.h"

#include <iostream>

namespace {
course::Model wrongFactory(std::uint32_t seed, const course::Viewport& viewport) {
    auto model = course::makeModel(seed, viewport);
    model.position.x += 1.0f;
    return model;
}
}  // namespace

int main() {
    // The public contract contains no solution oracle. It proves the learner's
    // test accepts the real model and rejects a controlled wrong implementation.
    const auto real = course::interlude::learnerKnownCaseTest(&course::makeModel);
    if (!real.passed) {
        std::cerr << "first-cpp-test: real model failed: " << real.message << '\n';
        return 1;
    }

    const auto mutation = course::interlude::learnerKnownCaseTest(&wrongFactory);
    if (mutation.passed) {
        std::cerr << "first-cpp-test: mutation was accepted: " << mutation.message << '\n';
        return 1;
    }
    if (mutation.message.empty()) {
        std::cerr << "first-cpp-test: mutation was rejected without a diagnostic\n";
        return 1;
    }

    std::cout << "first-cpp-test: learner known case passed; mutation diagnostic: "
              << mutation.message << '\n';
    return 0;
}
