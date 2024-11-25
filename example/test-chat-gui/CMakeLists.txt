project(test-chat-gui)

add_executable(${PROJECT_NAME} main.cpp ChatClientWindow.h ChatClientWindow.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC qtAiLib Qt5::Widgets)
