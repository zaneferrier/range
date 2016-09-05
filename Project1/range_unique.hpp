#pragma once

#include "iterator_helpers.hpp"

#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace adaptor
{
namespace detail
{

template <typename Range>
struct range_unique;

template <typename Range>
struct range_unique_iterator
    : public std::iterator<
        std::forward_iterator_tag,
        typename Range::value_type
      >
{
private:

    using range_type    = typename std::remove_reference<Range>::type;
    using self_type     = range_unique_iterator<Range>;
    using base_iterator = typename range_type::iterator;

public:

    using value_type        = typename range_type::value_type;
    using reference         = value_type&;
    using iterator_category = iterator_category_t<Range>;

    range_unique_iterator(base_iterator where, base_iterator end)
        : current_(where),
          end_(end)
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
        if(current_ == end_) { return *this; }
        const value_type& value = *current_;
        ++current_;
        while(current_ != end_ && value == *current_) { ++current_; }
        return *this;
    }

    self_type operator++(int)
    {
        self_type ret(*this);
        if(current_ == end_) { return ret; }
        const value_type& value = *current_;
        ++current_;
        while(current_ != end_ && value == *current_) { ++current_; }
        return ret;
    }
   
    bool equals(self_type other) const
    {
        return current_ == other.current_;
    }

private:

    base_iterator current_;
    base_iterator end_;
};

//================================================================================

template <typename Range>
bool operator==(
    range_unique_iterator<Range> r1, range_unique_iterator<Range> r2
)
{
    return r1.equals(r2);
}

template <typename Range>
bool operator!=(
    range_unique_iterator<Range> r1, range_unique_iterator<Range> r2
)
{
    return !operator==(r1, r2);
}

//================================================================================

template <typename Range>
struct range_unique 
{
private:

    using range_type = typename std::remove_reference_t<Range>;

public:

    using iterator   = range_unique_iterator<range_type>;
    using reference  = typename range_type::reference;
    using value_type = typename range_type::value_type;

    range_unique(Range&& c)
        : range_(std::forward<Range>(c))
    { }

    ~range_unique() = default;

    range_unique(const range_unique& other) = default;
    range_unique(range_unique&& other) = default;

    iterator begin()
    {
        return iterator(range_.begin(), range_.end());
    }

    iterator end()
    {
        return iterator(range_.end(), range_.end());
    }

private:
    
    Range&& range_;
};

} // end namespace detail

auto unique()
{
    return [](auto&& container) -> detail::range_unique<decltype(container)>
    { 
        return detail::range_unique<decltype(container)>(
            std::forward<decltype(container)>(container)
        ); 
    };
}

template <typename Range>
detail::range_unique<Range> unique(Range&& c)
{
    return detail::range_unique<Range>(std::forward<Range>(c));
}

template <typename Range>
auto operator|(Range&& c, decltype(unique()) adaptor)
{
    return adaptor(std::forward<Range>(c));
}

} // end namespace adaptor
