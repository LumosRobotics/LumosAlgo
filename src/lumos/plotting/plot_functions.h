#ifndef DUOPLOT_PLOT_FUNCTIONS_H
#define DUOPLOT_PLOT_FUNCTIONS_H

#include <stdlib.h>

#include <functional>
#include <map>
#include <utility>

#include "lumos/plotting/internal.h"
#include "lumos/plotting/math/math.h"

namespace lumos
{

    template <typename T, typename... Us>
    void plot(const Vector<T> &x, const Vector<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT) ||
            hdr.hasPropertyFlag(internal::PropertyFlag::APPENDABLE))
        {
            hdr.setFunction(internal::Function::FAST_PLOT2);
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void plot(const VectorConstView<T> &x, const VectorConstView<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT) ||
            hdr.hasPropertyFlag(internal::PropertyFlag::APPENDABLE))
        {
            hdr.setFunction(internal::Function::FAST_PLOT2);
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void plot(const Vector<T> &x, const Vector<T> &y, const Vector<properties::Color> &color, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
        {
            DUOPLOT_LOG_WARNING() << "Property FAST_PLOT not available when using function \"plot\" with color vector!";
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, color);
    }

    template <typename T, typename... Us>
    void plot(const VectorConstView<T> &x,
              const VectorConstView<T> &y,
              const VectorConstView<properties::Color> &color,
              const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
        {
            DUOPLOT_LOG_WARNING() << "Property FAST_PLOT not available when using function \"plot\" with color vector!";
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, color);
    }

    template <typename T, typename... Us>
    void lineCollection(const Vector<T> &x, const Vector<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::LINE_COLLECTION2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void lineCollection(const VectorConstView<T> &x, const VectorConstView<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::LINE_COLLECTION2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void lineCollection3(const Vector<T> &x, const Vector<T> &y, const Vector<T> &z, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::LINE_COLLECTION3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void lineCollection3(const VectorConstView<T> &x,
                         const VectorConstView<T> &y,
                         const VectorConstView<T> &z,
                         const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::LINE_COLLECTION3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void plotCollection(const std::vector<Vector<T>> &x, const std::vector<Vector<T>> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT_COLLECTION2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_OBJECTS, internal::toUInt32(x.size()));

        DUOPLOT_ASSERT(x.size() == y.size());

        uint32_t num_elements = 0;
        Vector<uint16_t> vector_lengths(x.size());

        for (size_t k = 0; k < x.size(); k++)
        {
            DUOPLOT_ASSERT(x[k].size() == y[k].size());
            vector_lengths(k) = x[k].size();
            num_elements += x[k].size();
        }

        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
        hdr.extend(settings...);

        const size_t num_bytes_to_send = sizeof(T) * 2 * num_elements;

        internal::sendHeaderAndVectorCollection(internal::getSendFunction(), hdr, vector_lengths, num_bytes_to_send, x, y);
    }

    template <typename T, typename... Us>
    void plotCollection(const std::vector<VectorConstView<T>> &x,
                        const std::vector<VectorConstView<T>> &y,
                        const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT_COLLECTION2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_OBJECTS, internal::toUInt32(x.size()));

        DUOPLOT_ASSERT(x.size() == y.size());

        uint32_t num_elements = 0;
        Vector<uint16_t> vector_lengths(x.size());

        for (size_t k = 0; k < x.size(); k++)
        {
            const VectorConstView<T> x_view = x[k];
            const VectorConstView<T> y_view = y[k];
            DUOPLOT_ASSERT(x_view.size() == y_view.size());
            vector_lengths(k) = x_view.size();
            num_elements += x_view.size();
        }

        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
        hdr.extend(settings...);

        const size_t num_bytes_to_send = sizeof(T) * 2 * num_elements;

        internal::sendHeaderAndVectorCollection(internal::getSendFunction(), hdr, vector_lengths, num_bytes_to_send, x, y);
    }

    template <typename T, typename... Us>
    void plotCollection3(const std::vector<Vector<T>> &x,
                         const std::vector<Vector<T>> &y,
                         const std::vector<Vector<T>> &z,
                         const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT_COLLECTION3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_OBJECTS, internal::toUInt32(x.size()));

        DUOPLOT_ASSERT(x.size() == y.size());
        DUOPLOT_ASSERT(x.size() == z.size());

        uint32_t num_elements = 0;
        Vector<uint16_t> vector_lengths(x.size());

        for (size_t k = 0; k < x.size(); k++)
        {
            DUOPLOT_ASSERT(x[k].size() == y[k].size());
            DUOPLOT_ASSERT(x[k].size() == z[k].size());
            vector_lengths(k) = x[k].size();
            num_elements += x[k].size();
        }

        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
        hdr.extend(settings...);

        const size_t num_bytes_to_send = sizeof(T) * 3 * num_elements;

        internal::sendHeaderAndVectorCollection(
            internal::getSendFunction(), hdr, vector_lengths, num_bytes_to_send, x, y, z);
    }

    template <typename T, typename... Us>
    void plotCollection3(const std::vector<VectorConstView<T>> &x,
                         const std::vector<VectorConstView<T>> &y,
                         const std::vector<VectorConstView<T>> &z,
                         const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT_COLLECTION3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_OBJECTS, internal::toUInt32(x.size()));

        DUOPLOT_ASSERT(x.size() == y.size());
        DUOPLOT_ASSERT(x.size() == z.size());

        uint32_t num_elements = 0;
        Vector<uint16_t> vector_lengths(x.size());

        for (size_t k = 0; k < x.size(); k++)
        {
            const VectorConstView<T> x_view = x[k];
            const VectorConstView<T> y_view = y[k];
            const VectorConstView<T> z_view = z[k];
            DUOPLOT_ASSERT(x_view.size() == y_view.size());
            DUOPLOT_ASSERT(x_view.size() == z_view.size());
            vector_lengths(k) = x_view.size();
            num_elements += x_view.size();
        }

        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
        hdr.extend(settings...);

        const size_t num_bytes_to_send = sizeof(T) * 3 * num_elements;

        internal::sendHeaderAndVectorCollection(
            internal::getSendFunction(), hdr, vector_lengths, num_bytes_to_send, x, y, z);
    }

    template <typename T, typename... Us>
    void stairs(const Vector<T> &x, const Vector<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::STAIRS};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void stairs(const VectorConstView<T> &x, const VectorConstView<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::STAIRS};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void plot3(const Vector<T> &x, const Vector<T> &y, const Vector<T> &z, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
        {
            hdr.setFunction(internal::Function::FAST_PLOT3);
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void plot3(const VectorConstView<T> &x, const VectorConstView<T> &y, const VectorConstView<T> &z, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
        {
            hdr.setFunction(internal::Function::FAST_PLOT3);
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void plot3(const Vector<T> &x,
               const Vector<T> &y,
               const Vector<T> &z,
               const Vector<properties::Color> &color,
               const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
        {
            DUOPLOT_LOG_WARNING() << "Property FAST_PLOT not available when using function \"plot3\" with color vector!";
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, color);
    }

    template <typename T, typename... Us>
    void plot3(const VectorConstView<T> &x,
               const VectorConstView<T> &y,
               const VectorConstView<T> &z,
               const VectorConstView<properties::Color> &color,
               const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
        {
            DUOPLOT_LOG_WARNING() << "Property FAST_PLOT not available when using function \"plot3\" with color vector!";
        }

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, color);
    }

    template <typename T, typename... Us>
    void stem(const Vector<T> &x, const Vector<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::STEM};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void stem(const VectorConstView<T> &x, const VectorConstView<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::STEM};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void scatter(const Vector<T> &x, const Vector<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void scatter(const Vector<T> &x, const Vector<T> &y, const Vector<T> &point_sizes, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_POINT_SIZES, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, point_sizes);
    }

    template <typename T, typename... Us>
    void scatter(const Vector<T> &x, const Vector<T> &y, const Vector<properties::Color> &color, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, color);
    }

    template <typename T, typename... Us>
    void scatter(const Vector<T> &x,
                 const Vector<T> &y,
                 const Vector<T> &point_sizes,
                 const Vector<properties::Color> &color,
                 const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_POINT_SIZES, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, point_sizes, color);
    }

    template <typename T, typename... Us>
    void scatter(const Vector<T> &x,
                 const Vector<T> &y,
                 const Vector<properties::Color> &color,
                 const Vector<T> &point_sizes,
                 const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_POINT_SIZES, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, point_sizes, color);
    }

    template <typename T, typename... Us>
    void scatter(const VectorConstView<T> &x,
                 const VectorConstView<T> &y,
                 const VectorConstView<properties::Color> &color,
                 const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, color);
    }

    template <typename T, typename... Us>
    void scatter(const VectorConstView<T> &x, const VectorConstView<T> &y, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void drawPoint(const Point2<T> &p, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER2};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(1));
        hdr.extend(settings...);

        Vector<T> x(1), y(1);
        x(0) = p.x;
        y(0) = p.y;

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void scatter3(const Vector<T> &x, const Vector<T> &y, const Vector<T> &z, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void scatter3(const VectorConstView<T> &x,
                  const VectorConstView<T> &y,
                  const VectorConstView<T> &z,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void scatter3(const Vector<T> &x,
                  const Vector<T> &y,
                  const Vector<T> &z,
                  const Vector<properties::Color> &color,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, color);
    }

    template <typename T, typename... Us>
    void scatter3(const Vector<T> &x,
                  const Vector<T> &y,
                  const Vector<T> &z,
                  const Vector<properties::Color> &color,
                  const Vector<T> &point_sizes,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_POINT_SIZES, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, point_sizes, color);
    }

    template <typename T, typename... Us>
    void scatter3(const Vector<T> &x,
                  const Vector<T> &y,
                  const Vector<T> &z,
                  const Vector<T> &point_sizes,
                  const Vector<properties::Color> &color,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_POINT_SIZES, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, point_sizes, color);
    }

    template <typename T, typename... Us>
    void scatter3(
        const Vector<T> &x, const Vector<T> &y, const Vector<T> &z, const Vector<T> &point_sizes, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_POINT_SIZES, internal::toUInt8(1));
        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, point_sizes);
    }

    template <typename T, typename... Us>
    void drawPoint(const Point3<T> &p, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCATTER3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(1));
        hdr.extend(settings...);

        Vector<T> x(1), y(1), z(1);
        x(0) = p.x;
        y(0) = p.y;
        z(0) = p.z;

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void surf(const Matrix<T> &x, const Matrix<T> &y, const Matrix<T> &z, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SURF};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D, internal::Dimension2D(x.numRows(), x.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void surf(const Matrix<T> &x,
              const Matrix<T> &y,
              const Matrix<T> &z,
              const Matrix<properties::Color> &color,
              const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SURF};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D, internal::Dimension2D(x.numRows(), x.numCols()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, color);
    }

    template <typename T, typename... Us>
    void surf(const MatrixConstView<T> &x, const MatrixConstView<T> &y, const MatrixConstView<T> &z, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SURF};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(x.size())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D, internal::Dimension2D(x.numRows(), x.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z);
    }

    template <typename T, typename... Us>
    void imShow(const ImageGray<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numElements())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageGrayAlpha<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(2));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numElements())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageRGB<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(3));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numRows() * img.numCols())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageRGBA<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(4));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numRows() * img.numCols())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageGrayConstView<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numElements())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageGrayAlphaConstView<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(2));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numElements())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageRGBConstView<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(3));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numRows() * img.numCols())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void imShow(const ImageRGBAConstView<T> &img, const Us &...settings)
    {
        static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value || std::is_same<T, double>::value,
                      "Only float, double and uint8_t supported for imShow!");

        internal::CommunicationHeader hdr{internal::Function::IM_SHOW};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_CHANNELS, internal::toUInt8(4));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS,
                   internal::toUInt32(img.numRows() * img.numCols())); // TODO: Needed?
        hdr.append(internal::CommunicationHeaderObjectType::DIMENSION_2D,
                   internal::Dimension2D(img.numRows(), img.numCols()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, img);
    }

    template <typename T, typename... Us>
    void drawMesh(const Vector<Point3<T>> &vertices, const Vector<IndexTriplet> &indices, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(vertices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, vertices, indices);
    }

    template <typename T, typename... Us>
    void drawMesh(const VectorConstView<Point3<T>> &vertices,
                  const VectorConstView<IndexTriplet> &indices,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(vertices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, vertices, indices);
    }

    template <typename T, typename... Us>
    void drawMesh(const Vector<T> &x,
                  const Vector<T> &y,
                  const Vector<T> &z,
                  const Vector<IndexTriplet> &indices,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH_SEPARATE_VECTORS};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, indices);
    }

    template <typename T, typename... Us>
    void drawMesh(const Vector<Point3<T>> &vertices,
                  const Vector<IndexTriplet> &indices,
                  const Vector<properties::Color> &colors,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(vertices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, vertices, indices, colors);
    }

    template <typename T, typename... Us>
    void drawMesh(const Vector<T> &x,
                  const Vector<T> &y,
                  const Vector<T> &z,
                  const Vector<IndexTriplet> &indices,
                  const Vector<properties::Color> &colors,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH_SEPARATE_VECTORS};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, indices, colors);
    }

    template <typename T, typename... Us>
    void drawMesh(const VectorConstView<Point3<T>> &vertices,
                  const VectorConstView<IndexTriplet> &indices,
                  const VectorConstView<properties::Color> &colors,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(vertices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, vertices, indices, colors);
    }

    template <typename T, typename... Us>
    void drawMesh(const VectorConstView<T> &x,
                  const VectorConstView<T> &y,
                  const VectorConstView<T> &z,
                  const VectorConstView<IndexTriplet> &indices,
                  const VectorConstView<properties::Color> &colors,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH_SEPARATE_VECTORS};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::HAS_COLOR, internal::toUInt8(1));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, indices, colors);
    }

    template <typename T, typename... Us>
    void drawMesh(const VectorConstView<T> &x,
                  const VectorConstView<T> &y,
                  const VectorConstView<T> &z,
                  const VectorConstView<IndexTriplet> &indices,
                  const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::DRAW_MESH_SEPARATE_VECTORS};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_VERTICES, internal::toUInt32(x.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_INDICES, internal::toUInt32(indices.size()));
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(indices.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y, z, indices);
    }

    template <typename... Us>
    void drawLine(const Line3D<double> &line, const double t0, const double t1, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::PLOT3};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::DataType::DOUBLE);
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(2));
        const Point3<double> p0 = line.eval(t0);
        const Point3<double> p1 = line.eval(t1);
        const Vector<double> x{VectorInitializer{p0.x, p1.x}};
        const Vector<double> y{VectorInitializer{p0.y, p1.y}};

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, x, y);
    }

    template <typename T, typename... Us>
    void realTimePlot(const T dt, const T y, const ItemId id, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::REAL_TIME_PLOT};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::typeToDataTypeEnum<T>());
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(1U));
        hdr.append(internal::CommunicationHeaderObjectType::ITEM_ID, id);
        hdr.extend(settings...);

        const Vector<T> data{VectorInitializer<T>{dt, y}};

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, data);
    }

    template <typename T, typename... Us>
    void drawCubes(const VectorConstView<T> &x,
                   const VectorConstView<T> &y,
                   const VectorConstView<T> &z,
                   const T side_length,
                   const Us &...settings)
    {
        const size_t num_cubes = x.size();
        const size_t num_triangles = num_cubes * 6U * 2U;
        const size_t num_points = num_triangles * 3U;

        const T size = side_length / static_cast<T>(2.0);

        Vector<T> x_new(num_points), y_new(num_points), z_new(num_points);
        Vector<IndexTriplet> indices(num_triangles);

        size_t points_idx{0U};
        size_t indices_idx{0U};

        for (size_t i = 0; i < num_cubes; i++)
        {
            const T x_i = x(i);
            const T y_i = y(i);
            const T z_i = z(i);

            // XY plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i + size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i + size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i - size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i - size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // XZ plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i + size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i + size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i - size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i - size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // YZ plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i + size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i + size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i - size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i - size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;
        }

        drawMesh(x_new, y_new, z_new, indices, settings...);
    }

    template <typename T, typename... Us>
    void drawCubes(const Vector<T> &x, const Vector<T> &y, const Vector<T> &z, const T side_length, const Us &...settings)
    {
        drawCubes(x.constView(), y.constView(), z.constView(), side_length, settings...);
    }

    template <typename T, typename... Us>
    void drawCubes(const VectorConstView<T> &x,
                   const VectorConstView<T> &y,
                   const VectorConstView<T> &z,
                   const VectorConstView<properties::Color> &colors,
                   const T side_length,
                   const Us &...settings)
    {
        const size_t num_cubes = x.size();
        const size_t num_triangles = num_cubes * 6U * 2U;
        const size_t num_points = num_triangles * 3U;

        const T size = side_length / static_cast<T>(2.0);

        Vector<T> x_new(num_points), y_new(num_points), z_new(num_points);
        Vector<properties::Color> colors_new{num_triangles};
        Vector<IndexTriplet> indices(num_triangles);

        size_t points_idx{0U};
        size_t indices_idx{0U};

        for (size_t i = 0; i < num_cubes; i++)
        {
            const T x_i = x(i);
            const T y_i = y(i);
            const T z_i = z(i);
            const properties::Color col_i = colors(i);

            // XY plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i + size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i + size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i - size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i - size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // XZ plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i + size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i + size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i - size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i - size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // YZ plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i + size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i + size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i - size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i - size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);
            colors_new(indices_idx) = col_i;

            points_idx += 3U;
            indices_idx += 1U;
        }

        drawMesh(x_new.constView(),
                 y_new.constView(),
                 z_new.constView(),
                 indices.constView(),
                 colors_new.constView(),
                 settings...);
    }

    template <typename T, typename... Us>
    void drawCubes(const Vector<T> &x,
                   const Vector<T> &y,
                   const Vector<T> &z,
                   const Vector<properties::Color> &colors,
                   const T side_length,
                   const Us &...settings)
    {
        drawCubes(x.constView(), y.constView(), z.constView(), colors.constView(), side_length, settings...);
    }

    template <typename T, typename... Us>
    void drawCubes(const VectorConstView<T> &x,
                   const VectorConstView<T> &y,
                   const VectorConstView<T> &z,
                   const VectorConstView<T> side_lengths,
                   const Us &...settings)
    {
        const size_t num_cubes = x.size();
        const size_t num_triangles = num_cubes * 6U * 2U;
        const size_t num_points = num_triangles * 3U;

        Vector<T> x_new(num_points), y_new(num_points), z_new(num_points);
        Vector<IndexTriplet> indices(num_triangles);

        size_t points_idx{0U};
        size_t indices_idx{0U};

        for (size_t i = 0; i < num_cubes; i++)
        {
            const T x_i = x(i);
            const T y_i = y(i);
            const T z_i = z(i);

            const T size = side_lengths(i);

            // XY plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i + size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i + size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i - size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i - size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // XZ plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i + size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i + size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i - size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i - size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // YZ plane
            // First triangle, positive xy plane, lower triangle
            x_new(points_idx) = x_i + size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Second triangle, positive xy plane, upper triangle
            x_new(points_idx) = x_i + size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i + size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i + size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Third triangle, negative xy plane, lower triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i - size;
            z_new(points_idx + 1) = z_i + size;

            x_new(points_idx + 2) = x_i - size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;

            // Fourth triangle, negative xy plane, upper triangle
            x_new(points_idx) = x_i - size;
            y_new(points_idx) = y_i - size;
            z_new(points_idx) = z_i - size;

            x_new(points_idx + 1) = x_i - size;
            y_new(points_idx + 1) = y_i + size;
            z_new(points_idx + 1) = z_i - size;

            x_new(points_idx + 2) = x_i - size;
            y_new(points_idx + 2) = y_i + size;
            z_new(points_idx + 2) = z_i + size;

            indices(indices_idx) = IndexTriplet(points_idx, points_idx + 1, points_idx + 2);

            points_idx += 3U;
            indices_idx += 1U;
        }

        drawMesh(x_new, y_new, z_new, indices, settings...);
    }

    template <typename T, typename... Us>
    void drawCubes(
        const Vector<T> &x, const Vector<T> &y, const Vector<T> &z, const Vector<T> side_lengths, const Us &...settings)
    {
        drawCubes(x.constView(), y.constView(), z.constView(), side_lengths.constView(), settings...);
    }

    template <typename T, typename... Us>
    void drawCube(const T x, const T y, const T z, const T size, const Us &...settings)
    {
        const Vector<T> x_vec{VectorInitializer<T>{x}};
        const Vector<T> y_vec{VectorInitializer<T>{y}};
        const Vector<T> z_vec{VectorInitializer<T>{z}};

        drawCubes(x_vec, y_vec, z_vec, size, settings...);
    }

    template <typename... Us>
    void drawScreenSpaceCoordinates(const Vector<std::array<Point2f, 3U>> &triangles, const Us &...settings)
    {
        internal::CommunicationHeader hdr{internal::Function::SCREEN_SPACE_PRIMITIVE};
        hdr.append(internal::CommunicationHeaderObjectType::DATA_TYPE, internal::DataType::FLOAT);
        hdr.append(internal::CommunicationHeaderObjectType::NUM_ELEMENTS, internal::toUInt32(triangles.size()));

        hdr.extend(settings...);

        internal::sendHeaderAndData(internal::getSendFunction(), hdr, triangles);
    }

    template <typename... Us>
    void drawScreenSpaceRect(const float x_upper_left, const float y_upper_left, const float width, const float height, const Us &...settings)
    {
        using Triangle = std::array<Point2f, 3U>;

        const Triangle t0{Point2f{x_upper_left, y_upper_left},
                          Point2f{x_upper_left + width, y_upper_left},
                          Point2f{x_upper_left + width, y_upper_left + height}};
        const Triangle t1{Point2f{x_upper_left, y_upper_left},
                          Point2f{x_upper_left, y_upper_left + height},
                          Point2f{x_upper_left + width, y_upper_left + height}};
        const Vector<Triangle> triangles{VectorInitializer<Triangle>{t0, t1}};
        drawScreenSpaceCoordinates(triangles, settings...);
    }

} // namespace lumos

#endif // DUOPLOT_PLOT_FUNCTIONS_H
