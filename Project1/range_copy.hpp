#pragma once

#include <iterator>

namespace adaptor
{
namespace detail
{

template <typename Range>
struct range_slice
{
public:

    using iterator = typename Range::iterator;
    using reference = typename Range::reference;

    range_slice(Range& c, std::size_t from, std::size_t to)
        : begin_(c.begin() + from),
          end_(c.begin() + to)
    { }

    ~range_slice() = default;

    reference operator*()
    {
        return *begin_;
    }

    const reference operator*() const
    {
        return *begin_;
    }

    range_slice& operator++()
    {
        ++begin_;
        return *this;
    }

    range_slice operator++(int)
    {
        range_slice cpy(*this);
        ++(*this);
        return cpy;
    }

    range_slice& operator+=(std::size_t n)
    {
        begin_ += n;
        return *this;
    }

    range_slice& operator-=(std::size_t n)
    {
        begin_ -= n;
        return *this;
    }

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

auto range_copy(std::size_t from, std::size_t to)
{
    return [from, to](auto& container) 
    { 
        using container_type = std::remove_reference_t<decltype(container)>;
        return detail::range_slice<container_type>(container, from, to); 
    };
}

template <typename Range>
detail::range_slice<Range> range_copy(Range& c, std::size_t from, std::size_t to)
{
    using iterator_type = typename Range::iterator;

    static_assert(
        std::is_same<
            typename std::iterator_traits<iterator_type>::iterator_category,
            std::random_access_iterator_tag
        >::value,
        "Must have random access iterators for range_copy!"
    );

    return detail::range_slice<Container>(c, from, to);
}

template <typename Range> 
detail::range_slice<Range> operator|(Range& c, decltype(range_copy(0, 0)) adaptor)
{
    return adaptor(c);
}

/*template <typename Container, typename OtherAdaptor>
auto operator|(detail::range_slice<Container> c, OtherAdaptor other)
{
    return detail::make_adaptor_chain(std::move(c), std::move(other));
}*/

} // end namespace adaptor
