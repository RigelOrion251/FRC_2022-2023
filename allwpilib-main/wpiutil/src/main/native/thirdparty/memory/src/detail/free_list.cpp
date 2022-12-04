// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "wpi/memory/detail/free_list.hpp"

#include "wpi/memory/detail/align.hpp"
#include "wpi/memory/detail/debug_helpers.hpp"
#include "wpi/memory/detail/assert.hpp"
#include "wpi/memory/debugging.hpp"
#include "wpi/memory/error.hpp"

#include "free_list_utils.hpp"

using namespace wpi::memory;
using namespace detail;

namespace
{
    // i.e. array
    struct interval
    {
        char* prev;  // last before
        char* first; // first in
        char* last;  // last in
        char* next;  // first after

        // number of nodes in the interval
        std::size_t size(std::size_t node_size) const noexcept
        {
            // last is inclusive, so add actual_size to it
            // note: cannot use next, might not be directly after
            auto end = last + node_size;
            WPI_MEMORY_ASSERT(static_cast<std::size_t>(end - first) % node_size == 0u);
            return static_cast<std::size_t>(end - first) / node_size;
        }
    };

    // searches for n consecutive bytes
    // begin and end are the proxy nodes
    // assumes list is not empty
    // similar to list_search_array()
    interval list_search_array(char* first, std::size_t bytes_needed,
                               std::size_t node_size) noexcept
    {
        interval i;
        i.prev  = nullptr;
        i.first = first;
        // i.last/next are used as iterator for the end of the interval
        i.last = first;
        i.next = list_get_next(first);

        auto bytes_so_far = node_size;
        while (i.next)
        {
            if (i.last + node_size != i.next) // not continous
            {
                // restart at next
                i.prev  = i.last;
                i.first = i.next;
                i.last  = i.next;
                i.next  = list_get_next(i.last);

                bytes_so_far = node_size;
            }
            else
            {
                // extend interval
                auto new_next = list_get_next(i.next);
                i.last        = i.next;
                i.next        = new_next;

                bytes_so_far += node_size;
                if (bytes_so_far >= bytes_needed)
                    return i;
            }
        }
        // not enough continuous space
        return {nullptr, nullptr, nullptr, nullptr};
    }

    // similar to list_search_array()
    // begin/end are proxy nodes
    interval xor_list_search_array(char* begin, char* end, std::size_t bytes_needed,
                                   std::size_t node_size) noexcept
    {
        interval i;
        i.prev  = begin;
        i.first = xor_list_get_other(begin, nullptr);
        // i.last/next are used as iterator for the end of the interval
        i.last = i.first;
        i.next = xor_list_get_other(i.last, i.prev);

        auto bytes_so_far = node_size;
        while (i.next != end)
        {
            if (i.last + node_size != i.next) // not continous
            {
                // restart at i.next
                i.prev  = i.last;
                i.first = i.next;
                i.last  = i.next;
                i.next  = xor_list_get_other(i.first, i.prev);

                bytes_so_far = node_size;
            }
            else
            {
                // extend interval
                auto new_next = xor_list_get_other(i.next, i.last);
                i.last        = i.next;
                i.next        = new_next;

                bytes_so_far += node_size;
                if (bytes_so_far >= bytes_needed)
                    return i;
            }
        }
        // not enough continuous space
        return {nullptr, nullptr, nullptr, nullptr};
    }
} // namespace

constexpr std::size_t free_memory_list::min_element_size;
constexpr std::size_t free_memory_list::min_element_alignment;

free_memory_list::free_memory_list(std::size_t node_size) noexcept
: first_(nullptr),
  node_size_(node_size > min_element_size ? node_size : min_element_size),
  capacity_(0u)
{
}

free_memory_list::free_memory_list(std::size_t node_size, void* mem, std::size_t size) noexcept
: free_memory_list(node_size)
{
    insert(mem, size);
}

free_memory_list::free_memory_list(free_memory_list&& other) noexcept
: first_(other.first_), node_size_(other.node_size_), capacity_(other.capacity_)
{
    other.first_    = nullptr;
    other.capacity_ = 0u;
}

free_memory_list& free_memory_list::operator=(free_memory_list&& other) noexcept
{
    free_memory_list tmp(detail::move(other));
    swap(*this, tmp);
    return *this;
}

void wpi::memory::detail::swap(free_memory_list& a, free_memory_list& b) noexcept
{
    detail::adl_swap(a.first_, b.first_);
    detail::adl_swap(a.node_size_, b.node_size_);
    detail::adl_swap(a.capacity_, b.capacity_);
}

