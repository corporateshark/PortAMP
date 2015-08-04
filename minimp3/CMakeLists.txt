cmake_minimum_required(VERSION 2.8)

project("minimp3")

set(SRC_FILES
	minimp3.c
)

set(HEADER_FILES
	libc.h
	minimp3.h
)

add_library(
	minimp3 ${SRC_FILES} ${HEADER_FILES}
)
