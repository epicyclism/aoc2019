#pragma once
#include <vector>
#include <algorithm>

template<typename T> struct linear_set : public std::vector<T>
{
    bool contains(T const& t) const noexcept
    {
        return std::find(this->begin(), this->end(), t) != this->end();
    }
};