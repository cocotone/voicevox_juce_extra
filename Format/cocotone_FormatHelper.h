#pragma once

namespace cctn
{

//==============================================================================
// Function to repeat elements in a vector
template<typename T>
std::vector<T> repeat(const std::vector<T>& input, const std::vector<T>& repeats) 
{
    std::vector<T> result;
    for (size_t i = 0; i < input.size(); ++i) 
    {
        result.insert(result.end(), repeats[i], input[i]);
    }
    return result;
}

//==============================================================================
// Function to repeat elements in a vector
template<typename T>
std::vector<T> repeat_safe(const std::vector<T>& input, const std::vector<T>& repeats)
{
    if (input.empty() || repeats.empty())
    {
        // Return empty vector if either input is empty
        return {};
    }

    std::vector<T> result;
    size_t min_size = std::min(input.size(), repeats.size());
    result.reserve(std::accumulate(repeats.begin(), repeats.begin() + min_size, size_t(0)));

    for (size_t i = 0; i < min_size; ++i)
    {
        if (repeats[i] < 0)
        {
            throw std::invalid_argument("Negative repeat count at index " + std::to_string(i));
        }
        result.insert(result.end(), repeats[i], input[i]);
    }

    // Handle any remaining elements in input if repeats is shorter
    for (size_t i = min_size; i < input.size(); ++i)
    {
        // Default to repeating once
        result.push_back(input[i]);
    }

    return result;
}

}
