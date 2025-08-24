#ifndef LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_CLASS_DEF_MATRIX_DYNAMIC_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_CLASS_DEF_MATRIX_DYNAMIC_H_

#include <assert.h>
#include <cstddef>
#include <vector>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <algorithm>

#include "logging.h"
#include "math/misc/forward_decl.h"

namespace lumos
{
  template <typename T>
  class MatrixInitializer
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;
    friend class Matrix<T>;

  public:
    MatrixInitializer() = delete;
    MatrixInitializer(const MatrixInitializer<T> &v) = delete;
    MatrixInitializer(MatrixInitializer<T> &&v);
    MatrixInitializer<T> &operator=(const MatrixInitializer<T> &v) = delete;
    MatrixInitializer<T> &operator=(MatrixInitializer<T> &&v) = delete;
    explicit MatrixInitializer(
        const std::initializer_list<std::initializer_list<T>> &il)
    {
      ASSERT(il.size() > 0) << "Tried to initialize with empty vector matrix!";
      ASSERT(il.begin()[0].size() > 0)
          << "Tried to initialize with empty vector matrix!";

      for (size_t r = 0; r < il.size(); r++)
      {
        ASSERT(il.begin()[0].size() == il.begin()[r].size())
            << "All row vectors in input std vectors do not have the same size!";
      }

      num_rows_ = il.size();
      num_cols_ = il.begin()[0].size();

      DATA_ALLOCATION(data_, num_cols_ * num_rows_, T, "MatrixInitializer");

      for (size_t r = 0; r < il.size(); r++)
      {
        for (size_t c = 0; c < il.begin()[r].size(); c++)
        {
          data_[r * num_cols_ + c] = il.begin()[r].begin()[c];
        }
      }
    }

