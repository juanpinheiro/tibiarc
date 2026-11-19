/*
 * Copyright 2026 "Juan Pinheiro"
 *
 * This file is part of tibiarc.
 *
 * tibiarc is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __TRC_TESTS_UNIT_CHECK_HPP__
#define __TRC_TESTS_UNIT_CHECK_HPP__

#include <functional>
#include <iostream>
#include <string>
#include <vector>

/* A test framework in a page: tests register themselves and report failed
 * expectations with their location. */
namespace trc {
namespace Test {

struct Case {
    std::string Name;
    std::function<void()> Body;
};

inline std::vector<Case> &Registry() {
    static std::vector<Case> cases;
    return cases;
}

inline int &Failures() {
    static int count = 0;
    return count;
}

struct Registration {
    Registration(std::string name, std::function<void()> body) {
        Registry().push_back(Case{std::move(name), std::move(body)});
    }
};

template <typename A, typename B>
void ExpectEqual(const A &actual,
                 const B &expected,
                 const char *expression,
                 const char *file,
                 int line) {
    if (!(actual == expected)) {
        std::cerr << file << ":" << line << ": " << expression << " is "
                  << actual << ", expected " << expected << std::endl;
        Failures()++;
    }
}

} // namespace Test
} // namespace trc

#define TEST(name)                                                             \
    static void name();                                                        \
    static const trc::Test::Registration name##_registration(#name, name);     \
    static void name()

#define EXPECT_EQ(actual, expected)                                            \
    trc::Test::ExpectEqual((actual), (expected), #actual, __FILE__, __LINE__)

#endif
