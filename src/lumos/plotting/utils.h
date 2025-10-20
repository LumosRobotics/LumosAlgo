#ifndef DUOPLOT_UTILS_H_
#define DUOPLOT_UTILS_H_

#include <stdlib.h>

namespace lumos
{
    namespace internal
    {
        template <typename U>
        constexpr size_t totalSizeOfObjects(__attribute__((unused)) const U &obj)
        {
            return sizeof(U);
        }

        template <typename U, typename... Us>
        constexpr size_t totalSizeOfObjects(__attribute__((unused)) const U &obj, const Us &...objects)
        {
            return sizeof(U) + totalSizeOfObjects(objects...);
        }

        template <typename U>
        void fillBufferWithObjectsInternal(size_t idx, uint8_t *const buffer, const U &obj)
        {
            const uint8_t *const obj_ptr = reinterpret_cast<const uint8_t *const>(&obj);

            // TODO: Use memcpy
            for (size_t k = 0; k < sizeof(U); k++)
            {
                buffer[idx] = obj_ptr[k];
                idx++;
            }
        }

        template <typename U, typename... Us>
        void fillBufferWithObjectsInternal(size_t idx, uint8_t *const buffer, const U &obj, const Us &...objects)
        {
            const uint8_t *const obj_ptr = reinterpret_cast<const uint8_t *const>(&obj);

            // TODO: Use memcpy
            for (size_t k = 0; k < sizeof(U); k++)
            {
                buffer[idx] = obj_ptr[k];
                idx++;
            }
            fillBufferWithObjectsInternal(idx, buffer, objects...);
        }

        template <typename... Us>
        void fillBufferWithObjects(uint8_t *const buffer, const Us &...objects)
        {
            size_t idx = 0;
            fillBufferWithObjectsInternal(idx, buffer, objects...);
        }

        template <typename U>
        void fillObjectsFromBufferInternal(size_t idx, const uint8_t *const buffer, U &obj)
        {
            uint8_t *const obj_ptr = reinterpret_cast<uint8_t *const>(&obj);

            // TODO: Use memcpy
            for (size_t k = 0; k < sizeof(U); k++)
            {
                obj_ptr[k] = buffer[idx];
                idx++;
            }
        }

        template <typename U, typename... Us>
        void fillObjectsFromBufferInternal(size_t idx, const uint8_t *const buffer, U &obj, Us &...objects)
        {
            uint8_t *const obj_ptr = reinterpret_cast<uint8_t *const>(&obj);

            for (size_t k = 0; k < sizeof(U); k++)
            {
                obj_ptr[k] = buffer[idx];
                idx++;
            }
            fillObjectsFromBufferInternal(idx, buffer, objects...);
        }

        template <typename... Us>
        void fillObjectsFromBuffer(const uint8_t *const buffer, Us &...objects)
        {
            size_t idx = 0;
            fillObjectsFromBufferInternal(idx, buffer, objects...);
        }

        inline uint8_t isBigEndian()
        {
            const uint32_t x = 1;
            const uint8_t *const ptr = reinterpret_cast<const uint8_t *const>(&x);

            if (ptr[0] == '\x01')
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }

        inline bool ackValid(const char data[256])
        {
            const bool ack_received = data[0] == 'a' && data[1] == 'c' && data[2] == 'k' && data[3] == '#' && data[4] == '\0';
            return ack_received;
        }

    } // namespace internal
} // namespace lumos

#endif // DUOPLOT_UTILS_H_
