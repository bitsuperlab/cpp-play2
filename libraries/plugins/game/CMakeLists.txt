file(GLOB HEADERS "include/graphene/witness/*.hpp")

add_library( graphene_game
             game.cpp
           )

target_link_libraries( graphene_game graphene_chain graphene_app graphene_time )
target_include_directories( graphene_game
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   graphene_game

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
