# add_library(infrastructure STATIC)

# target_sources(infrastructure
#     PRIVATE
#         SqliteUserRepository.cpp
#     PUBLIC
#         FILE_SET HEADERS
#         BASE_DIRS .
#         FILES
#             SqliteUserRepository.hpp
# )

# target_link_libraries(infrastructure
#     PUBLIC domain
#     PRIVATE Qt6::Core
# )
