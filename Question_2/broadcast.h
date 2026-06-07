#pragma once

#include <vector>
#include <optional>

std::optional<std::vector<int64_t>>
broadcastShape(
    const std::vector<int64_t>& a,
    const std::vector<int64_t>& b
);