cmake_minimum_required(VERSION 2.8)
project( Segmentation )
find_package( OpenCV REQUIRED )
set(headers

	include/Edge.h 
	include/Grafo.h
	include/Nodo.h
	
)
add_executable( Segmentation main.cpp )
set_property(TARGET Segmentation PROPERTY CXX_STANDARD 11)
target_link_libraries( Segmentation ${OpenCV_LIBS} )