void free_memory_list::insert(void* mem, std::size_t size) noexcept
{
    WPI_MEMORY_ASSERT(mem);
    WPI_MEMORY_ASSERT(is_aligned(mem, alignment()));
    detail::debug_fill_internal(mem, size, false);

    insert_impl(mem, size);
}

void* free_memory_list::allocate() noexcept
{
    WPI_MEMORY_ASSERT(!empty());
    --capacity_;

    auto mem = first_;
    first_   = list_get_next(first_);
    return detail::debug_fill_new(mem, node_size_, 0);
}

void* free_memory_list::allocate(std::size_t n) noexcept
{
    WPI_MEMORY_ASSERT(!empty());
    if (n <= node_size_)
        return allocate();

    auto i = list_search_array(first_, n, node_size_);
    if (i.first == nullptr)
        return nullptr;

    if (i.prev)
        list_set_next(i.prev, i.next); // change next from previous to first after
    else
        first_ = i.next;
    capacity_ -= i.size(node_size_);

    return detail::debug_fill_new(i.first, n, 0);
}

void free_memory_list::deallocate(void* ptr) noexcept
{
    ++capacity_;

    auto node = static_cast<char*>(detail::debug_fill_free(ptr, node_size_, 0));
    list_set_next(node, first_);
    first_ = node;
}

void free_memory_list::deallocate(void* ptr, std::size_t n) noexcept
{
    if (n <= node_size_)
        deallocate(ptr);
    else
    {
        auto mem = detail::debug_fill_free(ptr, n, 0);
        insert_impl(mem, n);
    }
}

std::size_t free_memory_list::alignment() const noexcept
{
    return alignment_for(node_size_);
}

void free_memory_list::insert_impl(void* mem, std::size_t size) noexcept
{
    auto no_nodes = size / node_size_;
    WPI_MEMORY_ASSERT(no_nodes > 0);

    auto cur = static_cast<char*>(mem);
    for (std::size_t i = 0u; i != no_nodes - 1; ++i)
    {
        list_set_next(cur, cur + node_size_);
        cur += node_size_;
    }
    list_set_next(cur, first_);
    first_ = static_cast<char*>(mem);

    capacity_ += no_nodes;
}

namespace
{
    // converts a block into a linked list
    void xor_link_block(void* memory, std::size_t node_size, std::size_t no_nodes, char* prev,
                        char* next) noexcept
    {
        auto cur = static_cast<char*>(memory);
        xor_list_change(prev, next, cur); // change next pointer of prev

        auto last_cur = prev;
        for (std::size_t i = 0u; i != no_nodes - 1; ++i)
        {
            xor_list_set(cur, last_cur,
                         cur + node_size); // cur gets last_cur and next node in continous memory
            last_cur = cur;
            cur += node_size;
        }
        xor_list_set(cur, last_cur, next); // last memory node gets next as next
        xor_list_change(next, prev, cur);  // change prev pointer of next
    }

    struct pos
    {
        char *prev, *next;
    };

    // finds position to insert memory to keep list ordered
    // first_prev -> first -> ... (memory somewhere here) ... -> last -> last_next
    pos find_pos_interval(const allocator_info& info, char* memory, char* first_prev, char* first,
                          char* last, char* last_next) noexcept
    {
        // note: first_prev/last_next can be the proxy nodes, then first_prev isn't necessarily less than first!
        WPI_MEMORY_ASSERT(less(first, memory) && less(memory, last));

        // need to insert somewhere in the middle
        // search through the entire list
        // search from both ends at once
        auto cur_forward  = first;
        auto prev_forward = first_prev;

        auto cur_backward  = last;
        auto prev_backward = last_next;

        do
        {
            if (greater(cur_forward, memory))
                return {prev_forward, cur_forward};
            else if (less(cur_backward, memory))
                // the next position is the previous backwards pointer
                return {cur_backward, prev_backward};
            debug_check_double_dealloc([&]
                                       { return cur_forward != memory && cur_backward != memory; },
                                       info, memory);
            xor_list_iter_next(cur_forward, prev_forward);
            xor_list_iter_next(cur_backward, prev_backward);
        } while (less(prev_forward, prev_backward));

        // ran outside of list
        debug_check_double_dealloc([] { return false; }, info, memory);
        return {nullptr, nullptr};
    }

