
template <typename T, uint16_t R, uint16_t C>
struct LUMatrices
{
    static constexpr uint16_t K = (R < C) ? R : C;

    FixedSizeMatrix<T, R, K> l_matrix;
    FixedSizeMatrix<T, K, C> u_matrix;
    FixedSizeVector<uint16_t, R> row_permutation; // maps row i in PA = LU
};

template <typename T, uint16_t R, uint16_t C>
struct SVDMatrices
{
    FixedSizeMatrix<T, R, R> u_matrix;
    FixedSizeMatrix<T, R, C> sigma_matrix;
    FixedSizeMatrix<T, C, C> v_matrix;
};

template <typename T, uint16_t R, uint16_t C>
struct QRResult
{
    FixedSizeMatrix<T, R, C> q; // Orthogonal (R×C for economy form)
    FixedSizeMatrix<T, C, C> r; // Upper triangular
};

template <typename T, uint16_t R, uint16_t C>
struct EigenDecomposition
{
    static_assert(R == C, "Eigen decomposition requires square matrix");
    FixedSizeMatrix<std::complex<T>, R, R> eigenvectors;
    FixedSizeVector<std::complex<T>, R> eigenvalues;
};

template <typename T, uint16_t R, uint16_t C>
class FixedSizeMatrix
{
    // protected:
public:
    T data_[R * C];

    FixedSizeMatrix();
    template <typename Y>
    FixedSizeMatrix(const FixedSizeMatrix<Y, R, C> &m);

    void fillBufferWithData(uint8_t *const buffer) const;

    T &operator()(const size_t r, const size_t c);
    const T &operator()(const size_t r, const size_t c) const;

    size_t numRows() const;
    size_t numCols() const;
    size_t size() const;
    size_t numElements() const;
    size_t numBytes() const;

    void fill(const T val);
    T *data() const;
    // Matrix<T, C, R> getTranspose() const;

    T max() const;
    T min() const;
    T sum() const;

    Matrix<T> toMatrix() const
    {
        Matrix<T> matrix(R, C);
        for (size_t r = 0; r < R; ++r)
        {
            for (size_t c = 0; c < C; ++c)
            {
                matrix(r, c) = data_[r * C + c];
            }
        }
        return matrix;
    }

    FixedSizeMatrix<T, C, R> transposed() const;

    std::optional<FixedSizeMatrix<T, R, C>> inverse() const;
    std::optional<LUMatrices<T, R, C>> luDecomposition() const;
    std::optional<SVDMatrices<T, R, C>> svd() const;
    std::optional<QRResult<T, R, C>> qrDecomposition() const;

    EigenDecomposition<T, R, C> eigen() const;
    FixedSizeMatrix<T, R, C> cholesky() const;

    T frobeniusNorm() const;
    T oneNorm() const;
    T infNorm() const;
    T pNorm(T p) const;

    // --- Condition number functions ---
    std::optional<T> condition_number_frobenius() const;
    std::optional<T> condition_number_l1() const;
    std::optional<T> condition_number_inf() const;
};

template <typename T>
struct Vec2
{
    T x;
    T y;

    Vec2(const T x_, const T y_);
    Vec2();
    template <typename Y>
    Vec2(const Vec2<Y> &v);

    Vec2<T> normalized() const;
    Vec2<T> vectorBetweenPoints(const Point2<T> &end_point) const;
    Vec2<T> normalizedVectorBetweenPoints(const Point2<T> &end_point) const;
    T squaredNorm() const;
    T norm() const;
    Vec2<T> elementWiseMultiply(const Vec2<T> &factor_vector) const;
    Vec2<T> elementWiseDivide(const Vec2<T> &numerator_vector) const;
    T angleBetweenVectors(const Vec2<T> &v) const;
    T angle() const;
};

template <typename T>
struct VecXY
{
    T x;
    T y;
    VecXY() = default;
    VecXY(const T x_, const T y_) : x(x_), y(y_) {}
    template <typename Y>
    VecXY(const VecXY<Y> &v) : x(v.x), y(v.y) {}
};

template <typename T>
struct VecXZ
{
    T x;
    T z;
    VecXZ() = default;
    VecXZ(const T x_, const T z_) : x(x_), z(z_) {}
    template <typename Y>
    VecXZ(const VecXZ<Y> &v) : x(v.x), z(v.z) {}
};

template <typename T>
struct VecYZ
{
    T y;
    T z;
    VecYZ() = default;
    VecYZ(const T y_, const T z_) : y(y_), z(z_) {}
    template <typename Y>
    VecYZ(const VecYZ<Y> &v) : y(v.y), z(v.z) {}
};

