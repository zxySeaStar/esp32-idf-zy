
file(GLOB private_lib "./daplink/cmsis-dap/*.c" "./daplink/usb/*.c" "./daplink/ota/*.c")

idf_component_register(SRCS "my_projects.c" ${private_lib}
                    INCLUDE_DIRS "." "./daplink/cmsis-dap/" "./daplink/usb/" "./include/" "./daplink/ota/")
