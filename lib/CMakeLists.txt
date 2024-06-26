#添加子目录的源文件给_SUB_SOURCES
aux_source_directory(. _SUB_SOURCES)
#将子目录源文件，追加给根目录_SOURCES变量
list(APPEND _SOURCES ${_SUB_SOURCES})
add_library(RubberDuck STATIC ${_SOURCES})