template <typename T>
struct Vec3
{
    T x;
    T y;
    T z;

    Vec3(const T x_, const T y_, const T z_);
    template <typename Y>
    Vec3(const Vec3<Y> &v);
    Vec3();

    Vec3<T> normalized() const;
    Vec3<T> vectorBetweenPoints(const Point3<T> &end_point) const;
    Vec3<T> normalizedVectorBetweenPoints(const Point3<T> &end_point) const;
    T squaredNorm() const;
    T norm() const;
    Vec3<T> elementWiseMultiply(const Vec3<T> &factor_vector) const;
    Vec3<T> elementWiseDivide(const Vec3<T> &numerator_vector) const;
    Vec3<T> crossProduct(const Vec3<T> &right_vector) const;
    Matrix<T> toCrossProductMatrix() const;
    T angleBetweenVectors(const Vec3<T> &v) const;
};

template <typename T>
struct Vec4
{
    T x;
    T y;
    T z;
    T w;

    Vec4(const T x_, const T y_, const T z_, const T w_);
    Vec4();
    template <typename Y>
    Vec4(const Vec4<Y> &v);

    Vec4<T> normalized() const;
    Vec4<T> vectorBetweenPoints(const Point4<T> &end_point) const;
    Vec4<T> normalizedVectorBetweenPoints(const Point4<T> &end_point) const;
    T squaredNorm() const;
    T norm() const;
    Vec4<T> elementWiseMultiply(const Vec4<T> &factor_vector) const;
    Vec4<T> elementWiseDivide(const Vec4<T> &numerator_vector) const;
    T angleBetweenVectors(const Vec4<T> &v) const;
};

template <typename T, uint16_t N>
class FixedSizeVector
{
private:
    T data_[N];

public:
    constexpr FixedSizeVector() = default;

    constexpr FixedSizeVector(std::initializer_list<T> values);
    constexpr T &operator[](const uint16_t index);
    constexpr const T &operator[](const uint16_t index) const;

    constexpr FixedSizeVector operator+(const FixedSizeVector &other) const;

    constexpr FixedSizeVector operator-(const FixedSizeVector &other) const;

    constexpr FixedSizeVector operator*(const T scalar) const;
    constexpr T dot(const FixedSizeVector &other) const;

    constexpr T norm() const;

    Vector<T> toDynamicVector() const;
};

template <typename T>
class Quaternion
{
public:
    T w, x, y, z;

    // Constructors
    constexpr Quaternion();
    constexpr Quaternion(T w, T x, T y, T z);
    // constexpr Quaternion(const FixedSizeVector<T, 3>& axis, T angle);

    // Quaternion operations
    constexpr Quaternion operator+(const Quaternion &other) const;
    constexpr Quaternion operator-(const Quaternion &other) const;
    constexpr Quaternion operator*(const Quaternion &other) const;
    constexpr Quaternion operator*(const T scalar) const;
    constexpr Quaternion conjugate() const;
    constexpr Quaternion inverse() const;
    constexpr FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;
    // constexpr FixedSizeVector<T, 3> toEulerAngles() const;
    // constexpr FixedSizeVector<T, 4> toAxisAngle() const;

    static Quaternion fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m);
    static Quaternion fromAxisAngle(const AxisAngle<T> &axis_angle);
    static Quaternion fromEulerAngles(const EulerAngles<T> &euler);

    // Normalization
    constexpr void normalize();

    // Output operator
    friend std::ostream &operator<<(std::ostream &os, const Quaternion &q)
    {
        os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
        return os;
    }
};

template <typename T>
class VectorInitializer
{
private:
    T *data_;
    size_t size_;
    friend class Vector<T>;

public:
    VectorInitializer() = delete;
    VectorInitializer(const VectorInitializer<T> &v) = delete;
    VectorInitializer(VectorInitializer<T> &&v);
    VectorInitializer<T> &operator=(const VectorInitializer<T> &v) = delete;
    VectorInitializer<T> &operator=(VectorInitializer<T> &&v) = delete;
    explicit VectorInitializer(const std::initializer_list<T> &il)
    {
        DATA_ALLOCATION(data_, il.size(), T, "VectorInitializer");

        size_ = il.size();

        size_t idx = 0;
        for (auto list_element : il)
        {
            data_[idx] = list_element;
            idx++;
        }
    }

    ~VectorInitializer() { delete[] data_; }
};

