cmake_minimum_required (VERSION 2.6)

project (Liber)

add_executable (
    liber
    src/allocator.cpp
    src/baseclasses.cpp
    src/blocks.cpp
    src/csscolorparser.cpp
    src/figure.cpp
    src/flowable.cpp
    src/group.cpp
    src/numbering.cpp
    src/operation.cpp
    src/page.cpp
    src/parser.cpp
    src/podofowriter.cpp
    src/writer.cpp
    src/main.cpp
)

target_link_libraries (
    liber
    podofo
)

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/blocks.h ${CMAKE_CURRENT_SOURCE_DIR}/src/blocks.cpp
    DEPENDS src/attributes.yaml src/blocks_code_generator.py src/blocks.h.template src/blocks.cpp.template
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/src && python blocks_code_generator.py
    COMMENT "Generating sources..."
)

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/docs/base_attributes.liber
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/page_attributes.liber
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/text_attributes.liber
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/image_attributes.liber
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/space_attributes.liber
    DEPENDS src/attributes.yaml src/blocks_code_generator.py docs/attributes.liber.template
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/src && python blocks_code_generator.py --docs
    COMMENT "Generating sources..."
)

add_custom_target (docs cd ${CMAKE_CURRENT_SOURCE_DIR}/docs/ &&
    ${CMAKE_CURRENT_BINARY_DIR}/liber manual.liber manual.pdf
    DEPENDS liber
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/docs/page_attributes.liber
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/text_attributes.liber
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/image_attributes.liber
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/space_attributes.liber
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/style.liber
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/manual.liber
)
