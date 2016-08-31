#pragma once

#include <iterator>
#include <type_traits>

namespace adaptor
{
namespace detail
{

template <typename Range, typename UnaryFunc>
struct range_map;

template <typename Range>
using iterator_category_t = typename std::iterator_traits<typename Range::iterator>::iterator_category;

template <typename Range>
using difference_type_t = typename std::iterator_traits<typename Range::iterator>::difference_type;

template <typename Range, typename UnaryFunc>
struct range_map_iterator
    : public std::iterator<
        iterator_category_t<Range>,
        typename range_map<Range, UnaryFunc>::value_type,
        difference_type_t<Range>
      >
{
private:

    using self_type      = range_map_iterator<Range, UnaryFunc>;
    using range_map_type = range_map<Range, UnaryFunc>;
    using base_iterator  = typename Range::iterator;

public:

    range_map_iterator(range_map_type& r, base_iterator where)
        : parent_(r),
          current_(where)
    { }

    value_type operator*() 
    {
        return parent_.func_(*current_);
    }

    self_type& operator++()
    {
        ++current_;
        return *this;
    }

    self_type& operator--()
    {
        --current_;
        return *this;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type 
    operator+(std::size_t n)
    {
        current_ += n;
        return *this;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type 
    operator-(std::size_t n)
    {
        current_ -= n;
        return *this;
    }

    bool equals(self_type other) const
    {
        return std::addressof(parent_) == std::addressof(other.parent_) &&
            current_ == other.current_;
    }

private:

    range_map_type& parent_;
    base_iterator   current_;
};

template <typename Range, typename UnaryFunc>
bool operator==(
    range_map_iterator<Range, UnaryFunc> r1, range_map_iterator<Range, UnaryFunc> r2
)
{
    return r1.equals(r2);
}

template <typename Range, typename UnaryFunc>
bool operator!=(
    range_map_iterator<Range, UnaryFunc> r1, range_map_iterator<Range, UnaryFunc> r2
)
{
    return !operator==(r1, r2);
}

template <typename Range, typename UnaryFunc>
struct range_map
{
    friend struct range_map_iterator<Range, UnaryFunc>;

    using base_iterator = typename Range::iterator;
    using base_value = typename std::iterator_traits<base_iterator>::value_type;
    using return_type = std::result_of_t<UnaryFunc(base_value)>;

public:

    using iterator = range_map_iterator<Range, UnaryFunc>;
    using value_type = std::remove_reference_t<return_type>;
    using reference = std::add_lvalue_reference_t<value_type>;

    range_map(Range& r, UnaryFunc func)
        : range_(r),
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

    Range&    range_;
    UnaryFunc func_;
};

template <typename UnaryFunc>
struct inner_transform
{
    UnaryFunc f_;

    inner_transform(UnaryFunc f)
        : f_(f)
    { }

    template <typename Range>
    auto operator()(Range& r)
    {
        return detail::range_map<Range, UnaryFunc>(r, f_);
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

template <typename UnaryFunc>
detail::inner_transform<UnaryFunc> range_transform(UnaryFunc f)
{

    return detail::inner_transform<UnaryFunc>(f);
}

template <typename Range, typename UnaryFunc>
auto operator|(Range& c, detail::inner_transform<UnaryFunc> inner)
{
    return inner(c);
}

} // end namespace adaptor