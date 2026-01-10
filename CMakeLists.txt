cmake_minimum_required(VERSION 3.26.4)
project(Magazyn)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
  Core
  Gui
  Widgets
  Sql
  REQUIRED)

add_executable(Magazyn main.cpp
        controllers/AppController.cpp
        controllers/AppController.h
        widgets/LoginView.cpp
        widgets/LoginView.h
        controllers/roles/WarehouseWorkerController.cpp
        controllers/roles/WarehouseWorkerController.h
        controllers/roles/ManagerController.cpp
        controllers/roles/ManagerController.h
        controllers/roles/AdminController.cpp
        controllers/roles/AdminController.h
        controllers/roles/ReceivingWorkerController.cpp
        controllers/roles/ReceivingWorkerController.h
        widgets/MainWindow.cpp
        widgets/MainWindow.h
        widgets/warehouseworker/WarehouseWorkerMainMenuView.cpp
        widgets/warehouseworker/WarehouseWorkerMainMenuView.h
        widgets/warehouseworker/WarehouseWorkerOrderView.cpp
        widgets/warehouseworker/WarehouseWorkerOrderView.h
        widgets/warehouseworker/WarehouseWorkerOrderView.h
        widgets/warehouseworker/WarehouseWorkerOrderView.h
        widgets/receivingworker/ReceivingWorkerMainMenuView.cpp
        widgets/receivingworker/ReceivingWorkerMainMenuView.h
        widgets/receivingworker/ReceivingWorkerOrderListView.cpp
        widgets/receivingworker/ReceivingWorkerOrderListView.h
        widgets/receivingworker/ReceivingWorkerDeliveryListView.cpp
        widgets/receivingworker/ReceivingWorkerDeliveryListView.h
        widgets/receivingworker/ReceivingWorkerDeliveryView.cpp
        widgets/receivingworker/ReceivingWorkerDeliveryView.h
        widgets/manager/ManagerMainMenuView.cpp
        widgets/manager/ManagerMainMenuView.h
        widgets/manager/ManagerLogsView.cpp
        widgets/manager/ManagerLogsView.h
        widgets/manager/ManagerVehiclesView.cpp
        widgets/manager/ManagerVehiclesView.h
        widgets/manager/ManagerEmployeesView.cpp
        widgets/manager/ManagerEmployeesView.h
        widgets/manager/ManagerOrdersListView.cpp
        widgets/manager/ManagerOrdersListView.h
        widgets/manager/ManagerOrderView.cpp
        widgets/manager/ManagerOrderView.h
        controllers/databases/WarehouseWorkerDatabase.cpp
        controllers/databases/WarehouseWorkerDatabase.h)
target_link_libraries(Magazyn
  Qt::Core
  Qt::Gui
  Qt::Widgets
  Qt::Sql
)


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach(QT_LIB)
endif()
