#ifndef DUOPLOT_ENCODE_DECODE_FUNCTIONS_DEFS_H
#define DUOPLOT_ENCODE_DECODE_FUNCTIONS_DEFS_H

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <stdexcept>
#include <vector>

#include "lumos/plotting/plot_properties.h"

namespace duoplot
{
    namespace internal
    {

        struct CommunicationHeaderObject;

        inline uint8_t numBytes(const properties::Alpha &obj);
        inline uint8_t numBytes(const properties::LineWidth &obj);
        inline uint8_t numBytes(const properties::ZOffset &obj);
        inline uint8_t numBytes(const properties::Transform &obj);
        inline uint8_t numBytes(const properties::Label &obj);
        inline uint8_t numBytes(const properties::LineStyle &obj);
        inline uint8_t numBytes(const properties::ScatterStyle &obj);
        inline uint8_t numBytes(const internal::ColorInternal &obj);
        inline uint8_t numBytes(const properties::EdgeColor &obj);
        inline uint8_t numBytes(const properties::FaceColor &obj);
        inline uint8_t numBytes(const properties::Silhouette &obj);
        inline uint8_t numBytes(const properties::ColorMap &obj);
        inline uint8_t numBytes(const properties::PointSize &obj);
        inline uint8_t numBytes(const properties::DistanceFrom &obj);
        inline uint8_t numBytes(const properties::BufferSize &obj);
        inline uint8_t numBytes(const internal::PropertyFlag &obj);
        inline uint8_t numBytes(const internal::PropertyBase &obj);
        inline uint8_t numBytes(const ItemId &obj);

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Label &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const PropertyBase &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::LineWidth &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Alpha &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::ZOffset &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Transform &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Label &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::LineStyle &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::ScatterStyle &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const internal::ColorInternal &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::EdgeColor &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Silhouette &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::FaceColor &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::ColorMap &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::PointSize &prop);
        inline void serializeToCommunicationHeaderObject(::duoplot::internal::CommunicationHeaderObject &obj,
                                                         const properties::DistanceFrom &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::BufferSize &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const internal::PropertyFlag &prop);
        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const ItemId &prop);

        inline void deserializeFromCommunicationHeaderObject(internal::PropertyBase &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::LineWidth &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::Alpha &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::ZOffset &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::Transform &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::Label &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(internal::ColorInternal &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::EdgeColor &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::FaceColor &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::Silhouette &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::LineStyle &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::ScatterStyle &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::ColorMap &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::PointSize &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::DistanceFrom &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(properties::BufferSize &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(internal::PropertyFlag &prop,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(ItemId &prop, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(internal::Dimension2D &item, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(std::pair<Vec3<double>, Vec3<double>> &item,
                                                             const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(internal::DataType &item, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(Vec3<double> &item, const CommunicationHeaderObject &obj);
        inline void deserializeFromCommunicationHeaderObject(MatrixFixed<double, 3, 3> &item,
                                                             const CommunicationHeaderObject &obj);

    } // namespace internal
} // namespace duoplot

#endif // DUOPLOT_ENCODE_DECODE_FUNCTIONS_DEFS_H
