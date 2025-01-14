#include <Interpolation.hpp>
#include "include/Assertion.hpp"
#include "include/rel_err.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

// M_PI is not part of the standard
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    using namespace std::chrono;
    using namespace intp;

    // random sample points

    std::vector<double> coord_1d;
    std::vector<std::array<double, 2>> coord_2d;
    std::vector<std::array<double, 3>> coord_3d;

    std::mt19937 rand_gen(static_cast<unsigned int>(
        high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<> rand_dist(-M_PI, M_PI);
    std::uniform_real_distribution<> rand_dist2(-.5, .5);
    {
        constexpr size_t coord_num = 100;
        coord_1d.reserve(coord_num);
        coord_2d.reserve(coord_num);
        coord_3d.reserve(coord_num);
        for (size_t i = 0; i < coord_num; ++i) {
            coord_1d.push_back(rand_dist(rand_gen));
            coord_2d.push_back({rand_dist(rand_gen), rand_dist(rand_gen)});
            coord_3d.push_back({rand_dist(rand_gen), rand_dist(rand_gen),
                                rand_dist2(rand_gen)});
        }
    }

    Assertion assertion;
    constexpr size_t len = 256;
    constexpr size_t eval_count = 1 << 20;
    const double eps = std::sqrt(std::numeric_limits<double>::epsilon());

    // 1D case
    {
        const auto t_start_1d = high_resolution_clock::now();

        constexpr size_t len_1d = len * len * len;
        constexpr double dx = 2 * M_PI / (len_1d);
        std::vector<double> vec_1d{};
        vec_1d.reserve(len_1d + 1);
        for (size_t i = 0; i <= len_1d; ++i) {
            vec_1d.emplace_back(
                std::sin(13 * (static_cast<double>(i) * dx - M_PI)));
        }

        std::vector<double> vals_1d;
        vals_1d.reserve(coord_1d.size());
        for (auto& x : coord_1d) { vals_1d.push_back(std::sin(13 * x)); }

        const auto t_after_vec = high_resolution_clock::now();

        InterpolationFunction1D<> interp1d(
            std::make_pair(-M_PI, M_PI),
            std::make_pair(vec_1d.begin(), vec_1d.end()), 3, true);

        const auto t_after_interpolation = high_resolution_clock::now();

        for (size_t i = 0; i < eval_count; ++i) {
            interp1d(rand_dist(rand_gen));
        }

        const auto t_after_eval = high_resolution_clock::now();

        double err_1d =
            rel_err(interp1d, std::make_pair(coord_1d.begin(), coord_1d.end()),
                    std::make_pair(vals_1d.begin(), vals_1d.end()));
        assertion(err_1d < eps);
        std::cout << "Interpolation 1d trigonometric function with err = "
                  << err_1d << '\n';

        std::cout << "Interpolation on a 1D mesh consisting " << vec_1d.size()
                  << " points. Then evaluate the function on " << eval_count
                  << " points.\n\n";
        std::cout << "Phase\t\t\tTime\n";
        std::cout << "Mesh\t\t\t"
                  << duration<double, milliseconds::period>(t_after_vec -
                                                            t_start_1d)
                         .count()
                  << "ms\n";
        std::cout << "Interpolate\t\t"
                  << duration<double, milliseconds::period>(
                         t_after_interpolation - t_after_vec)
                         .count()
                  << "ms\n";
        std::cout << "Evaluate\t\t"
                  << duration<double, milliseconds::period>(
                         t_after_eval - t_after_interpolation)
                         .count()
                  << "ms\n\n";
    }

    // 2d case
    {
        const auto t_start_2d = high_resolution_clock::now();

        const size_t len_2d = static_cast<std::size_t>(std::pow(len, 1.5));
        const double dt = 2 * M_PI / static_cast<double>(len_2d);

        Mesh<double, 2> trig_mesh_2d(len_2d + 1);
        for (size_t i = 0; i <= len_2d; ++i) {
            for (size_t j = 0; j <= len_2d; ++j) {
                trig_mesh_2d(i, j) =
                    std::sin(static_cast<double>(i) * dt - M_PI) *
                    std::cos(static_cast<double>(j) * dt - M_PI);
            }
        }

        std::vector<double> vals_2d;
        vals_2d.reserve(coord_2d.size());

        for (auto& pt : coord_2d) {
            vals_2d.push_back(std::sin(pt[0]) * std::cos(pt[1]));
        }

        const auto t_after_mesh = high_resolution_clock::now();

        InterpolationFunction<double, 2> interp2d(3, {true, true}, trig_mesh_2d,
                                                  std::make_pair(-M_PI, M_PI),
                                                  std::make_pair(-M_PI, M_PI));

        const auto t_after_interpolation = high_resolution_clock::now();

        for (size_t i = 0; i < eval_count; ++i) {
            interp2d(rand_dist(rand_gen), rand_dist(rand_gen));
        }

        const auto t_after_eval = high_resolution_clock::now();

        double err_2d =
            rel_err(interp2d, std::make_pair(coord_2d.begin(), coord_2d.end()),
                    std::make_pair(vals_2d.begin(), vals_2d.end()));
        assertion(err_2d < eps);
        std::cout << "Interpolation 2d trigonometric function with err = "
                  << err_2d << '\n';

        std::cout << "Interpolation on a 2D mesh consisting "
                  << trig_mesh_2d.size()
                  << " points. Then evaluate the function on " << eval_count
                  << " points\n\n";
        std::cout << "Phase\t\t\tTime\n";
        std::cout << "Mesh\t\t\t"
                  << duration<double, milliseconds::period>(t_after_mesh -
                                                            t_start_2d)
                         .count()
                  << "ms\n";
        std::cout << "Interpolate\t\t"
                  << duration<double, milliseconds::period>(
                         t_after_interpolation - t_after_mesh)
                         .count()
                  << "ms\n";
        std::cout << "Evaluate\t\t"
                  << duration<double, milliseconds::period>(
                         t_after_eval - t_after_interpolation)
                         .count()
                  << "ms\n\n";
    }

    // 3d case
    {
        const auto t_start_3d = high_resolution_clock::now();

        constexpr double dt_3d = 2 * M_PI / len;
        constexpr double dt_3d_aperiodic = 1. / (len - 1);

        Mesh<double, 3> mesh_3d{len + 1, len + 1, len};
        for (size_t i = 0; i <= len; ++i) {
            for (size_t j = 0; j <= len; ++j) {
                for (size_t k = 0; k < len; ++k) {
                    mesh_3d(i, j, k) =
                        std::sin(static_cast<double>(i) * dt_3d - M_PI) *
                        std::cos(static_cast<double>(j) * dt_3d - M_PI) *
                        std::exp(-std::pow(
                            static_cast<double>(k) * dt_3d_aperiodic - .5, 2));
                }
            }
        }

        std::vector<double> vals_3d;
        vals_3d.reserve(coord_3d.size());

        for (auto& pt : coord_3d) {
            vals_3d.push_back(std::sin(pt[0]) * std::cos(pt[1]) *
                              std::exp(-std::pow(pt[2], 2)));
        }

        const auto t_after_mesh_3d = high_resolution_clock::now();

        InterpolationFunction<double, 3> interp3d(
            3, {true, true, false}, mesh_3d, std::make_pair(-M_PI, M_PI),
            std::make_pair(-M_PI, M_PI), std::make_pair(-.5, .5));

        const auto t_after_interpolation = high_resolution_clock::now();

        for (size_t i = 0; i < eval_count; ++i) {
            interp3d(rand_dist(rand_gen), rand_dist(rand_gen),
                     rand_dist2(rand_gen));
        }

        const auto t_after_eval = high_resolution_clock::now();

        double err_3d =
            rel_err(interp3d, std::make_pair(coord_3d.begin(), coord_3d.end()),
                    std::make_pair(vals_3d.begin(), vals_3d.end()));
        assertion(err_3d < eps);
        std::cout << "Interpolation 3d trig-exp function with err = " << err_3d
                  << '\n';

        std::cout << "Interpolation on a 3D mesh consisting " << mesh_3d.size()
                  << " points. Then evaluate the function on " << eval_count
                  << " points\n\n";
        std::cout << "Phase\t\t\tTime\n";
        std::cout << "Mesh\t\t\t"
                  << duration<double, milliseconds::period>(t_after_mesh_3d -
                                                            t_start_3d)
                         .count()
                  << "ms\n";
        std::cout << "Interpolate\t\t"
                  << duration<double, milliseconds::period>(
                         t_after_interpolation - t_after_mesh_3d)
                         .count()
                  << "ms\n";
        std::cout << "Evaluate\t\t"
                  << duration<double, milliseconds::period>(
                         t_after_eval - t_after_interpolation)
                         .count()
                  << "ms\n\n";
    }

    return assertion.status();
}
