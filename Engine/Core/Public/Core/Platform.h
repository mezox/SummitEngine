#pragma once

#define NO_DISCARD [[nodiscard]]

#define DECLARE_NOCOPY_NOMOVE(Name) \
Name(const Name& other) = delete; \
Name(Name&& other) = delete; \
Name& operator=(const Name& other) = delete; \
Name& operator=(Name&& other) = delete;
