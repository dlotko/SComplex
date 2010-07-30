set(TEST_OUTPUT_FORMAT HRF)
set(TEST_LOG_LEVEL message)
set(TEST_REPORT_LEVEL short)
set(TEST_RUN *)

set(ARGS "${ARGS} -VV")

set(TestLibraryName "${PROJECT_NAME}_test")

macro(AddSingleTest file)
  get_filename_component(name ${file} NAME_WE)

  add_executable(${name} ${file})

  target_link_libraries(${name} capd)
  target_link_libraries(${name} ${TestLibraryName} ${PROJECT_NAME})

  add_test(${name} ${name} --output_format=${TEST_OUTPUT_FORMAT} --log_level=${TEST_LOG_LEVEL} --report_level=${TEST_REPORT_LEVEL} --run_test=${TEST_RUN})

endmacro(AddSingleTest)


add_library(${TestLibraryName} SComplexUnitTestMain.cpp)

AddSingleTest(SComplexInitTest.cpp)
AddSingleTest(SComplexColorTest.cpp)
AddSingleTest(SComplexReductionTest.cpp)
AddSingleTest(CubSComplexIteratorsTest.cpp)
AddSingleTest(CubSComplexReductionTest.cpp)
AddSingleTest(SimplexSComplexTest.cpp)

AddSingleTest(COAKQTest.cpp)
AddSingleTest(AKQTest.cpp)
AddSingleTest(CellComplexAKQTest.cpp)