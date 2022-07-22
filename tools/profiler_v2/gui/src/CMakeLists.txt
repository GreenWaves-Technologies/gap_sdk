
qt5_wrap_cpp(gui_SRC ${HEADER_DIR}/MainWindow.hpp)

add_executable(profiler
    mainwindow.cpp
    main.cpp
    adjustingscrollarea.cpp
    dialog.cpp
    highlighter.cpp
    hlegendwidget.cpp
    profiler_backend_user.cpp
    prolabel.cpp
    signaltree.cpp
    splitterdesign.cpp
    timeline.cpp
    tlgview.cpp
    treeitem.cpp
    treemodel.cpp
    hotspotswidget.cpp
    hotspotsfunctionwidget.cpp
    hotspotsfilewidget.cpp
    hotspotslinewidget.cpp
    codeeditor.cpp
    codeeditorwrap.cpp
    pccheckbox.cpp
    json.cpp
    jsmn.cpp
    ../include/mainwindow.hpp
    ../include/adjustingscrollarea.hpp
    ../include/dialog.hpp
    ../include/highlighter.hpp
    ../include/hlegendwidget.hpp
    ../include/profiler_backend_user.hpp
    ../include/prolabel.hpp
    ../include/signaltree.hpp
    ../include/splitterdesign.hpp
    ../include/timeline.hpp
    ../include/tlgview.hpp
    ../include/treeitem.hpp
    ../include/treemodel.hpp
    ../include/hotspotswidget.hpp
    ../include/hotspotsfunctionwidget.hpp
    ../include/hotspotsfilewidget.hpp
    ../include/hotspotslinewidget.hpp
    ../include/codeeditor.hpp
    ../include/linenumberarea.hpp
    ../include/codeeditorwrap.hpp
    ../include/pccheckbox.hpp)

qt5_use_modules(profiler Widgets Charts)
#include_directories(profiler  "../backend/include ")
target_include_directories(profiler PRIVATE ../include)

#target_link_libraries(main LINK_PUBLIC profiler_backend Qt5Widgets)
target_link_libraries(profiler profiler_backend)
#target_link_libraries(main PRIVATE )
