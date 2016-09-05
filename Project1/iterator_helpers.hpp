#pragma once

namespace adaptor
{
namespace detail
{

template <typename Range>
using iterator_category_t = 
    typename std::iterator_traits<
        typename std::remove_reference<Range>::type
    ::iterator>::iterator_category;

template <typename Range>
using difference_type_t = 
    typename std::iterator_traits<
        typename std::remove_reference<Range>::type
    ::iterator>::difference_type;

template <typename Range>
using value_type_t =
    typename std::iterator_traits<
        typename std::remove_reference<Range>::type
    ::iterator>::value_type;

} // end namespace detail
} // end namespace adaptor
