

install(DIRECTORY . DESTINATION "include"
  PATTERN "*~" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE)