    ~MatrixInitializer() { delete[] data_; }
  };

  template <typename T>
  class MatrixView
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    MatrixView() : data_{nullptr}, num_rows_{0U}, num_cols_{0U} {}

    MatrixView(T *const data_ptr_in, const size_t num_rows, const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols} {}

    T *data() const { return data_; }

    size_t size() const { return num_rows_ * num_cols_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t numElements() const { return num_rows_ * num_cols_; }

    size_t numBytes() const { return num_rows_ * num_cols_ * sizeof(T); }

    void fillBufferWithData(uint8_t *const buffer) const
    {
      const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
      const size_t num_bytes = num_rows_ * num_cols_ * sizeof(T);

      std::memcpy(buffer, internal_ptr, num_bytes);
    }

    T &operator()(const size_t r, const size_t c)
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

      return data_[r * num_cols_ + c];
    }

    const T &operator()(const size_t r, const size_t c) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

      return data_[r * num_cols_ + c];
    }

    std::pair<T, T> findMinMax() const
    {
      T min_value = data_[0], max_value = data_[0];

      for (size_t r = 0; r < num_rows_; r++)
      {
        const size_t idx = r * num_cols_;
        for (size_t c = 0; c < num_cols_; c++)
        {
          const T val = data_[idx + c];
          if (val < min_value)
          {
            min_value = val;
          }
          if (val > max_value)
          {
            max_value = val;
          }
        }
      }

      return {min_value, max_value};
    }
  };

  // Unified slicing example - single-file minimal Matrix implementation
  // Supports selectors: All, End, RangeInclusive (via 0_i <= 4_i), Indices, size_t

  // ---------------------------- Selectors ----------------------------------

  struct All
  {
  };
  struct End
  {
  };
  struct RangeInclusive
  {
    std::size_t first;
    std::size_t last; // inclusive
  };

  struct RangeUpTo
  {
    std::size_t end; // exclusive
    RangeUpTo(std::size_t e) : end(e) {}
  };

  struct RangeFrom
  {
    std::size_t start; // inclusive
    RangeFrom(std::size_t s) : start(s) {}
  };

  struct Indices
  {
    std::vector<std::size_t> values;
    Indices(std::initializer_list<std::size_t> list) : values(list) {}
    Indices(std::vector<std::size_t> v) : values(std::move(v)) {}
  };

  namespace internal
  {
    // ------------------------- forward declarations ---------------------------

    template <typename T>
    class MatrixGenericSlice;

    // ------------------------- Resolve selectors ------------------------------

    inline std::vector<std::size_t> resolve_indices(All, std::size_t dim)
    {
      std::vector<std::size_t> idx(dim);
      for (std::size_t i = 0; i < dim; ++i)
      {
        idx[i] = i;
      }
      return idx;
    }

    inline std::vector<std::size_t> resolve_indices(End, std::size_t dim)
    {
      if (dim == 0)
      {
        throw std::out_of_range("End on empty dimension");
      }
      return std::vector<std::size_t>{dim - 1};
    }

    inline std::vector<std::size_t> resolve_indices(std::size_t single, std::size_t dim)
    {
      if (single >= dim)
      {
        throw std::out_of_range("Index out of range");
      }
      return std::vector<std::size_t>{single};
    }

    inline std::vector<std::size_t> resolve_indices(const RangeInclusive &r, std::size_t dim)
    {
      if (r.first > r.last || r.last >= dim)
      {
        throw std::out_of_range("RangeInclusive out of bounds");
      }
      std::vector<std::size_t> idx(r.last - r.first + 1);
      for (std::size_t i = 0; i < idx.size(); ++i)
      {
        idx[i] = r.first + i;
      }
      return idx;
    }

    inline std::vector<std::size_t> resolve_indices(const Indices &ind, std::size_t dim)
    {
      for (auto i : ind.values)
      {
        if (i >= dim)
        {
          throw std::out_of_range("Index out of range");
        }
      }
      return ind.values;
    }

    template <typename T>
    class MatrixGenericSlice
    {
    private:
      T *parent_data_ = nullptr;
      std::vector<std::size_t> row_indices_;
      std::vector<std::size_t> col_indices_;
      std::size_t parent_stride_ = 0; // number of columns in parent

    public:
      MatrixGenericSlice(T *data,
                         std::vector<std::size_t> rows,
                         std::vector<std::size_t> cols,
                         std::size_t stride)
          : parent_data_(data), row_indices_(std::move(rows)), col_indices_(std::move(cols)), parent_stride_(stride) {}

      std::size_t numRows() const { return row_indices_.size(); }
      std::size_t numCols() const { return col_indices_.size(); }

      // element access (mapped into parent storage)
      T &operator()(std::size_t r, std::size_t c)
      {
        return parent_data_[row_indices_[r] * parent_stride_ + col_indices_[c]];
      }
      const T &operator()(std::size_t r, std::size_t c) const
      {
        return parent_data_[row_indices_[r] * parent_stride_ + col_indices_[c]];
      }

      // assign from any matrix-like object that provides numRows(), numCols(), operator()
      template <typename MatLike>
      MatrixGenericSlice &operator=(const MatLike &other)
      {
        if (other.numRows() != numRows() || other.numCols() != numCols())
          throw std::runtime_error("slice assignment size mismatch");
        for (std::size_t r = 0; r < numRows(); ++r)
        {
          for (std::size_t c = 0; c < numCols(); ++c)
          {
            (*this)(r, c) = other(r, c);
          }
        }
        return *this;
      }

      // inside internal::MatrixGenericSlice<T>
      operator Matrix<T>() const
      {
        Matrix<T> out(numRows(), numCols());
        for (size_t r = 0; r < numRows(); ++r)
          for (size_t c = 0; c < numCols(); ++c)
            out(r, c) = (*this)(r, c);
        return out;
      }

      // Allow assigning from raw scalar if the slice is a single element
      MatrixGenericSlice &operator=(const T &scalar)
      {
        if (numRows() != 1 || numCols() != 1)
          throw std::runtime_error("scalar assign size mismatch");
        (*this)(0, 0) = scalar;
        return *this;
      }
    };
  }
  template <typename T>
  class MatrixConstView
  {
  private:
    const T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    MatrixConstView() : data_{nullptr}, num_rows_{0U}, num_cols_{0U} {}

    MatrixConstView(const T *const data_ptr_in, const size_t num_rows,
                    const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols} {}

    const T *data() const { return data_; }

    size_t size() const { return num_rows_ * num_cols_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t numElements() const { return num_rows_ * num_cols_; }

    size_t numBytes() const { return num_rows_ * num_cols_ * sizeof(T); }

    void fillBufferWithData(uint8_t *const buffer) const
    {
      const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
      const size_t num_bytes = num_rows_ * num_cols_ * sizeof(T);

      std::memcpy(buffer, internal_ptr, num_bytes);
    }

    const T &operator()(const size_t r, const size_t c) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

      return data_[r * num_cols_ + c];
    }

    std::pair<T, T> findMinMax() const
    {
      T min_value = data_[0], max_value = data_[0];

      for (size_t r = 0; r < num_rows_; r++)
      {
        const size_t idx = r * num_cols_;
        for (size_t c = 0; c < num_cols_; c++)
        {
          const T val = data_[idx + c];
          if (val < min_value)
          {
            min_value = val;
          }
          if (val > max_value)
          {
            max_value = val;
          }
        }
      }

      return {min_value, max_value};
    }
  };

  template <typename T>
  class Matrix
  {
  protected:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    Matrix();
    Matrix(const size_t num_rows, const size_t num_cols);
    Matrix(const Matrix<T> &m);
    template <typename Y>
    Matrix(const Matrix<Y> &m);

    Matrix(MatrixInitializer<T> &&m)
    {
      data_ = m.data_;
      num_rows_ = m.num_rows_;
      num_cols_ = m.num_cols_;

      m.data_ = nullptr;
      m.num_rows_ = 0U;
      m.num_cols_ = 0U;
    }
    Matrix(Matrix<T> &&m);
    ~Matrix();

    void fillBufferWithData(uint8_t *const buffer) const;

    T &operator()(const size_t r, const size_t c);
    const T &operator()(const size_t r, const size_t c) const;

    Matrix<T> &operator=(const Matrix<T> &m);
    Matrix<T> &operator=(Matrix<T> &&m);

    Matrix<T> &operator=(const internal::MatrixGenericSlice<T> &m)
    {
      if (m.numRows() != numRows() || m.numCols() != numCols())
        throw std::runtime_error("Matrix assignment size mismatch");
      for (size_t r = 0; r < numRows(); ++r)
      {
        for (size_t c = 0; c < numCols(); ++c)
        {
          (*this)(r, c) = m(r, c);
        }
      }
      return *this;
    }

    MatrixView<T> view() const { return MatrixView{data_, num_rows_, num_cols_}; }

    MatrixConstView<T> constView() const
    {
      return MatrixConstView{data_, num_rows_, num_cols_};
    }

    // non-const: slicing for assignment (LHS)
    template <typename RowSel, typename ColSel>
    internal::MatrixGenericSlice<T> operator()(const RowSel &rsel, const ColSel &csel)
    {
      auto rows = internal::resolve_indices(rsel, num_rows_);
      auto cols = internal::resolve_indices(csel, num_cols_);
      return internal::MatrixGenericSlice<T>(data_, std::move(rows), std::move(cols), num_cols_);
    }

    // const: slicing produces a copy (RHS, auto, direct init)
    template <typename RowSel, typename ColSel>
    Matrix<T> operator()(const RowSel &rsel, const ColSel &csel) const
    {
      auto rows = internal::resolve_indices(rsel, num_rows_);
      auto cols = internal::resolve_indices(csel, num_cols_);
      return Matrix<T>(internal::MatrixGenericSlice<T>(data_, std::move(rows), std::move(cols), num_cols_));
    }

    void resize(const size_t num_rows, const size_t num_cols);
    size_t numRows() const;
    size_t numCols() const;
    size_t size() const;
    size_t numElements() const;
    size_t numBytes() const;

    void fill(const T val);
    T *data() const;
    Matrix<T> getTranspose() const;

    size_t lastRowIdx() const;
    size_t lastColIdx() const;

    T max() const;
    T min() const;
    T sum() const;
  };

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_CLASS_DEF_MATRIX_DYNAMIC_H_
