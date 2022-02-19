#include <cmath>  // ceil

#include <Eigen/Sparse>

#include "BSpline.hpp"
#include "BandMatrix.hpp"

template <typename T, unsigned D>
class InterpolationFunction {
   private:
    using val_type = T;
    using spline_type = BSpline<T, D>;
    using size_type = typename spline_type::size_type;

    const size_type order;
    const static size_type dim = D;

    spline_type spline;
    bool _uniform;
    std::array<val_type, dim> _dx;

    // auxiliary methods

    template <typename... Coords, unsigned... indices>
    val_type call_op_helper(util::index_sequence<indices...>,
                            Coords... coords) const {
        return _uniform ? spline(std::make_pair(
                              coords,
                              std::min(spline.knots_num(indices) - order - 2,
                                       (size_type)std::ceil(std::max(
                                           0., (coords - range(indices).first) /
                                                       _dx[indices] -
                                                   .5 * (order + 1))) +
                                           order))...)
                        : spline(coords...);
    }

    template <typename Arr, size_type... indices>
    val_type call_op_arr_helper(util::index_sequence<indices...> i,
                                Arr& x) const {
        return call_op_helper(i, x[indices]...);
    }

   public:
    /**
     * @brief Construct a new 1D Interpolation Function object, mimicking
     * Mathematica's `Interpolation` function, with option `Method->"Spline"`.
     *
     * @tparam InputIter
     * @param order order of interpolation, the interpolated function is of
     * $C^{order-1}$
     * @param f_range a pair of iterators defining to-be-interpolated data
     * @param x_range a pair of x_min and x_max
     */
    template <
        typename InputIter,
        typename = typename std::enable_if<
            dim == 1u && std::is_convertible<typename std::iterator_traits<
                                                 InputIter>::iterator_category,
                                             std::input_iterator_tag>::value,
            int>::type>
    InterpolationFunction(size_type order,
                          std::pair<InputIter, InputIter> f_range,
                          std::pair<double, double> x_range)
        : InterpolationFunction(
              order,
              Mesh<val_type, 1u>{f_range.first, f_range.second},
              x_range) {}

    /**
     * @brief Construct a new nD Interpolation Function object, mimicking
     * Mathematica's `Interpolation` function, with option `Method->"Spline"`.
     *
     * @tparam RangePairs
     * @param order order of interpolation, the interpolated function is of
     * $C^{order-1}$
     * @param f_mesh a mesh containing data to be interpolated
     * @param x_ranges pairs of x_min and x_max
     */
    template <typename... RangePairs>
    InterpolationFunction(size_type order,
                          Mesh<val_type, dim> f_mesh,
                          RangePairs... x_ranges)
        : order(order), _uniform(true), spline(order) {
        // load knots into spline
        util::dispatch_indexed(
            [&](size_type dim_ind,
                std::pair<typename spline_type::knot_type,
                          typename spline_type::knot_type> x_range) {
                const size_type n = f_mesh.dim_size(dim_ind);
                _dx[dim_ind] = (x_range.second - x_range.first) / (n - 1);

                std::vector<typename spline_type::knot_type> xs(n + order + 1,
                                                                x_range.first);

                for (int i = order + 1; i < xs.size() - order - 1; ++i) {
                    xs[i] =
                        x_range.first + .5 * (2 * i - order - 1) * _dx[dim_ind];
                }
                for (int i = xs.size() - order - 1; i < xs.size(); ++i) {
                    xs[i] = x_range.second;
                }

                spline.load_knots(dim_ind, std::move(xs));
            },
            x_ranges...);

        Eigen::SparseMatrix<double, Eigen::RowMajor> coef(f_mesh.size(),
                                                          f_mesh.size());
        // numbers of base spline covering
        std::vector<size_type> entry_per_row(f_mesh.size());
        for (size_type total_ind = 0; total_ind < f_mesh.size(); ++total_ind) {
            auto indices = f_mesh.dimwise_indices(total_ind);
            size_type c = 1;
            for (size_type d = 0; d < dim; ++d) {
                auto ind = indices[d];
                c *= ind == 0 || ind == f_mesh.dim_size(d) - 1 ? 1
                     : ind <= order / 2 ||
                             ind >= f_mesh.dim_size(d) - 1 - order / 2
                         ? order + 1
                         : order | 1;
            }
            entry_per_row[total_ind] = c;
        }
        coef.reserve(entry_per_row);
        Eigen::VectorXd mesh_val(f_mesh.size());

        std::array<typename spline_type::BaseSpline, dim> weights_per_dim;

        // loop over dimension to calculate 1D base spline for each dim
        for (auto it = f_mesh.begin(); it != f_mesh.end(); ++it) {
            size_type f_total_ind = std::distance(f_mesh.begin(), it);
            auto f_indices = f_mesh.iter_indices(it);
            decltype(f_indices) base_spline_anchor;

            for (int i = 0; i < dim; ++i) {
                const auto knot_num = spline.knots_num(i);
                // knot points has a larger gap in both ends
                const auto knot_ind = std::min(
                    knot_num - order - 2, f_indices[i] > order / 2
                                              ? f_indices[i] + (order + 1) / 2
                                              : order);

                if (knot_ind <= 2 * order + 1 ||
                    knot_ind >= knot_num - 2 * order - 2) {
                    // update base spline
                    const auto iter = spline.knots_begin(i) + knot_ind;
                    const double x =
                        spline.range(i).first + f_indices[i] * _dx[i];
                    weights_per_dim[i] = spline.base_spline_value(i, iter, x);
                }

                base_spline_anchor[i] = knot_ind - order;
            }

            // loop over nD base splines that contributes to current f_mesh
            // point, fill matrix
            for (int i = 0; i < util::pow(order + 1, dim); ++i) {
                std::array<unsigned, dim> ind_arr;
                val_type spline_val = 1;
                for (size_type j = 0, local_ind = i; j < dim; ++j) {
                    ind_arr[j] = local_ind % (order + 1);
                    local_ind /= (order + 1);

                    spline_val *= weights_per_dim[j][ind_arr[j]];
                    ind_arr[j] += base_spline_anchor[j];
                }
                if (spline_val != val_type{0}) {
                    coef.insert(f_total_ind, f_mesh.indexing(ind_arr)) =
                        spline_val;
                }
            }

            // fill rhs vector
            mesh_val(f_total_ind) = *it;
        }

        Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
            solver;
        solver.compute(coef);

        Eigen::VectorXd weights_vec;
        weights_vec = solver.solve(mesh_val);

        Mesh<val_type, dim> weights{f_mesh};
        weights.storage =
            std::vector<val_type>(weights_vec.begin(), weights_vec.end());

        spline.load_ctrlPts(std::move(weights));
    }

    const std::pair<typename spline_type::knot_type,
                    typename spline_type::knot_type>&
    range(size_type dim_ind) const {
        return spline.range(dim_ind);
    }

    template <typename... Coords,
              typename Indices = util::make_index_sequence_for<Coords...>,
              typename = typename std::enable_if<std::is_scalar<
                  typename std::common_type<Coords...>::type>::value>::type>
    val_type operator()(Coords... x) const {
        return call_op_helper(Indices{}, x...);
    }

    template <
        typename Arr,
        typename = typename std::enable_if<!std::is_scalar<Arr>::value>::type>
    val_type operator()(Arr& x) const {
        using Indices = util::make_index_sequence<dim>;
        return call_op_arr_helper(Indices{}, x);
    }
};
