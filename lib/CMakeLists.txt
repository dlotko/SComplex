
add_library(${PROJECT_NAME} STATIC 
complex/cubical/CubSComplex_Cell.cpp
complex/simplicial/SimplexCell.cpp
complex/simplicial/SimplexSComplex.cpp
complex/simplicial/SimplexSubdivision.cpp
complex/simplicial/SimplexIO.cpp
)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)


