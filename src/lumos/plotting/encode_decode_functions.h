#ifndef DUOPLOT_ENCODE_DECODE_FUNCTIONS_H
#define DUOPLOT_ENCODE_DECODE_FUNCTIONS_H

#include "lumos/plotting/plot_properties.h"

namespace duoplot
{
    namespace internal
    {

        /////////////// PropertyBase ///////////////
        inline uint8_t numBytes(const internal::PropertyBase &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const PropertyBase &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
        }

        inline void deserializeFromCommunicationHeaderObject(internal::PropertyBase &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
        }

        /////////////// Alpha ///////////////
        inline uint8_t numBytes(const properties::Alpha &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(float);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Alpha &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            std::memcpy(obj.data + 1U, &(prop.data), sizeof(float));
        }

        inline void deserializeFromCommunicationHeaderObject(properties::Alpha &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            std::memcpy(&(prop.data), obj.data + 1U, sizeof(float));
        }

        /////////////// LineWidth ///////////////
        inline uint8_t numBytes(const properties::LineWidth &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::LineWidth &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1U] = prop.data;
        }

        inline void deserializeFromCommunicationHeaderObject(properties::LineWidth &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.data = obj.data[1U];
        }

        /////////////// ZOffset ///////////////
        inline uint8_t numBytes(const properties::ZOffset &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(float);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::ZOffset &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            std::memcpy(obj.data + 1U, &(prop.data), sizeof(float));
        }

        inline void deserializeFromCommunicationHeaderObject(properties::ZOffset &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            std::memcpy(&(prop.data), obj.data + 1U, sizeof(float));
        }

        /////////////// Transform ///////////////
        inline uint8_t numBytes(const properties::Transform &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(double) * (3U * 3U + 3U * 3U + 3U);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Transform &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());

            size_t idx = 1U;

            for (size_t r = 0; r < 3; r++)
            {
                for (size_t c = 0; c < 3; c++)
                {
                    std::memcpy(obj.data + idx, &(prop.rotation(r, c)), sizeof(double));
                    idx += sizeof(double);
                }
            }
            for (size_t r = 0; r < 3; r++)
            {
                for (size_t c = 0; c < 3; c++)
                {
                    std::memcpy(obj.data + idx, &(prop.scale(r, c)), sizeof(double));
                    idx += sizeof(double);
                }
            }

            std::memcpy(obj.data + idx, &(prop.translation.x), sizeof(double));
            idx += sizeof(double);

            std::memcpy(obj.data + idx, &(prop.translation.y), sizeof(double));
            idx += sizeof(double);

