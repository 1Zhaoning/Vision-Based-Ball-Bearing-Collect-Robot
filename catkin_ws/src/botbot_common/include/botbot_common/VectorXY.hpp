#pragma once

#include <Eigen/Core>

using EigenVec2 = Eigen::Matrix<double, 2, 1>;

namespace mms {
class VectorXY : public EigenVec2
/**
    @brief A 2 element vector class based on eigen, specifically Eigen::Matrix<double, 2, 1>. Inherits almost all of the
   methods that eigen vectors have and can be multiplied with eigen vectors / matrices.
    @warning Many maths operations that combine eigen vectors/matrices and the VectorXY class will automatically cast
   the VectorXY to its parent eigen vector.
    @warning In order to create a derived class of this, you must include the command "using mms::VectorXY::VectorXY"
*/
{
public:
    // Default, Copy and move construtors.
    VectorXY() noexcept;
    VectorXY(const double _x, const double _y) noexcept;

    /**
        @brief Constructs a unit vector in the direction of the specified angle (in radians)
    */
    explicit VectorXY(const double angle) noexcept;

    explicit VectorXY(const VectorXY &obj) noexcept;
    VectorXY(const EigenVec2 &vec) noexcept;
    VectorXY(const std::initializer_list<double> &other) noexcept;

    VectorXY(VectorXY &&obj) noexcept;
    explicit VectorXY(EigenVec2 &&vec) noexcept;

    VectorXY &operator=(const VectorXY &other);
    VectorXY &operator=(const std::initializer_list<double> &other) noexcept;

    //    VectorXY &operator=(const EigenVec2 &other) noexcept;

    // Removing inherited eigen methods which we don't want;
    void resize()             = delete;
    void resizeLike()         = delete;
    void conservativeResize() = delete;
    double z()                = delete;
    double w()                = delete;

    /**
        @brief Returns the angle of this vector (between [-pi, pi])
    */
    [[nodiscard]] double angle() const noexcept;

    [[nodiscard]] double distance(const VectorXY &vec) const noexcept;

    [[nodiscard]] double determinant(const VectorXY &vec) const noexcept;

    /**
        @brief Returns the anti-clockwise angle between this vector and another.
    */
    [[nodiscard]] double angle_diff(const VectorXY &vec) const noexcept;

    /**
        @brief Returns of copy of this vector in (rho,theta) form; assuming it was in (x,y) form
    */
    [[nodiscard]] VectorXY xy_2_rho_theta() const noexcept;

    /**
        @brief Convert this vector in-place to (rho,theta) form; assuming it was in (x,y) form
    */
    VectorXY xy_2_rho_theta_IP() noexcept;

    /**
        @brief Returns of copy of this vector in (x,y) form; assuming it was in (rho,theta) form
    */
    [[nodiscard]] VectorXY rho_theta_2_xy() const noexcept;

    /**
        @brief Convert this vector in-place to (x,y) form; assuming it was in (rho,theta) form
    */
    VectorXY rho_theta_2_xy_IP() noexcept;

    /**
        @brief Rotates the vector in-place by *angle* radians in the CCW direction
    */
    VectorXY rotate_IP(const double angle) noexcept;

    /**
        @brief Returns a rotated copy of the vector by *angle* radians in the CCW direction
    */
    [[nodiscard]] VectorXY rotate(const double angle) const noexcept;

    /**
        @brief Returns the scalar parallel projection magnitude of this vector onto the given vector
    */
    [[nodiscard]] double parallel_resolute(const VectorXY &direction_vector) const;

    /**
    @brief Returns the projection of this vector onto the given direction vector
    */
    [[nodiscard]] VectorXY parallel_projection(const VectorXY &direction_vector) const;

    /**
    @brief Returns the scalar perpendicular projection magnitude of this vector onto the given vector
    */
    [[nodiscard]] double perpendicular_resolute(const VectorXY &direction_vector) const;

    /**
        @brief Returns the projection of this vector perpendicular to the given direction vector
    */
    [[nodiscard]] VectorXY perpendicular_projection(const VectorXY &direction_vector) const;

    /**
    @brief Adds guassian noise with the given standard deviation to the vector in-place.
    */
    VectorXY add_xy_noise_IP(const double stddev);

    /**
        @brief Adds rho (radial) and theta (anglular) guassian noise with the given standard deviation.
        In-place operation.
    */
    VectorXY add_rho_theta_noise_IP(const double stddev_rho, const double stddev_theta);

    template <typename container>[[nodiscard]]
    typename std::enable_if<std::is_same<typename container::value_type, VectorXY>::value, bool>::type
    in_polygon(const container &points) const
    /**
        @brief Determines if the VectorXY is within a polygon defined by a vector of vertices
        Based on:
        https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon/2922778#2922778
        This does not require that the last vertex is the same as the first vertex.
        @todo Check if this works with eigen stl iterators.
    */
    {
        const int num_vertices = points.size();
        int i;
        int j;
        bool crossed = false;
        // We look at one edge at a time, starting with (i=0)-->(end), then (i)-->(i++)
        for (i = 0, j = num_vertices - 1; i < num_vertices; j = i++)
        {
            if (((points[i](1) > (*this)(1)) != (points[j](1) > (*this)(1)))
                && ((*this)(0)
                    < (points[j](0) - points[i](0)) * ((*this)(1) - points[i](1)) / (points[j](1) - points[i](1))
                          + points[i](0)))
            { crossed = !crossed; }
        }
        return crossed;
    }

    template <typename container>[[nodiscard]] std::pair<int, double>
    // Template requirements don't work with eigen iterators
    // typename std::enable_if<std::is_same<typename container::value_type, VectorXY>::value, int>::type
    find_closest(const container &begin, const container &end) const
    /**
        @brief Takes in a begin and and iterator to a container (vector, array list, etc..) of VectorXYs and finds the
       index and distance to the closest point to this object (by euclidean distance).
    */
    {
        auto is_closer = [this](const EigenVec2 &vec1, const EigenVec2 &vec2) {
            return ((*this) - vec1).norm() < ((*this) - vec2).norm();
        };
        const auto iter    = std::min_element(begin, end, is_closer);
        const EigenVec2 pt = *iter; // Need explicit conversion or Eigen won't compile

        return {std::distance(begin, iter), (pt - (*this)).norm()};
    }

    [[nodiscard]] VectorXY operator+(const EigenVec2 &rhs) const noexcept;

    [[nodiscard]] VectorXY operator-(const EigenVec2 &rhs) const noexcept;

    [[nodiscard]] VectorXY operator*(const double scale) const;

    [[nodiscard]] VectorXY operator/(const double scale) const;
};
} // namespace mms