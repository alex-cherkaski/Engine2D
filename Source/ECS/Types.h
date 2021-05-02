#pragma once

using Entity = size_t;
using ComponentId = size_t;
using TagId = size_t;

constexpr size_t COMPONENT_COUNT = 16;
using ComponentKey = std::bitset<COMPONENT_COUNT>;