            std::memcpy(obj.data + idx, &(prop.translation.z), sizeof(double));
            idx += sizeof(double);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::Transform &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));

            size_t idx = 1U;

            for (size_t r = 0; r < 3; r++)
            {
                for (size_t c = 0; c < 3; c++)
                {
                    std::memcpy(&(prop.rotation(r, c)), obj.data + idx, sizeof(double));
                    idx += sizeof(double);
                }
            }
            for (size_t r = 0; r < 3; r++)
            {
                for (size_t c = 0; c < 3; c++)
                {
                    std::memcpy(&(prop.scale(r, c)), obj.data + idx, sizeof(double));
                    idx += sizeof(double);
                }
            }

            std::memcpy(&(prop.translation.x), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(prop.translation.y), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(prop.translation.z), obj.data + idx, sizeof(double));
            idx += sizeof(double);
        }

        /////////////// Label ///////////////
        inline uint8_t numBytes(const properties::Label &obj)
        {
            return sizeof(uint8_t) + sizeof(uint8_t) + static_cast<uint8_t>(obj.length);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Label &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1] = static_cast<uint8_t>(prop.length);
            std::memcpy(obj.data + 2U, prop.data, prop.length);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::Label &prop, const CommunicationHeaderObject &obj)
        {
            prop.resetData();
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.length = obj.data[1U];
            std::memcpy(prop.data, obj.data + 2U, prop.length);
        }

        /////////////// LineStyle ///////////////
        inline uint8_t numBytes(const properties::LineStyle &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::LineStyle &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(PropertyType::LINE_STYLE);
            obj.data[1U] = static_cast<uint8_t>(prop);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::LineStyle &prop, const CommunicationHeaderObject &obj)
        {
            prop = static_cast<properties::LineStyle>(obj.data[1U]);
        }

        /////////////// ScatterStyle ///////////////
        inline uint8_t numBytes(const properties::ScatterStyle &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::ScatterStyle &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(PropertyType::SCATTER_STYLE);
            obj.data[1U] = static_cast<uint8_t>(prop);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::ScatterStyle &prop,
                                                             const CommunicationHeaderObject &obj)
        {
            prop = static_cast<properties::ScatterStyle>(obj.data[1U]);
        }

        /////////////// ColorInternal ///////////////
        inline uint8_t numBytes(const internal::ColorInternal &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + 3U * sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const internal::ColorInternal &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1U] = prop.red;
            obj.data[2U] = prop.green;
            obj.data[3U] = prop.blue;
        }

        inline void deserializeFromCommunicationHeaderObject(internal::ColorInternal &prop,
                                                             const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.red = obj.data[1U];
            prop.green = obj.data[2U];
            prop.blue = obj.data[3U];
        }

        /////////////// EdgeColor ///////////////
        inline uint8_t numBytes(const properties::EdgeColor &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t) + 3U * sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::EdgeColor &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1U] = prop.red;
            obj.data[2U] = prop.green;
            obj.data[3U] = prop.blue;
            obj.data[4U] = prop.use_color;
        }

        inline void deserializeFromCommunicationHeaderObject(properties::EdgeColor &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.red = obj.data[1U];
            prop.green = obj.data[2U];
            prop.blue = obj.data[3U];
            prop.use_color = obj.data[4U];
        }

        /////////////// FaceColor ///////////////
        inline uint8_t numBytes(const properties::FaceColor &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t) + 3U * sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::FaceColor &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1U] = prop.red;
            obj.data[2U] = prop.green;
            obj.data[3U] = prop.blue;
            obj.data[4U] = prop.use_color;
        }

        inline void deserializeFromCommunicationHeaderObject(properties::FaceColor &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.red = obj.data[1U];
            prop.green = obj.data[2U];
            prop.blue = obj.data[3U];
            prop.use_color = obj.data[4U];
        }

        /////////////// Silhouette ///////////////
        inline uint8_t numBytes(const properties::Silhouette &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + 4U * sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::Silhouette &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1U] = prop.red;
            obj.data[2U] = prop.green;
            obj.data[3U] = prop.blue;
            obj.data[4U] = static_cast<std::uint8_t>(prop.percentage * 255.0);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::Silhouette &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.red = obj.data[1U];
            prop.green = obj.data[2U];
            prop.blue = obj.data[3U];
            prop.percentage = static_cast<float>(obj.data[4U]) / 255.0;
        }

        /////////////// ColorMap ///////////////
        inline uint8_t numBytes(const properties::ColorMap &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::ColorMap &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(PropertyType::COLOR_MAP);
            obj.data[1U] = static_cast<uint8_t>(prop);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::ColorMap &prop, const CommunicationHeaderObject &obj)
        {
            prop = static_cast<properties::ColorMap>(obj.data[1U]);
        }

        /////////////// PointSize ///////////////
        inline uint8_t numBytes(const properties::PointSize &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::PointSize &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            obj.data[1U] = prop.data;
        }

        inline void deserializeFromCommunicationHeaderObject(properties::PointSize &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            prop.data = obj.data[1U];
        }

        /////////////// DistanceFrom ///////////////
        inline uint8_t numBytes(const properties::DistanceFrom &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t)       // property_type_
                   + 3U * sizeof(double) // pt_
                   + sizeof(double)      // min_dist_
                   + sizeof(double)      // max_dist_
                   + sizeof(uint8_t);    // dist_from_type_
        }

        inline void serializeToCommunicationHeaderObject(::duoplot::internal::CommunicationHeaderObject &obj,
                                                         const properties::DistanceFrom &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());

            const Vec3<double> pt{prop.getPoint()};
            const double min_dist{prop.getMinDist()};
            const double max_dist{prop.getMaxDist()};
            const DistanceFromType dist_from_type{prop.getDistFromType()};

            size_t idx = 1U;

            std::memcpy(obj.data + idx, &(pt.x), sizeof(double));
            idx += sizeof(double);

            std::memcpy(obj.data + idx, &(pt.y), sizeof(double));
            idx += sizeof(double);

            std::memcpy(obj.data + idx, &(pt.z), sizeof(double));
            idx += sizeof(double);

            std::memcpy(obj.data + idx, &(min_dist), sizeof(double));
            idx += sizeof(double);

            std::memcpy(obj.data + idx, &(max_dist), sizeof(double));
            idx += sizeof(double);

            obj.data[idx] = static_cast<uint8_t>(dist_from_type);
        }

        inline void deserializeFromCommunicationHeaderObject(properties::DistanceFrom &prop,
                                                             const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));

            size_t idx = 1U;

            Vec3<double> pt{};
            double min_dist{};
            double max_dist{};
            DistanceFromType dist_from_type{};

            std::memcpy(&(pt.x), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(pt.y), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(pt.z), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(min_dist), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(max_dist), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            dist_from_type = static_cast<DistanceFromType>(obj.data[idx]);

            prop.setPoint(pt);
            prop.setMinDist(min_dist);
            prop.setMaxDist(max_dist);
            prop.setDistFromType(dist_from_type);
        }

        /////////////// BufferSize ///////////////
        inline uint8_t numBytes(const properties::BufferSize &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint16_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const properties::BufferSize &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(prop.getPropertyType());
            std::memcpy(obj.data + 1U, &(prop.data), sizeof(uint16_t));
        }

        inline void deserializeFromCommunicationHeaderObject(properties::BufferSize &prop, const CommunicationHeaderObject &obj)
        {
            prop.setPropertyType(static_cast<PropertyType>(obj.data[0U]));
            std::memcpy(&(prop.data), obj.data + 1U, sizeof(uint16_t));
        }

        /////////////// PropertyFlag ///////////////
        inline uint8_t numBytes(const internal::PropertyFlag &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint8_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const internal::PropertyFlag &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(PropertyType::PROPERTY_FLAG);
            obj.data[1U] = static_cast<uint8_t>(prop);
        }

        inline void deserializeFromCommunicationHeaderObject(internal::PropertyFlag &prop, const CommunicationHeaderObject &obj)
        {
            prop = static_cast<PropertyFlag>(obj.data[1U]);
        }

        /////////////// ItemId ///////////////
        inline uint8_t numBytes(const ItemId &obj)
        {
            static_cast<void>(obj);
            return sizeof(uint8_t) + sizeof(uint16_t);
        }

        inline void serializeToCommunicationHeaderObject(CommunicationHeaderObject &obj, const ItemId &prop)
        {
            obj.size = numBytes(prop);
            obj.data[0U] = static_cast<uint8_t>(PropertyType::ITEM_ID);
            std::memcpy(obj.data + 1U, &(prop), sizeof(uint16_t));
        }

        inline void deserializeFromCommunicationHeaderObject(ItemId &prop, const CommunicationHeaderObject &obj)
        {
            std::memcpy(&(prop), obj.data + 1U, sizeof(uint16_t));
        }

        /////////////// Others ///////////////
        inline void deserializeFromCommunicationHeaderObject(internal::Dimension2D &item, const CommunicationHeaderObject &obj)
        {
            std::memcpy(&(item.rows), obj.data, sizeof(uint32_t));
            std::memcpy(&(item.cols), obj.data + sizeof(uint32_t), sizeof(uint32_t));
        }

        inline void deserializeFromCommunicationHeaderObject(std::pair<Vec3<double>, Vec3<double>> &item,
                                                             const CommunicationHeaderObject &obj)
        {
            size_t idx = 0U;

            std::memcpy(&(item.first.x), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.first.y), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.first.z), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.second.x), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.second.y), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.second.z), obj.data + idx, sizeof(double));
            idx += sizeof(double);
        }

        inline void deserializeFromCommunicationHeaderObject(internal::DataType &item, const CommunicationHeaderObject &obj)
        {
            item = static_cast<internal::DataType>(obj.data[0U]);
        }

        inline void deserializeFromCommunicationHeaderObject(Vec3<double> &item, const CommunicationHeaderObject &obj)
        {
            size_t idx = 0U;

            std::memcpy(&(item.x), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.y), obj.data + idx, sizeof(double));
            idx += sizeof(double);

            std::memcpy(&(item.z), obj.data + idx, sizeof(double));
            idx += sizeof(double);
        }

        inline void deserializeFromCommunicationHeaderObject(MatrixFixed<double, 3, 3> &item,
                                                             const CommunicationHeaderObject &obj)
        {
            size_t idx = 0U;

            for (size_t r = 0; r < 3; r++)
            {
                for (size_t c = 0; c < 3; c++)
                {
                    std::memcpy(&(item(r, c)), obj.data + idx, sizeof(double));
                    idx += sizeof(double);
                }
            }
        }

    } // namespace internal
} // namespace duoplot

#endif // DUOPLOT_ENCODE_DECODE_FUNCTIONS_H
