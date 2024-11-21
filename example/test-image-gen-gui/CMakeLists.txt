project(test-image-gen-gui)

add_executable(${PROJECT_NAME} main.cpp ImageGeneratorWindow.h ImageGeneratorWindow.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC qtAiLib Qt5::Widgets)
