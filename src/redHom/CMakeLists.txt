
add_executable(redHom main.cpp)
target_link_libraries(redHom capd ${Boost_LIBRARIES} ${PROJECT_NAME})
include_directories(inc)


install(TARGETS redHom
  RUNTIME DESTINATION bin)
