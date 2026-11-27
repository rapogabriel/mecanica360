cmake_minimum_required(VERSION 3.25)
project(MeuProjeto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Catch2 3 REQUIRED)

# Fontes e headers
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Retira main.cpp (quando existir) e o FakeRepository da biblioteca principal
set(MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set(FAKE_REPO_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/FakeRepository.cpp")
list(REMOVE_ITEM PROJECT_SOURCES "${MAIN_SOURCE}" "${FAKE_REPO_SOURCE}")

add_library(project_lib STATIC
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(project_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(project_lib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_options(project_lib PRIVATE -Wall -Wextra -Wpedantic)

# Executável principal (só é criado quando src/main.cpp existir)
if(EXISTS "${MAIN_SOURCE}")
    add_executable(app "${MAIN_SOURCE}")
    target_link_libraries(app PRIVATE project_lib)
endif()

# Testes separados por subpasta (unit, func, integration)
include(CTest)
if(BUILD_TESTING)
    include(Catch)

    # Biblioteca auxiliar de testes (contém o FakeRepository, se existir)
    add_library(test_support INTERFACE)
    if(EXISTS "${FAKE_REPO_SOURCE}")
        add_library(fake_repo_lib STATIC "${FAKE_REPO_SOURCE}")
        target_link_libraries(fake_repo_lib PUBLIC project_lib)
        target_link_libraries(test_support INTERFACE fake_repo_lib)
    endif()

    function(add_test_suite suite_name)
        file(GLOB_RECURSE SUITE_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/${suite_name}/*.cpp"
        )

        # Só cria o target se houver arquivos .cpp na pasta
        if(SUITE_SOURCES)
            add_executable(tests_${suite_name} ${SUITE_SOURCES})
            target_link_libraries(tests_${suite_name} PRIVATE
                project_lib
                test_support
                Catch2::Catch2WithMain
            )
            catch_discover_tests(tests_${suite_name}
                TEST_PREFIX "[${suite_name}] "
                PROPERTIES LABELS "${suite_name}"
            )
        endif()
    endfunction()

    add_test_suite(unit)
    add_test_suite(func)
    add_test_suite(integration)
endif()