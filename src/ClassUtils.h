#pragma once

#define NO_COPY(T) \
T(const T&) = delete; \
T& operator=(const T&) = delete;

#define NO_MOVE(T) \
T(T&&) = delete; \
T& operator=(T&&) = delete;

#define DEFAULT_COPY(T) \
T(const T&) = default; \
T& operator=(const T&) = default;

#define DEFAULT_MOVE(T) \
T(T&&) = default; \
T& operator=(T&&) = default;
