#ifndef DUOPLOT_MATH_MISC_MATH_MACROS_H_
#define DUOPLOT_MATH_MISC_MATH_MACROS_H_

#include "lumos/plotting/logging.h"

namespace duoplot
{
#define DATA_ALLOCATION(data, data_size, data_type, alloc_type)                      \
    try                                                                              \
    {                                                                                \
        data = new data_type[data_size];                                             \
    }                                                                                \
    catch (std::bad_alloc & ba)                                                      \
    {                                                                                \
        std::cerr << alloc_type << " allocation failed: " << ba.what() << std::endl; \
        exit(-1);                                                                    \
    }

#define ASSERT_MAT_VALID_INTERNAL()                          \
    DUOPLOT_ASSERT(num_rows_ > 0) << "Number of rows is 0!"; \
    DUOPLOT_ASSERT(num_cols_ > 0) << "Number of cols is 0!"

#define ASSERT_VEC_VALID_INTERNAL() DUOPLOT_ASSERT(size_ > 0) << "Number of elements is 0!"

} // namespace duoplot

#endif // DUOPLOT_MATH_MISC_MATH_MACROS_H_
