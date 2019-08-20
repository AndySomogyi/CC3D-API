add_library(CC3D_C_API INTERFACE)

target_include_directories(CC3D_C_API INTERFACE include)

target_sources(CC3D_C_API INTERFACE
  cc3d_api.h)
