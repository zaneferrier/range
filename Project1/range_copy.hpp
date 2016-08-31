#pragma once

#include <iterator>

namespace adaptor
{
namespace detail
{

template <typename Range>
struct range_slice
{
private:

    using range_type = typename std::remove_reference_t<Range>;

public:

    using iterator   = typename range_type::iterator;
    using reference  = typename range_type::reference;
    using value_type = typename range_type::value_type;

    range_slice(Range&& c, std::size_t from, std::size_t to)
        : begin_(c.begin() + from),
          end_(c.begin() + to)
    { }

    ~range_slice() = default;

    iterator begin()
    {
        return begin_;
    }

    iterator end()
    {
        return end_;
    }

private:
    
    iterator begin_;
    iterator end_;
};

} // end namespace detail

auto slice(std::size_t from, std::size_t to)
{
    return [from, to](auto&& container) 
    { 
        return detail::range_slice<decltype(container)>(
            std::forward<decltype(container)>(container), from, to
        ); 
    };
}

template <typename Range>
detail::range_slice<Range> slice(Range&& c, std::size_t from, std::size_t to)
{
    using iterator_type = typename Range::iterator;

    static_assert(
        std::is_same<
            typename std::iterator_traits<iterator_type>::iterator_category,
            std::random_access_iterator_tag
        >::value,
        "Must have random access iterators for range_copy!"
    );

    return detail::range_slice<Range>(std::forward<Range>(c), from, to);
}

template <typename Range>
auto operator|(Range&& c, decltype(slice(0, 0)) adaptor)
{
    return adaptor(std::forward<Range>(c));
}

} // end namespace adaptor
