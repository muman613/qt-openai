project(test-tts-gen-gui)

add_executable(${PROJECT_NAME} main.cpp 
    TTSGeneratorWindow.h 
    TTSGeneratorWindow.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC qtAiLib Qt5::Widgets Qt5::Multimedia)