template <typename T>
class VectorView
{
private:
    T *data_;
    size_t size_;

public:
    VectorView() : data_{nullptr}, size_{0U} {}

    VectorView(T *const data_ptr_in, const size_t size_in)
        : data_{data_ptr_in}, size_{size_in} {}

    T *data() const { return data_; }

    size_t size() const { return size_; }

    size_t numBytes() const { return size_ * sizeof(T); }

    size_t numElements() const { return size_; }

    void fillBufferWithData(uint8_t *const buffer) const
    {
        const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
        const size_t num_bytes = size_ * sizeof(T);

        std::memcpy(buffer, internal_ptr, num_bytes);
    }

    T &operator()(const size_t idx)
    {
        assert(idx < size_);
        return data_[idx];
    }

    const T &operator()(const size_t idx) const
    {
        assert(idx < size_);
        return data_[idx];
    }

    std::pair<T, T> findMinMax() const
    {
        T min_value = data_[0], max_value = data_[0];

        for (size_t k = 0; k < size_; k++)
        {
            const T val = data_[k];
            if (val < min_value)
            {
                min_value = val;
            }
            if (val > max_value)
            {
                max_value = val;
            }
        }

        return {min_value, max_value};
    }
};

template <typename T>
class VectorConstView
{
private:
    const T *data_;
    size_t size_;

public:
    VectorConstView() : data_{nullptr}, size_{0U} {}

    VectorConstView(const T *const data_ptr_in, const size_t size_in)
        : data_{data_ptr_in}, size_{size_in} {}

    const T *data() const { return data_; }

    size_t size() const { return size_; }

    size_t numBytes() const { return size_ * sizeof(T); }

    size_t numElements() const { return size_; }

    void fillBufferWithData(uint8_t *const buffer) const
    {
        const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
        const size_t num_bytes = size_ * sizeof(T);

        std::memcpy(buffer, internal_ptr, num_bytes);
    }

    const T &operator()(const size_t idx) const
    {
        assert(idx < size_);
        return data_[idx];
    }

    std::pair<T, T> findMinMax() const
    {
        T min_value = data_[0], max_value = data_[0];

        for (size_t k = 0; k < size_; k++)
        {
            const T val = data_[k];
            if (val < min_value)
            {
                min_value = val;
            }
            if (val > max_value)
            {
                max_value = val;
            }
        }

        return {min_value, max_value};
    }
};

template <typename T>
class Vector
{
protected:
    T *data_;
    size_t size_;

public:
    Vector();
    explicit Vector(const size_t vector_length);
    Vector(const Vector<T> &v);
    Vector(Vector<T> &&v);

    Vector(VectorInitializer<T> &&v)
    {
        data_ = v.data_;
        size_ = v.size_;

        v.data_ = nullptr;
        v.size_ = 0U;
    }

    template <typename Y>
    Vector(const Vector<Y> &v);

    template <typename Y>
    friend void fillWithPtr(Vector<Y> &v, const void *const ptr,
                            const size_t vector_length);

    Vector(const std::vector<T> &v);

    ~Vector();

    VectorView<T> view() const { return VectorView{data_, size_}; }

    VectorConstView<T> constView() const { return VectorConstView{data_, size_}; }

    void fillBufferWithData(uint8_t *const buffer) const;

    Vector<T> &operator=(const Vector<T> &v);
    Vector<T> &operator=(Vector<T> &&v);
    T &operator()(const size_t idx);
    const T &operator()(const size_t idx) const;

    size_t size() const;
    size_t numElements() const;
    size_t numBytes() const;
    void fill(const T &val);
    void resize(const size_t new_size);
    size_t endIndex() const;
    T *data() const;
    T *begin() const;
    T *end() const;
    template <typename Y>
    Vector<T> &operator=(const Vector<Y> &rhs);

    T max() const;
    T min() const;
    T sum() const;

    template <uint16_t N>
    FixedSizeVector<T, N> toFixedSizeVector() const
    {
        static_assert(N <= 0xFFFF, "N must be less than or equal to 65535");
        assert(size_ == N && "Size mismatch in toFixedSizeVector conversion");

        FixedSizeVector<T, N> fixed_size_vector;
        for (uint16_t i = 0; i < N; ++i)
        {
            fixed_size_vector[i] = data_[i];
        }
        return fixed_size_vector;
    }
};

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

    MatrixView<T> view() const { return MatrixView{data_, num_rows_, num_cols_}; }

    MatrixConstView<T> constView() const
    {
        return MatrixConstView{data_, num_rows_, num_cols_};
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
