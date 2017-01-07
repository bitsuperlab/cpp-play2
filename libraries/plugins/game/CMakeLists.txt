file(GLOB HEADERS "include/graphene/game/*.hpp")

add_library( graphene_game
             v8_helper.cpp
             v8_api.cpp
             v8_game.cpp
             game_plugin.cpp
           )

target_link_libraries( graphene_game graphene_chain graphene_app graphene_time exlib v8 )
target_include_directories( graphene_game
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../vendor/v8-fibjs/v8"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../vendor/v8-fibjs/exlib" )

install( TARGETS
   graphene_game

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
