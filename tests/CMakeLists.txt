
set(CURRENT_PROJ_NAME "RunTests")

add_executable(${CURRENT_PROJ_NAME}
    "StringLiteralTest.cpp"
)


target_link_libraries(${CURRENT_PROJ_NAME} PRIVATE
    QkTraits
    GTest::gtest_main
)


include(GoogleTest)
gtest_discover_tests(${CURRENT_PROJ_NAME})
