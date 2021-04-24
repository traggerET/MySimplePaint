project(Editor)

set(DEF_PREFIX "definitions/")

set(CPP_FILES main.cpp
               ${DEF_PREFIX}Adapter.cpp
               ${DEF_PREFIX}Application.cpp
               ${DEF_PREFIX}UIBasic.cpp
               ${DEF_PREFIX}GraphicEditor.cpp)

set(CMAKE_CXX_FLAGS "-lsfml-graphics -lsfml-window -lsfml-system -fsanitize=address -g")
set(CMAKE_EXE_LINKER_FLAGS "-lsfml-graphics -lsfml-window -lsfml-system")

include_directories(headers ./)

add_executable(main ${CPP_FILES})
target_link_libraries(main sfml-graphics sfml-window sfml-system)
