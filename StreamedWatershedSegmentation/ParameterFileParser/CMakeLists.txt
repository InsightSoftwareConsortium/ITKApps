# Sources of non-templated classes.
SET(param_SRCS
lex.yy.cxx
y.tab.cxx
param.cxx
)


ADD_LIBRARY(param  ${param_SRCS})
INSTALL_TARGETS(/lib param)
TARGET_LINK_LIBRARIES(param ITKCommon)
