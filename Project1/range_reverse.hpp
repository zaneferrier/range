#pragma once

#include "iterator_helpers.hpp"

#include <iterator>
#include <type_traits>

namespace adaptor
{
namespace detail
{

//================================================================================

template <typename Range>
struct range_reverse_iterator 
    : public std::iterator<
        iterator_category_t<Range>,
        typename Range::value_type
      >
{
private:

    using range_type    = typename std::remove_reference<Range>::type;
    using self_type     = range_reverse_iterator<Range>;
    using base_iterator = typename range_type::iterator;

public:

    using value_type        = typename range_type::value_type;
    using reference         = value_type&;
    using iterator_category = iterator_category_t<Range>;

    range_reverse_iterator(base_iterator where)
        : current_(where)
    { }

    auto operator*() 
    {
        return *current_;
    }

    auto operator*() const
    {
        return *current_;
    }

    self_type& operator++()
    {
        --current_;
        return *this;
    }

    self_type operator++(int)
    {
        self_type ret(*this);
        --current_;
        return ret;
    }

    template <typename T = self_type>
    typename std::enable_if<
        std::is_same<std::random_access_iterator_tag, iterator_category>::value 
    >::type operator+(std::ptrdiff_t n)
    {
        self_type ret(*this);
        current_ -= n;
        return ret;
    }
    
    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<std::random_access_iterator_tag, iterator_category>::value
    >::type operator+=(std::ptrdiff_t n)
    {
        current_ -= n;
        return *this;
    }

    self_type& operator--()
    {
        ++current_;
        return *this;
    }

    self_type operator--(int)
    {
        self_type ret(*this);
        ++current_;
        return ret;
    }
   
    bool equals(self_type other) const
    {
        return current_ == other.current_;
    }

private:

    base_iterator current_;
};

//================================================================================

template <typename Range>
bool operator==(
    range_reverse_iterator<Range> r1, range_reverse_iterator<Range> r2
)
{
    return r1.equals(r2);
}

template <typename Range>
bool operator!=(
    range_reverse_iterator<Range> r1, range_reverse_iterator<Range> r2
)
{
    return !operator==(r1, r2);
}

//================================================================================

template <typename Range>
struct range_reverse
{
private:

    using range_type = typename std::remove_reference_t<Range>;

public:

    using iterator   = range_reverse_iterator<range_type>;
    using reference  = typename range_type::reference;
    using value_type = typename range_type::value_type;

    range_reverse(Range&& c)
        : range_(std::forward<Range>(c))
    { }

    ~range_reverse() = default;

    range_reverse(const range_reverse& other) = default;
    range_reverse(range_reverse&& other) = default;

    iterator begin()
    {
        auto end = range_.end();
        --end;
        return iterator(end);
    }

    iterator end()
    {
        auto begin = range_.begin();
        --begin;
        return iterator(begin);
    }

private:
    
    Range&& range_;
};

} // end namespace detail

auto reverse()
{
    return [](auto&& container) -> detail::range_reverse<decltype(container)>
    { 
        return detail::range_reverse<decltype(container)>(
            std::forward<decltype(container)>(container)
        ); 
    };
}

template <typename Range>
detail::range_reverse<Range> unique(Range&& c)
{
    return detail::range_reverse<Range>(std::forward<Range>(c));
}

template <typename Range>
auto operator|(Range&& c, decltype(reverse()) adaptor)
{
    return adaptor(std::forward<Range>(c));
}

} // end namespace adaptor
