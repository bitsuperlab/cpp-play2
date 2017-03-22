list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/libraries/fc/GitVersionGen" )
include( GetGitRevisionDescription )
get_git_head_revision(GIT_REFSPEC GRAPHENE_GIT_REVISION_SHA)
get_git_unix_timestamp(GRAPHENE_GIT_REVISION_UNIX_TIMESTAMP)
git_describe(GRAPHENE_GIT_REVISION_DESCRIPTION --tags)
if(NOT GRAPHENE_GIT_REVISION_DESCRIPTION)
    set(GRAPHENE_GIT_REVISION_DESCRIPTION "unknown")
endif(NOT GRAPHENE_GIT_REVISION_DESCRIPTION)

IF( WIN32 )
  add_definitions(-DCURL_STATICLIB) # force statick link
ENDIF(WIN32)

file(GLOB headers "include/graphene/utilities/*.hpp")

set(sources
   key_conversion.cpp
   string_escape.cpp
   tempdir.cpp
   words.cpp
   http_downloader.cpp
   combinatorics.cpp
   ${headers})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_revision.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_revision.cpp" @ONLY)
list(APPEND sources "${CMAKE_CURRENT_BINARY_DIR}/git_revision.cpp")

add_library( graphene_utilities
             ${sources}
             ${HEADERS} )
target_link_libraries( graphene_utilities fc ${CURL_LIBRARY})
target_include_directories( graphene_utilities
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${CURL_INCLUDE_DIRS})
if (USE_PCH)
  set_target_properties(graphene_utilities PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
  cotire(graphene_utilities)
endif(USE_PCH)

install( TARGETS
   graphene_utilities

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
