#pragma once

#include "iterator_helpers.hpp"

#include <iterator>
#include <type_traits>

namespace adaptor
{
namespace detail
{

template <typename Range, typename Predicate> 
struct range_filter;


template <typename Range, typename Predicate>
struct range_filter_iterator 
    : public std::iterator<
        std::forward_iterator_tag,
        value_type_t<Range>,
        difference_type_t<Range>
      >
{
private:

    using range_type        = typename std::remove_reference<Range>::type;
    using self_type         = range_filter_iterator<Range, Predicate>;
    using range_filter_type = range_filter<Range, Predicate>;
    using base_iterator     = typename range_type::iterator;

public:

    using reference = typename std::iterator_traits<base_iterator>::reference;

    range_filter_iterator(range_filter_type& r, base_iterator where)
        : parent_(r),
          current_(where)
    { }

    reference operator*() 
    {
        while (!parent_.func_(*current_)) { ++current_; }
        return *current_;
    }

    self_type& operator++()
    {
        ++current_;
        while (current_ != parent_.range_.end() && !parent_.func_(*current_)) { 
            ++current_; 
        }
        return *this;
    }

    self_type& operator--()
    {
        --current_;
        while (current_ != parent_.range_.begin() && !parent_.func_(*current_)) { 
            --current_; 
        }
        return *this;
    }

    bool equals(self_type other) const
    {
        return std::addressof(parent_) == std::addressof(other.parent_) &&
            current_ == other.current_;
    }

private:

    range_filter_type& parent_;
    base_iterator      current_;
};

template <typename Range, typename Predicate>
bool operator==(
    range_filter_iterator<Range, Predicate> r1, range_filter_iterator<Range, Predicate> r2
)
{
    return r1.equals(r2);
}

template <typename Range, typename Predicate>
bool operator!=(
    range_filter_iterator<Range, Predicate> r1, range_filter_iterator<Range, Predicate> r2
)
{
    return !operator==(r1, r2);
}

template <typename Range, typename Predicate>
struct range_filter
{
    friend struct range_filter_iterator<Range, Predicate>;

    using base_iterator = typename std::remove_reference_t<Range>::iterator;
    using base_value = typename std::iterator_traits<base_iterator>::value_type;

public:

    using iterator   = range_filter_iterator<Range, Predicate>;
    using value_type = typename std::remove_reference_t<Range>::value_type;
    using reference  = typename std::remove_reference_t<Range>::reference;

    range_filter(Range&& r, Predicate func)
        : range_(std::forward<Range>(r)),
          func_(func)
    { }

    iterator begin()
    {
        return iterator(*this, range_.begin());
    }

    iterator end()
    {
        return iterator(*this, range_.end());
    }

private:

    Range&&    range_;
    Predicate  func_;
};

template <typename Predicate>
struct inner_filter
{
    Predicate p_;

    inner_filter(Predicate p)
        : p_(p)
    { }

    template <typename Range>
    auto operator()(Range&& r)
    {
        return detail::range_filter<Range, Predicate>(
            std::forward<Range>(r), p_
        );
    }
};

} // end namespace detail

// This should work, but MSVC doesn't like it for some reason.
// Instead, use the variant returning detail::inner_transform below.

/*
template <typename UnaryFunc>
auto range_transform(UnaryFunc f)
{
    return [f](auto& range)
    {
        using range_type = std::remove_reference_t<decltype(range)>;
        using mapping_type = detail::range_map<range_type, UnaryFunc>;
        return mapping_type(range, f);
    };
}
*/

template <typename Predicate>
detail::inner_filter<Predicate> filter(Predicate f)
{
    return detail::inner_filter<Predicate>(f);
}

template <typename Range, typename Predicate>
auto operator|(Range&& c, detail::inner_filter<Predicate> inner)
{
    return inner(std::forward<Range>(c));
}

} // end namespace adaptor