    // finds the position in the entire list
    pos find_pos(const allocator_info& info, char* memory, char* begin_node, char* end_node,
                 char* last_dealloc, char* last_dealloc_prev) noexcept
    {
        auto first = xor_list_get_other(begin_node, nullptr);
        auto last  = xor_list_get_other(end_node, nullptr);

        if (greater(first, memory))
            // insert at front
            return {begin_node, first};
        else if (less(last, memory))
            // insert at the end
            return {last, end_node};
        else if (less(last_dealloc_prev, memory) && less(memory, last_dealloc))
            // insert before last_dealloc
            return {last_dealloc_prev, last_dealloc};
        else if (less(memory, last_dealloc))
            // insert into [first, last_dealloc_prev]
            return find_pos_interval(info, memory, begin_node, first, last_dealloc_prev,
                                     last_dealloc);
        else if (greater(memory, last_dealloc))
            // insert into (last_dealloc, last]
            return find_pos_interval(info, memory, last_dealloc_prev, last_dealloc, last, end_node);

        WPI_MEMORY_UNREACHABLE("memory must be in some half or outside");
        return {nullptr, nullptr};
    }
} // namespace

constexpr std::size_t ordered_free_memory_list::min_element_size;
constexpr std::size_t ordered_free_memory_list::min_element_alignment;

ordered_free_memory_list::ordered_free_memory_list(std::size_t node_size) noexcept
: node_size_(node_size > min_element_size ? node_size : min_element_size),
  capacity_(0u),
  last_dealloc_(end_node()),
  last_dealloc_prev_(begin_node())
{
    xor_list_set(begin_node(), nullptr, end_node());
    xor_list_set(end_node(), begin_node(), nullptr);
}

ordered_free_memory_list::ordered_free_memory_list(ordered_free_memory_list&& other) noexcept
: node_size_(other.node_size_), capacity_(other.capacity_)
{
    if (!other.empty())
    {
        auto first = xor_list_get_other(other.begin_node(), nullptr);
        auto last  = xor_list_get_other(other.end_node(), nullptr);

        xor_list_set(begin_node(), nullptr, first);
        xor_list_change(first, other.begin_node(), begin_node());
        xor_list_change(last, other.end_node(), end_node());
        xor_list_set(end_node(), last, nullptr);

        other.capacity_ = 0u;
        xor_list_set(other.begin_node(), nullptr, other.end_node());
        xor_list_set(other.end_node(), other.begin_node(), nullptr);
    }
    else
    {
        xor_list_set(begin_node(), nullptr, end_node());
        xor_list_set(end_node(), begin_node(), nullptr);
    }

    // for programming convenience, last_dealloc is reset
    last_dealloc_prev_ = begin_node();
    last_dealloc_      = xor_list_get_other(last_dealloc_prev_, nullptr);
}

void wpi::memory::detail::swap(ordered_free_memory_list& a,
                                     ordered_free_memory_list& b) noexcept
{
    auto a_first = xor_list_get_other(a.begin_node(), nullptr);
    auto a_last  = xor_list_get_other(a.end_node(), nullptr);

    auto b_first = xor_list_get_other(b.begin_node(), nullptr);
    auto b_last  = xor_list_get_other(b.end_node(), nullptr);

    if (!a.empty())
    {
        xor_list_set(b.begin_node(), nullptr, a_first);
        xor_list_change(a_first, a.begin_node(), b.begin_node());
        xor_list_change(a_last, a.end_node(), b.end_node());
        xor_list_set(b.end_node(), a_last, nullptr);
    }
    else
    {
        xor_list_set(b.begin_node(), nullptr, b.end_node());
        xor_list_set(b.end_node(), b.begin_node(), nullptr);
    }

    if (!b.empty())
    {
        xor_list_set(a.begin_node(), nullptr, b_first);
        xor_list_change(b_first, b.begin_node(), a.begin_node());
        xor_list_change(b_last, b.end_node(), a.end_node());
        xor_list_set(a.end_node(), b_last, nullptr);
    }
    else
    {
        xor_list_set(a.begin_node(), nullptr, a.end_node());
        xor_list_set(a.end_node(), a.begin_node(), nullptr);
    }

    detail::adl_swap(a.node_size_, b.node_size_);
    detail::adl_swap(a.capacity_, b.capacity_);

    // for programming convenience, last_dealloc is reset
    a.last_dealloc_prev_ = a.begin_node();
    a.last_dealloc_      = xor_list_get_other(a.last_dealloc_prev_, nullptr);

    b.last_dealloc_prev_ = b.begin_node();
    b.last_dealloc_      = xor_list_get_other(b.last_dealloc_prev_, nullptr);
}

