#pragma once

inline static constexpr unsigned char operator ""_uc(unsigned long long value) noexcept
{
	return static_cast<unsigned char>(value);
}

