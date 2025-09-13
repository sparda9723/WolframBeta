#pragma once
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
namespace cartesianR2 {

    struct vec2d {
        double x;
        double y;
        std::vector<double> toVector(double step = 0.1f) {
            std::vector<double> ret;
            int count = static_cast<int>((y - x) / step);
            for (int i = 0; i <= count; i++) {
                ret.push_back(x + i * step);
            }
            return ret;
        }
    };
    inline std::vector<vec2d> functToCart(std::function<double(double x)> F, vec2d domain = { -5.0,5.0 }) {
        std::vector<vec2d> ret;
        for (double i = domain.x; i < domain.y; i += 1.f) {
            ret.push_back({ i, F(i) });
        }
        return ret;
    }

    inline std::vector<std::string> genCart(int width, int height, int y_center) {
        std::vector<std::string> lines;

        for (int i = 0; i < height; ++i) {
            if (i == y_center) {
                lines.push_back(std::string(width / 2, '-') + '+' + std::string(width / 2, '-'));
            }
            else {
                lines.push_back(std::string(width / 2, ' ') + '|' + std::string(width / 2, ' '));
            }
        }

        return lines;
    }

    inline std::vector<std::string> plotter_cli(std::function<double(double)> F, vec2d domain = { -5.0, 5.0 }, vec2d y_range = { -10.0f, 10.0f }) {
        auto r2_array = cartesianR2::functToCart(F, domain);
        int width = static_cast<int>(r2_array.size());
        int height = 20;
        int x_center = width / 2;
        int y_center = -1;
        if (y_range.x < 0 && y_range.y > 0) {
            double zero_pos = (0.0f - y_range.x) / (y_range.y - y_range.x);
            y_center = static_cast<int>((1.0f - zero_pos) * height);
        }
        auto cartPlan = genCart(width, height, y_center);

        for (int px = 0; px < width; ++px) {
            double x = domain.x + (domain.y - domain.x) * px / width;
            double y = F(x);

            if (y < y_range.x || y > y_range.y || std::isnan(y) || std::isinf(y)) continue;

            double y_normalized = (y - y_range.x) / (y_range.y - y_range.x);
            int row = height - 1 - static_cast<int>(y_normalized * height);

            if (row >= 0 && row < height) {
                cartPlan[row][px] = '*';
            }
        }
        int resolution = 15;

        for (double i = domain.x; i < domain.y; i += 1.0f / resolution) {
            double y = F(i);

            if (y < y_range.x || y > y_range.y)
                continue;

            double y_normalized = (y - y_range.x) / (y_range.y - y_range.x);
            int row = height - 1 - static_cast<int>(y_normalized * height);
            int col = static_cast<int>((i - domain.x) * resolution);

            if (row >= 0 && row < height && col >= 0 && col < width)
                cartPlan[row][col] = '*';
        }
        return cartPlan;
    }


    inline std::string plotter_js(std::function<double(double)> F,vec2d domain = { -5.0, 5.0 }, int resolution = 200) {
        std::ostringstream oss;
        oss << "[";

        for (int i = 0; i <= resolution; i++) {
            double x = domain.x + (domain.y - domain.x) * i / double(resolution);
            double y = F(x);

            if (i > 0) oss << ",";
            if (std::isfinite(y))
                oss << "[" << x << "," << y << "]";
            else
                oss << "[" << x << ",null]";
        }

        oss << "]";
        return oss.str();
    }

    namespace utils {

        inline bool B_injectora(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }) {
            auto x_vals = domain.toVector(0.1);
            std::vector<double> y_seen;

            for (double x : x_vals) {
                double y = F(x);
                for (double seen : y_seen) {
                    if (std::fabs(seen - y) < 0.001) {
                        return false;
                    }
                }
                y_seen.push_back(y);
            }

            return true;
        }
        inline bool B_sobrejectora(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, vec2d contra = { -1000.0f, 1000.0f }, double step = 0.1f, double epsilon = 0.01f) {
            auto domainVec = domain.toVector(step);
            auto contraVec = contra.toVector(step);
            std::vector<double> image;

            for (double x : domainVec) {
                image.push_back(F(x));
            }

            for (double y : contraVec) {
                bool found = false;
                for (double fy : image) {
                    if (std::fabs(fy - y) < epsilon) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
            return true;
        }
        inline bool B_Bijectora(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, vec2d contra = { -1000.0f, 1000.0f }, double step = 0.1f, double epsilon = 0.01f) {
            return B_sobrejectora(F, domain, contra, step, epsilon) && B_injectora(F, domain);
        }
        inline bool B_periodica(std::function<double(double)> F,vec2d domain = { -10.0f, 10.0f },double epsilon = 0.1f,double step = 0.01f) {

            std::vector<double> periods = { 3.14159f, 6.28318f, 1.5708f };

            auto values = domain.toVector(step);

            for (double p : periods) {
                bool periodic = true;

                for (double x : values) {
                    double x_p = x + p;

                    if (x_p > domain.y || x_p < domain.x) continue;

                    double fx = F(x);
                    double fxp = F(x_p);

                    if (!std::isfinite(fx) || !std::isfinite(fxp)) {
                        periodic = false;
                        break;
                    }

                    if (std::fabs(fxp - fx) > epsilon) {
                        periodic = false;
                        break;
                    }
                }

                if (periodic) return true;
            }

            return false;
        }
        inline bool B_par(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, double epsilon = .01f) {
            for (double x : domain.toVector(0.1f)) {
                if (std::fabs(F(x) - F(-x)) > epsilon) {
                    return false;
                }
            }
            return true;
        }
        inline bool B_impar(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, double epsilon = .01f) {
            for (double x : domain.toVector(0.1f)) {
                if (std::fabs(F(-x) - (-F(x))) > epsilon) {
                    return false;
                }
            }
            return true;
        }
        inline double derivada(std::function<double(double x)> F,double a=0, double step = 0.01) {
            double h = 1e-5f;
            return (double)(F(a + h) - F(a - h)) / (2 * h);
        }
        inline double integralriemannstyle(std::function<double(double x)> F, double a = 0, double b=0,double n= 1000000) {
            double deltaecs = (b - a) / n;
            double sum = 0.0;

            for (int i = 0; i < n; i++) {
                double x = a + (i + 0.5) * deltaecs;
                sum += F(x) * deltaecs;
            }
            return sum;
        }
        
    }
}