void ordered_free_memory_list::insert(void* mem, std::size_t size) noexcept
{
    WPI_MEMORY_ASSERT(mem);
    WPI_MEMORY_ASSERT(is_aligned(mem, alignment()));
    detail::debug_fill_internal(mem, size, false);

    insert_impl(mem, size);
}

void* ordered_free_memory_list::allocate() noexcept
{
    WPI_MEMORY_ASSERT(!empty());

    // remove first node
    auto prev = begin_node();
    auto node = xor_list_get_other(prev, nullptr);
    auto next = xor_list_get_other(node, prev);

    xor_list_set(prev, nullptr, next); // link prev to next
    xor_list_change(next, node, prev); // change prev of next
    --capacity_;

    if (node == last_dealloc_)
    {
        // move last_dealloc_ one further in
        last_dealloc_ = next;
        WPI_MEMORY_ASSERT(last_dealloc_prev_ == prev);
    }
    else if (node == last_dealloc_prev_)
    {
        // now the previous node is the node before ours
        last_dealloc_prev_ = prev;
        WPI_MEMORY_ASSERT(last_dealloc_ == next);
    }

    return detail::debug_fill_new(node, node_size_, 0);
}

void* ordered_free_memory_list::allocate(std::size_t n) noexcept
{
    WPI_MEMORY_ASSERT(!empty());

    if (n <= node_size_)
        return allocate();

    auto i = xor_list_search_array(begin_node(), end_node(), n, node_size_);
    if (i.first == nullptr)
        return nullptr;

    xor_list_change(i.prev, i.first, i.next); // change next pointer from i.prev to i.next
    xor_list_change(i.next, i.last, i.prev);  // change prev pointer from i.next to i.prev
    capacity_ -= i.size(node_size_);

    // if last_dealloc_ points into the array being removed
    if ((less_equal(i.first, last_dealloc_) && less_equal(last_dealloc_, i.last)))
    {
        // move last_dealloc just outside range
        last_dealloc_      = i.next;
        last_dealloc_prev_ = i.prev;
    }
    // if the previous deallocation is the last element of the array
    else if (last_dealloc_prev_ == i.last)
    {
        // it is now the last element before the array
        WPI_MEMORY_ASSERT(last_dealloc_ == i.next);
        last_dealloc_prev_ = i.prev;
    }

    return detail::debug_fill_new(i.first, n, 0);
}

void ordered_free_memory_list::deallocate(void* ptr) noexcept
{
    auto node = static_cast<char*>(debug_fill_free(ptr, node_size_, 0));

    auto p =
        find_pos(allocator_info(WPI_MEMORY_LOG_PREFIX "::detail::ordered_free_memory_list",
                                this),
                 node, begin_node(), end_node(), last_dealloc_, last_dealloc_prev_);

    xor_list_insert(node, p.prev, p.next);
    ++capacity_;

    last_dealloc_      = node;
    last_dealloc_prev_ = p.prev;
}

void ordered_free_memory_list::deallocate(void* ptr, std::size_t n) noexcept
{
    if (n <= node_size_)
        deallocate(ptr);
    else
    {
        auto mem  = detail::debug_fill_free(ptr, n, 0);
        auto prev = insert_impl(mem, n);

        last_dealloc_      = static_cast<char*>(mem);
        last_dealloc_prev_ = prev;
    }
}

std::size_t ordered_free_memory_list::alignment() const noexcept
{
    return alignment_for(node_size_);
}

char* ordered_free_memory_list::insert_impl(void* mem, std::size_t size) noexcept
{
    auto no_nodes = size / node_size_;
    WPI_MEMORY_ASSERT(no_nodes > 0);

    auto p =
        find_pos(allocator_info(WPI_MEMORY_LOG_PREFIX "::detail::ordered_free_memory_list",
                                this),
                 static_cast<char*>(mem), begin_node(), end_node(), last_dealloc_,
                 last_dealloc_prev_);

    xor_link_block(mem, node_size_, no_nodes, p.prev, p.next);
    capacity_ += no_nodes;

    if (p.prev == last_dealloc_prev_)
    {
        last_dealloc_ = static_cast<char*>(mem);
    }

    return p.prev;
}

char* ordered_free_memory_list::begin_node() noexcept
{
    void* mem = &begin_proxy_;
    return static_cast<char*>(mem);
}

char* ordered_free_memory_list::end_node() noexcept
{
    void* mem = &end_proxy_;
    return static_cast<char*>(mem);
}
