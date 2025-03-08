cmake_minimum_required(VERSION 3.16)
project(DNSChanger VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(dns-changer main.cpp)
target_link_libraries(dns-changer PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

install(TARGETS dns-changer DESTINATION bin)
# Include the install required system libraries
include(InstallRequiredSystemLibraries)

# CPack metadata
set(CPACK_PACKAGE_NAME "dns-changer")
set(CPACK_PACKAGE_VERSION "1.0.1")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")

# For Debian packaging, you can specify additional metadata
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name")   # required field
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6 (>= 6.0)")

# For RPM packaging, similar settings can be provided:
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.0")

# Set generators for both DEB and RPM
set(CPACK_GENERATOR "DEB;RPM")

# Optionally define package icon, summary, description, etc.
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "a simple dns changer.")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")


# Include CPack to process the packaging configuration
include(CPack)
