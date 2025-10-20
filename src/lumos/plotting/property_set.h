#ifndef DUOPLOT_PROPERTY_STRUCTURE_
#define DUOPLOT_PROPERTY_STRUCTURE_

#include <stdint.h>

#include "lumos/plotting/communication_header.h"
#include "lumos/plotting/item_id.h"

namespace duoplot
{

    /*
    setProperties({properties::ID0, transform0, properties::Color::RED},
    {properties::ID1, transform1, properties::Color::GREEN},
    {properties::ID2, transform2, properties::Color::BLUE},
    {properties::ID3, transform3, properties::Color::YELLOW})
    */

    class PropertySet
    {
    private:
        ItemId id_;
        std::vector<internal::CommunicationHeaderObject> props_;

        template <typename U>
        void append(const U &prop)
        {
            props_.push_back(internal::CommunicationHeaderObject{internal::CommunicationHeaderObjectType::PROPERTY});
            internal::CommunicationHeaderObject &current_obj = props_.back();

            serializeToCommunicationHeaderObject(current_obj, prop);
        }

        void extendInternal()
        {
            // To catch when there are no arguments to extend
        }

        template <typename U>
        void extendInternal(const U &prop)
        {
            append(prop);
        }

        template <typename U, typename... Us>
        void extendInternal(const U &prop, const Us &...props)
        {
            append(prop);
            extendInternal(props...);
        }

    public:
        PropertySet() : id_{ItemId::UNKNOWN} {}
        template <typename... Us>
        PropertySet(const ItemId id, const Us &...props) : id_{id}
        {
            extendInternal(props...);
        }

        template <typename... Us>
        PropertySet(const Us &...props) : id_{ItemId::UNKNOWN}
        {
            extendInternal(props...);
        }

        std::size_t getTotalSize() const
        {
            std::size_t total_size{0U};
            for (size_t k = 0; k < props_.size(); k++)
            {
                total_size += props_[k].size;
            }

            // +1 for number of properties in this property set
            // +sizeof(ItemId) for id
            // +props_.size() for size of each property
            return total_size + 1U + sizeof(ItemId) + props_.size();
        }

        void fillBuffer(std::uint8_t *const data_to_fill) const
        {
            data_to_fill[0U] = static_cast<std::uint8_t>(props_.size());
            std::memcpy(data_to_fill + 1U, reinterpret_cast<const std::uint8_t *const>(&id_), sizeof(ItemId));
            std::size_t idx{1U + sizeof(ItemId)};

            for (size_t k = 0; k < props_.size(); k++)
            {
                data_to_fill[idx] = props_[k].size;
                idx += 1U;

                std::memcpy(data_to_fill + idx, props_[k].data, props_[k].size);

                idx += props_[k].size;
            }
        }
    };

} // namespace duoplot

#endif // DUOPLOT_PROPERTY_STRUCTURE_