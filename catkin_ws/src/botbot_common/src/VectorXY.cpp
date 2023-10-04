#include "../include/botbot_common/VectorXY.hpp"

#include "../include/botbot_common/MathUtils.hpp"


mms::VectorXY::VectorXY() noexcept : EigenVec2(0, 0) {}

mms::VectorXY::VectorXY(const double _x, const double _y) noexcept : EigenVec2(_x, _y) {}

mms::VectorXY::VectorXY(const double angle) noexcept : EigenVec2(std::cos(angle), std::sin(angle)) {}

mms::VectorXY::VectorXY(const mms::VectorXY &obj) noexcept : EigenVec2(obj) {}

mms::VectorXY::VectorXY(const EigenVec2 &vec) noexcept : EigenVec2(vec) {}

mms::VectorXY::VectorXY(const std::initializer_list<double> &other) noexcept
    : EigenVec2(*other.begin(), *(other.begin() + 1))
{
}

mms::VectorXY::VectorXY(mms::VectorXY &&obj) noexcept : EigenVec2(std::move(obj)) {}

mms::VectorXY::VectorXY(EigenVec2 &&vec) noexcept : EigenVec2(std::move(vec)) {}

// mms::VectorXY &mms::VectorXY::operator=(const EigenVec2 &other) noexcept
//{
//    this->data()[0] = other.data()[0];
//    this->data()[1] = other.data()[1];
//    return *this;
//}

double mms::VectorXY::angle() const noexcept { return std::atan2((*this)(1), (*this)(0)); }

double mms::VectorXY::distance(const mms::VectorXY &vec) const noexcept { return (*this - vec).norm(); }

double mms::VectorXY::determinant(const mms::VectorXY &vec) const noexcept
{
    return ((*this)(0) * vec(1)) - (vec(0) * (*this)(1));
}

double mms::VectorXY::angle_diff(const mms::VectorXY &vec) const noexcept
{
    return std::atan2(this->determinant(vec), this->dot(vec));
}

mms::VectorXY mms::VectorXY::xy_2_rho_theta() const noexcept { return {this->norm(), this->angle()}; }

mms::VectorXY mms::VectorXY::xy_2_rho_theta_IP() noexcept
{
    *this = this->xy_2_rho_theta();
    return *this;
}

mms::VectorXY mms::VectorXY::rho_theta_2_xy() const noexcept
{
    double rho   = (*this)(0);
    double theta = (*this)(1);
    return {rho * std::cos(theta), rho * std::sin(theta)};
}

mms::VectorXY mms::VectorXY::rho_theta_2_xy_IP() noexcept
{
    *this = this->rho_theta_2_xy();
    return *this;
}

mms::VectorXY mms::VectorXY::rotate_IP(const double angle) noexcept

{
    *this = this->rotate(angle);
    return *this;
}

mms::VectorXY mms::VectorXY::rotate(const double angle) const noexcept
{
    return {std::cos(angle) * (*this)(0) - std::sin(angle) * (*this)(1),
            std::sin(angle) * (*this)(0) + std::cos(angle) * (*this)(1)};
}

double mms::VectorXY::parallel_resolute(const mms::VectorXY &direction_vector) const
{
    return this->dot(direction_vector.normalized());
}

mms::VectorXY mms::VectorXY::parallel_projection(const mms::VectorXY &direction_vector) const
{
    VectorXY proj_vec = direction_vector.normalized();
    proj_vec *= this->dot(proj_vec);
    return proj_vec;
}

double mms::VectorXY::perpendicular_resolute(const mms::VectorXY &direction_vector) const
{
    return this->perpendicular_projection(direction_vector).norm();
}

mms::VectorXY mms::VectorXY::perpendicular_projection(const mms::VectorXY &direction_vector) const
{
    return (*this) - this->parallel_projection(direction_vector);
}

mms::VectorXY mms::VectorXY::add_xy_noise_IP(const double stddev)
{
    (*this)(0) += mms::MathUtils::gaussian_noise(stddev);
    (*this)(1) += mms::MathUtils::gaussian_noise(stddev);
    return *this;
}

mms::VectorXY mms::VectorXY::add_rho_theta_noise_IP(const double stddev_rho, const double stddev_theta)
{
    const double new_rho = mms::MathUtils::gaussian_noise(stddev_rho, this->norm());
    (*this) *= new_rho / this->norm();

    const double theta_noise = mms::MathUtils::gaussian_noise(stddev_theta, 0);
    this->rotate_IP(theta_noise);
    return *this;
}

mms::VectorXY mms::VectorXY::operator+(const EigenVec2 &rhs) const noexcept
{
    VectorXY result(*this);
    result += rhs;
    return result;
}

mms::VectorXY mms::VectorXY::operator-(const EigenVec2 &rhs) const noexcept
{
    VectorXY result(*this);
    result -= rhs;
    return result;
}

mms::VectorXY mms::VectorXY::operator*(const double scale) const { return {(*this)(0) * scale, (*this)(1) * scale}; }

mms::VectorXY mms::VectorXY::operator/(const double scale) const { return {(*this)(0) / scale, (*this)(1) / scale}; }

mms::VectorXY &mms::VectorXY::operator=(const std::initializer_list<double> &other) noexcept
{
    (*this)(0) = *other.begin();
    (*this)(1) = *(other.begin() + 1);
    return *this;
}

mms::VectorXY &mms::VectorXY::operator=(const mms::VectorXY &other)
{
    (*this)(0) = other.data()[0];
    (*this)(1) = other.data()[1];
    return *this;
}
