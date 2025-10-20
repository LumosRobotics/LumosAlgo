#ifndef DUOPLOT_FILLABLE_UINT8_ARRAY_H_
#define DUOPLOT_FILLABLE_UINT8_ARRAY_H_

#include <stdint.h>

#include "lumos/plotting/logging.h"

class UInt8ArrayView
{
private:
    const uint8_t *const data_;
    size_t size_;

public:
    UInt8ArrayView() = delete;
    UInt8ArrayView(const uint8_t *const data_in, const size_t size_in) : data_{data_in}, size_{size_in} {}

    const uint8_t *data() const
    {
        return data_;
    }

    size_t size() const
    {
        return size_;
    }
};

class FillableUInt8Array
{
private:
    uint8_t *data_;
    size_t size_;
    size_t idx_;

public:
    FillableUInt8Array() = delete;
    FillableUInt8Array(const size_t size) : data_{new uint8_t[size]}, size_{size}, idx_{0U}
    {
        if (size == 0U)
        {
            throw std::runtime_error("FillableUInt8Array: size cannot be 0.");
        }
    }
    FillableUInt8Array(const FillableUInt8Array &a) = delete;
    ~FillableUInt8Array()
    {
        idx_ = 0U;
        size_ = 0U;
        delete[] data_;
        data_ = nullptr;
    }

    uint8_t *data() const
    {
        return data_;
    }

    size_t size() const
    {
        return size_;
    }

    template <typename T>
    void fillWithStaticType(const T &d)
    {
        std::memcpy(&(data_[idx_]), &d, sizeof(T));
        idx_ += sizeof(T);

        DUOPLOT_ASSERT(idx_ <= size_);
    }

    template <typename T>
    void fillWithDataFromPointer(const T *const d, const size_t num_elements)
    {
        std::memcpy(&(data_[idx_]), d, num_elements * sizeof(T));
        idx_ += num_elements * sizeof(T);

        DUOPLOT_ASSERT(idx_ <= size_);
    }

    UInt8ArrayView view() const
    {
        return UInt8ArrayView{data_, size_};
    }
};

#endif // DUOPLOT_FILLABLE_UINT8_ARRAY_H_
