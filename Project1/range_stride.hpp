#pragma once

#include <iterator>
#include <stdexcept>
#include <type_traits>

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
struct range_stride;

template <typename Range>
struct range_stride_iterator
    : public std::iterator<
        iterator_category_t<Range>, 
        typename Range::value_type
      >
{
private:

    using range_type        = typename std::remove_reference<Range>::type;
    using self_type         = range_stride_iterator<Range>;
    using range_stride_type = range_stride<Range>;
    using base_iterator     = typename range_type::iterator;

public:

    using value_type        = typename range_type::value_type;
    using reference         = value_type&;
    using iterator_category = iterator_category_t<Range>;

    range_stride_iterator(
        base_iterator where, base_iterator end, std::size_t stride
    )
        : current_(where),
          end_(end),
          stride_(stride)
    { }

    reference operator*() 
    {
        return *current_;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::bidirectional_iterator_tag>::value,
        T
    >::type operator++()
    {
        auto i = 0U;
        while(i < stride_ && current_ != end_) { ++current_; ++i; }
        return *this;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type operator++()
    {
        if(current_ + stride_ > end_) { current_ = end_; }
        else { current_ += stride_; }
        return *this;
    }

    template <typename T = self_type>
    typename std::enable_if<
        std::is_same<iterator_category, std::bidirectional_iterator_tag>::value,
        T
    >::type operator++(int)
    {
        self_type ret(*this);
        auto i = 0U;
        while(i < stride_ && current_ < end_) { ++current_; ++i; }
        return ret;
    }
   
    template <typename T = self_type>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type operator++(int)
    {
        self_type ret(*this);
        if(current_ + stride_ > end_) { current_ = end_; }
        else { current_ += stride_; }
        return ret;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::bidirectional_iterator_tag>::value,
        T
    >::type operator--()
    {
        for(auto i = 0U; i < stride_; ++i) {
            --current_;
        }
        return *this;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type operator--()
    {
        current_ -= stride_;
        return *this;
    }

    template <typename T = self_type>
    typename std::enable_if<
        std::is_same<iterator_category, std::bidirectional_iterator_tag>::value,
        T
    >::type operator--(int)
    {
        self_type ret(*this);
        for(auto i = 0U; i < stride_; ++i) {
            --current_;
        }
        return ret;
    }

    template <typename T = self_type>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type operator--(int)
    {
        self_type ret(*this);
        current_ -= stride_;
        return ret;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type operator+(std::size_t n)
    {
        current_ += (stride_ * n);
        return *this;
    }

    template <typename T = self_type&>
    typename std::enable_if<
        std::is_same<iterator_category, std::random_access_iterator_tag>::value,
        T
    >::type operator-(std::size_t n)
    {
        current_ -= (stride_ * n);
        return *this;
    }

    bool equals(self_type other) const
    {
        return current_ == other.current_;
    }

private:

    // Note: I should use an actual value to keep track of how many
    // "past" the end I end up, so that operator-- and operator-=
    // will work correctly (they currently don't).
    
    base_iterator   current_;
    base_iterator   end_;
    std::size_t     stride_;
};

//================================================================================

template <typename Range>
bool operator==(
    range_stride_iterator<Range> r1, range_stride_iterator<Range> r2
)
{
    return r1.equals(r2);
}

template <typename Range>
bool operator!=(
    range_stride_iterator<Range> r1, range_stride_iterator<Range> r2
)
{
    return !operator==(r1, r2);
}

//================================================================================

template <typename Range>
struct range_stride
{
private:

    using range_type = typename std::remove_reference_t<Range>;

public:

    using iterator   = range_stride_iterator<range_type>;
    using reference  = typename range_type::reference;
    using value_type = typename range_type::value_type;

    range_stride(Range&& c, std::size_t stride)
        : range_(std::forward<Range>(c)),
          stride_(stride)
    { 
        if(stride_ == 0) {
            throw std::invalid_argument("Stride value must be > 0!");
        }
    }

    ~range_stride() = default;

    iterator begin()
    {
        return iterator(range_.begin(), range_.end(), stride_);
    }

    iterator end()
    {
        return iterator(range_.end(), range_.end(), stride_);
    }

private:
    
    Range&&     range_;
    std::size_t stride_;    
};

} // end namespace detail


auto stride(std::size_t stride)
{
    return [stride](auto&& container) 
    { 
        return detail::range_stride<decltype(container)>(
            std::forward<decltype(container)>(container), stride
        ); 
    };
}

template <typename Range>
detail::range_stride<Range> stride(Range&& c, std::size_t stride)
{
    using iterator_type = typename Range::iterator;

    static_assert(
        std::is_base_of<
            std::bidirectional_iterator_tag,
            typename std::iterator_traits<iterator_type>::iterator_category
        >::value,
        "Must have at least bidirectional iterators for range stride!"
    );

    return detail::range_stride<Range>(std::forward<Range>(c), stride);
}

template <typename Range>
auto operator|(Range&& c, decltype(stride(1)) adaptor)
{
    return adaptor(std::forward<Range>(c));
}

} // end namespace adaptor
