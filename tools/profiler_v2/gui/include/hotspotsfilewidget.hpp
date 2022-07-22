/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef HOTSPOTSFILEWIDGET_HPP
#define HOTSPOTSFILEWIDGET_HPP

#include <QWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QTableWidget>
#include <QString>
#include <QHBoxLayout>
#include <QFile>
#include <QTimer>
#include <QPlainTextEdit>
#include <QMutex>

#include "highlighter.hpp"
#include "stallchart.hpp"
//#include "splitterdesign.hpp"
#include "util.hpp"
#include "profiler_backend_user.hpp"
#include "codeeditorwrap.hpp"
#include "safehotspotfileinfo.hpp"

/**
  @brief displays hotspots metrics, such as  ...
  */
class HotspotsFileWidget : public QWidget
{
    Q_OBJECT

    static const char* columnHeader[];
    static const int nbColumn;

public :
    HotspotsFileWidget(QWidget* parent,
                       Profiler_backend_user* backend,
                       CodeEditorWrap* sourceCodeWrap);

    /** in order for the app to be more accessible, table data can be exported to
        a text file (that an other program can read out to a blind person for
        example)*/
    void exportTableToTextFile(const char* filename) const;
    //StallChart* stallChart;
    //void switchLegendMode(LegendMode newMode);
    void updateHotspotsTable(Hospot_results *result,
                             int64_t start_timestamp,
                             int64_t end_timestamp,
                             int64_t max_timestamp);

    //gets hotspots for a specific time range
    void getHotspots(uint64_t t0Select, uint64_t t1Select);


private :
    void updateTimeStamps(void);
    void setSourceCode(std::string fname,
                       std::string fileName,
                       int hotspotLine );
    void selectFunction(std::string funcName,
                        std::string fileName,
                        int hotspotLine);
    std::string get_source_code(std::string funcName,
                                std::string fileName,
                                int hotspotLine);
    std::string read_file(std::string filename);
    void removeTableRows();
    void getFileHotspotList(const std::string & fileName,
                            std::list<SafeHotspotLineInfo> * fileHotspots);


    QTableWidget* table = nullptr;
    QHeaderView* header;
    CodeEditorWrap* sourceCodeWrap;
    QPlainTextEdit* asmCode;
    QHBoxLayout* layout;
    QTimer* timer;
    std::string textVersion;
    //LegendMode currentMode=TIME_MODE;

    Highlighter* cppHighlighter;
    Highlighter* asmHighlighter;
    std::string selectedFunction = "";

    // Hot spots parameters
    Profiler_backend_user* backend;
    Hospot_results *result;
    int64_t start_timestamp = 0;
    int64_t end_timestamp =0;
    int64_t max_timestamp =10;

    Hospot_results *current_ht_result;

    // All hotspots ordered by function
    std::list<Hotspot_file_info *> beHotspots;
    std::list<SafeHotspotFileInfo> hotspots;
    // All the hotspots ordered by duration
    std::list<Hotspot_line_info *> beAllHotspots;
    std::list<SafeHotspotLineInfo> allHotspots;


    QMutex hotspotsMutex;

signals:
    void hotspotsReady();

private slots:

    void getHotspots();
    void fillHotspotsTable();

    QString formatTimeStamp(QString ts) const;

public slots:
    /** selects the function for which more details are displayed */
    void selectFunction(QTableWidgetItem* it);
    void selectFunction(int row);
    void selectRow(const char* name);
    void handleVisibilityChanged(bool visible);



};

#endif //HotspotsFileWidget_HPP
