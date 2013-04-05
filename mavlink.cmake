set(MAVGEN /usr/share/pyshared/pymavlink/generator/mavgen.py)
find_package(PythonInterp 2 REQUIRED)
ADD_CUSTOM_COMMAND(
   OUTPUT ${PROJECT_SOURCE_DIR}/mavlink/ardrone/mavlink.h
   COMMAND ${PYTHON_EXECUTABLE} ${MAVGEN} --lang=C --output=mavlink ardrone.xml --wire-protocol=1.0
   DEPENDS ${PROJECT_SOURCE_DIR}/ardrone.xml
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
add_custom_target(mavlink_headers ALL DEPENDS mavlink/ardrone/mavlink.h)
