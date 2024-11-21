# cmake_minimum_required(VERSION 3.16)

project(qtAiLib LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
  OpenAIBaseClient.cpp
  OpenAIBaseClient.h
  OpenAIAPIClient.cpp
  OpenAIAPIClient.h
  OpenAIImageGenerator.cpp
  OpenAIImageGenerator.h
  OpenAITTSClient.cpp
  OpenAITTSClient.h
  ImageDownloader.cpp
  ImageDownloader.h
)

# Add the include directory to the library's include path
target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(${PROJECT_NAME} PRIVATE LIB2_LIBRARY)
