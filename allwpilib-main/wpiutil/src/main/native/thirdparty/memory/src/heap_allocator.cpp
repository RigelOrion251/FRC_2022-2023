// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "wpi/memory/heap_allocator.hpp"

#include "wpi/memory/error.hpp"

using namespace wpi::memory;

#ifdef _WIN32
#include <malloc.h>
#include <windows.h>

namespace
{
    HANDLE get_process_heap() noexcept
    {
        static auto heap = GetProcessHeap();
        return heap;
    }

    std::size_t max_size() noexcept
    {
        return _HEAP_MAXREQ;
    }
} // namespace

void* wpi::memory::heap_alloc(std::size_t size) noexcept
{
    return HeapAlloc(get_process_heap(), 0, size);
}

void wpi::memory::heap_dealloc(void* ptr, std::size_t) noexcept
{
    HeapFree(get_process_heap(), 0, ptr);
}

#elif WPI_HOSTED_IMPLEMENTATION
#include <cstdlib>
#include <memory>

void* wpi::memory::heap_alloc(std::size_t size) noexcept
{
    return std::malloc(size);
}

void wpi::memory::heap_dealloc(void* ptr, std::size_t) noexcept
{
    std::free(ptr);
}

namespace
{
    std::size_t max_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
} // namespace
#else
// no implementation for heap_alloc/heap_dealloc

namespace
{
    std::size_t max_size() noexcept
    {
        return std::size_t(-1);
    }
} // namespace
#endif

allocator_info detail::heap_allocator_impl::info() noexcept
{
    return {WPI_MEMORY_LOG_PREFIX "::heap_allocator", nullptr};
}

std::size_t detail::heap_allocator_impl::max_node_size() noexcept
{
    return max_size();
}

#if WPI_MEMORY_EXTERN_TEMPLATE
template class detail::lowlevel_allocator<detail::heap_allocator_impl>;
template class wpi::memory::allocator_traits<heap_allocator>;
#endif
