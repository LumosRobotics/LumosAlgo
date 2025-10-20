#ifndef DUOPLOT_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_H_
#define DUOPLOT_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_H_

#include <cmath>
#include <cstring>

#include "lumos/plotting/logging.h"
#include "lumos/plotting/math/lin_alg/matrix_fixed/class_def/matrix_fixed.h"
#include "lumos/plotting/math/misc/math_macros.h"

namespace duoplot
{

    template <typename T, uint16_t R, uint16_t C>
    template <typename Y>
    MatrixFixed<T, R, C>::MatrixFixed(const MatrixFixed<Y, R, C> &m)
    {
        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                data_[r * C + c] = m(r, c);
            }
        }
    }

    template <typename T, uint16_t R, uint16_t C>
    MatrixFixed<T, R, C>::MatrixFixed()
    {
        static_assert((R * C) < 10001, "Too many elements!");
    }

    template <typename T, uint16_t R, uint16_t C>
    void MatrixFixed<T, R, C>::fillBufferWithData(uint8_t *const buffer) const
    {
        const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
        const size_t num_bytes = R * C * sizeof(T);
        std::memcpy(buffer, internal_ptr, num_bytes);
    }

    template <typename T, uint16_t R, uint16_t C>
    size_t MatrixFixed<T, R, C>::size() const
    {
        return R * C;
    }

    template <typename T, uint16_t R, uint16_t C>
    T *MatrixFixed<T, R, C>::data() const
    {
        return data_;
    }

    template <typename T, uint16_t R, uint16_t C>
    size_t MatrixFixed<T, R, C>::numRows() const
    {
        return R;
    }

    template <typename T, uint16_t R, uint16_t C>
    size_t MatrixFixed<T, R, C>::numCols() const
    {
        return C;
    }

    template <typename T, uint16_t R, uint16_t C>
    size_t MatrixFixed<T, R, C>::numElements() const
    {
        // Returns totalt number of elements in matrix
        return R * C;
    }

    template <typename T, uint16_t R, uint16_t C>
    size_t MatrixFixed<T, R, C>::numBytes() const
    {
        return R * C * sizeof(T);
    }

    template <typename T, uint16_t R, uint16_t C>
    void MatrixFixed<T, R, C>::fill(T val)
    {
        for (size_t k = 0; k < (R * C); k++)
        {
            data_[k] = val;
        }
    }

    template <typename T, uint16_t R, uint16_t C>
    T &MatrixFixed<T, R, C>::operator()(const size_t r, const size_t c)
    {
        assert((r < R) && "Row index is larger than R - 1!");
        assert((c < C) && "Column index is larger than C - 1!");

        return data_[r * C + c];
    }

    template <typename T, uint16_t R, uint16_t C>
    const T &MatrixFixed<T, R, C>::operator()(const size_t r, const size_t c) const
    {
        assert((r < R) && "Row index is larger than R - 1!");
        assert((c < C) && "Column index is larger than C - 1!");

        return data_[r * C + c];
    }

    template <typename T, uint16_t R, uint16_t C>
    MatrixFixed<T, R, C> MatrixFixed<T, R, C>::transposed() const
    {
        MatrixFixed<T, R, C> m_out;

        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                m_out(c, r) = (*this)(r, c);
            }
        }

        return m_out;
    }

    template <typename T, uint16_t R0, uint16_t C0, uint16_t R1, uint16_t C1>
    MatrixFixed<T, R0, C1> operator*(const MatrixFixed<T, R0, C0> &m0, const MatrixFixed<T, R1, C1> &m1)
    {
        static_assert(C0 == R1);
        MatrixFixed<T, R0, C1> res;

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                T p = 0.0;
                for (size_t i = 0; i < m0.numCols(); i++)
                {
                    p = p + m0(r, i) * m1(i, c);
                }
                res(r, c) = p;
            }
        }
        return res;
    }

    /*template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator+(const MatrixFixed<T, R, C>& m0, const
    MatrixFixed<T, R, C>& m1)
    {
        DUOPLOT_ASSERT(m0.numCols() == m1.numCols());
        DUOPLOT_ASSERT(m0.numRows() == m1.numRows());
        MatrixFixed<T, R, C> res(m0.numRows(), m1.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m0(r, c) + m1(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator-(const MatrixFixed<T, R, C>& m0, const
    MatrixFixed<T, R, C>& m1)
    {
        DUOPLOT_ASSERT(m0.numCols() == m1.numCols());
        DUOPLOT_ASSERT(m0.numRows() == m1.numRows());
        MatrixFixed<T, R, C> res(m0.numRows(), m1.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m0(r, c) - m1(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator*(const MatrixFixed<T, R, C>& m, const T f)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = f * m(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator*(const T f, const MatrixFixed<T, R, C>& m)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = f * m(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator^(const MatrixFixed<T, R, C>& m0, const
    MatrixFixed<T, R, C>& m1)
    {
        DUOPLOT_ASSERT(m0.numRows() == m1.numRows());
        DUOPLOT_ASSERT(m0.numCols() == m1.numCols());

        MatrixFixed<T, R, C> res(m0.numRows(), m0.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m0(r, c) * m1(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator/(const MatrixFixed<T, R, C>& m0, const
    MatrixFixed<T, R, C>& m1)
    {
        DUOPLOT_ASSERT(m0.numRows() == m1.numRows());
        DUOPLOT_ASSERT(m0.numCols() == m1.numCols());

        MatrixFixed<T, R, C> res(m0.numRows(), m0.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m0(r, c) / m1(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator/(const MatrixFixed<T, R, C>& m, const T f)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m(r, c) / f;
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator/(const T f, const MatrixFixed<T, R, C>& m)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = f / m(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator-(const MatrixFixed<T, R, C>& m, const T f)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m(r, c) - f;
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator-(const T f, const MatrixFixed<T, R, C>& m)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = f - m(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator+(const MatrixFixed<T, R, C>& m, const T f)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m(r, c) + f;
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator+(const T f, const MatrixFixed<T, R, C>& m)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = m(r, c) + f;
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> operator-(const MatrixFixed<T, R, C>& m)
    {
        MatrixFixed<T, R, C> res(m.numRows(), m.numCols());

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = -m(r, c);
            }
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> Vector<T> operator*(const MatrixFixed<T, R, C>& m, const Vector<T>& v)
    {
        DUOPLOT_ASSERT(m.numCols() == v.size());
        Vector<T> res(m.numRows());

        for (size_t r = 0; r < m.numRows(); r++)
        {
            T p = 0.0f;
            for (size_t c = 0; c < m.numCols(); c++)
            {
                p = p + m(r, c) * v(c);
            }
            res(r) = p;
        }
        return res;
    }

    template <typename T, uint16_t R, uint16_t C> Vector<T> operator*(const Vector<T>& v, const MatrixFixed<T, R, C>& m)
    {
        DUOPLOT_ASSERT(m.numRows() == v.size());
        Vector<T> res(m.numCols());

        for (size_t c = 0; c < m.numCols(); c++)
        {
            T p = 0.0f;
            for (size_t r = 0; r < m.numRows(); r++)
            {
                p = p + m(r, c) * v(r);
            }
            res(c) = p;
        }
        return res;
    }*/

    /*template <typename T, uint16_t R, uint16_t C> MatrixFixed<T, R, C> MatrixFixed<T, R, C>::getTranspose() const
    {
        MatrixFixed<T, R, C> res(num_cols_, num_rows_);

        for (size_t r = 0; r < res.numRows(); r++)
        {
            for (size_t c = 0; c < res.numCols(); c++)
            {
                res(r, c) = data_[c * num_cols_ + r];
            }
        }
        return res;
    }*/

    template <typename T, uint16_t R, uint16_t C>
    MatrixFixed<T, R, C> unitMatrix()
    {
        MatrixFixed<T, R, C> unit_matrix;
        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                if (r == c)
                {
                    unit_matrix(r, c) = 1.0;
                }
                else
                {
                    unit_matrix(r, c) = 0.0;
                }
            }
        }
        return unit_matrix;
    }

    template <typename T, uint16_t R, uint16_t C>
    MatrixFixed<T, R, C> zerosMatrix()
    {
        MatrixFixed<T, R, C> zero_matrix;
        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                zero_matrix(r, c) = 0.0;
            }
        }
        return zero_matrix;
    }

    template <typename T, uint16_t R, uint16_t C>
    MatrixFixed<T, R, C> onesMatrix()
    {
        MatrixFixed<T, R, C> ones_matrix;
        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                ones_matrix(r, c) = 1.0;
            }
        }
        return ones_matrix;
    }

    template <typename T, uint16_t R, uint16_t C>
    std::ostream &operator<<(std::ostream &os, const MatrixFixed<T, R, C> &m)
    {
        std::string s = "";

        for (size_t r = 0; r < R; r++)
        {
            s = s + "[ ";
            for (size_t c = 0; c < C; c++)
            {
                s = s + std::to_string(m(r, c));
                if (c != C - 1)
                {
                    s = s + ", ";
                }
            }
            s = s + " ]\n";
        }

        os << s;

        return os;
    }

    template <typename T, uint16_t R, uint16_t C>
    T MatrixFixed<T, R, C>::max() const
    {
        T max_val = data_[0];

        for (size_t r = 0; r < R; r++)
        {
            const size_t r_times_c = r * C;

            for (size_t c = 0; c < C; c++)
            {
                max_val = std::max(max_val, data_[r_times_c + c]);
            }
        }

        return max_val;
    }

    template <typename T, uint16_t R, uint16_t C>
    T MatrixFixed<T, R, C>::min() const
    {
        T min_val = data_[0];

        for (size_t r = 0; r < R; r++)
        {
            const size_t r_times_c = r * C;

            for (size_t c = 0; c < C; c++)
            {
                min_val = std::min(min_val, data_[r_times_c + c]);
            }
        }

        return min_val;
    }

    template <typename T, uint16_t R, uint16_t C>
    T MatrixFixed<T, R, C>::sum() const
    {
        T s = 0.0;
        for (size_t r = 0; r < R; r++)
        {
            const size_t r_times_c = r * C;
            for (size_t c = 0; c < C; c++)
            {
                s = s + data_[r_times_c + c];
            }
        }
        return s;
    }

    template <typename T>
    MatrixFixed<T, 3, 3> fixedRotationMatrixX(const T angle)
    {
        const T ca = std::cos(angle);
        const T sa = std::sin(angle);
        MatrixFixed<T, 3, 3> rotation_matrix;

        rotation_matrix(0, 0) = 1.0;
        rotation_matrix(0, 1) = 0.0;
        rotation_matrix(0, 2) = 0.0;

        rotation_matrix(1, 0) = 0.0;
        rotation_matrix(1, 1) = ca;
        rotation_matrix(1, 2) = -sa;

        rotation_matrix(2, 0) = 0.0;
        rotation_matrix(2, 1) = sa;
        rotation_matrix(2, 2) = ca;

        return rotation_matrix;
    }

    template <typename T>
    MatrixFixed<T, 3, 3> fixedRotationMatrixY(const T angle)
    {
        const T ca = std::cos(angle);
        const T sa = std::sin(angle);
        MatrixFixed<T, 3, 3> rotation_matrix;

        rotation_matrix(0, 0) = ca;
        rotation_matrix(0, 1) = 0.0;
        rotation_matrix(0, 2) = sa;

        rotation_matrix(1, 0) = 0.0;
        rotation_matrix(1, 1) = 1.0;
        rotation_matrix(1, 2) = 0.0;

        rotation_matrix(2, 0) = -sa;
        rotation_matrix(2, 1) = 0.0;
        rotation_matrix(2, 2) = ca;

        return rotation_matrix;
    }

    template <typename T>
    MatrixFixed<T, 3, 3> fixedRotationMatrixZ(const T angle)
    {
        const T ca = std::cos(angle);
        const T sa = std::sin(angle);
        MatrixFixed<T, 3, 3> rotation_matrix;

        rotation_matrix(0, 0) = ca;
        rotation_matrix(0, 1) = -sa;
        rotation_matrix(0, 2) = 0.0;

        rotation_matrix(1, 0) = sa;
        rotation_matrix(1, 1) = ca;
        rotation_matrix(1, 2) = 0.0;

        rotation_matrix(2, 0) = 0.0;
        rotation_matrix(2, 1) = 0.0;
        rotation_matrix(2, 2) = 1.0;

        return rotation_matrix;
    }

    template <typename T>
    MatrixFixed<T, 2, 2> fixedRotationMatrix2D(const T angle)
    {
        const T ca = std::cos(angle);
        const T sa = std::sin(angle);
        MatrixFixed<T, 2, 2> rotation_matrix;

        rotation_matrix(0, 0) = ca;
        rotation_matrix(0, 1) = -sa;

        rotation_matrix(1, 0) = sa;
        rotation_matrix(1, 1) = ca;

        return rotation_matrix;
    }

    template <typename T, uint16_t R, uint16_t C>
    MatrixFixed<T, R, C> unitMatrixFixed()
    {
        MatrixFixed<T, R, C> unit_mat;

        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                if (r == c)
                {
                    unit_mat(r, c) = 1.0;
                }
                else
                {
                    unit_mat(r, c) = 0.0;
                }
            }
        }

        return unit_mat;
    }

} // namespace duoplot

#endif // DUOPLOT_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_H_
