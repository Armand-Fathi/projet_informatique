#pragma once
#include <QMainWindow>
#include <vector>

class HeatMapWidget;
class QPushButton;
class QLabel;
class QSlider;
class QWidget;
class QVBoxLayout;
class QScrollArea;

#include "simulationcontroller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    struct WindRow {
        QWidget* container = nullptr;
        QLabel* title = nullptr;
        QSlider* speedSlider = nullptr;
        QLabel* speedLabel = nullptr;
        QSlider* angleSlider = nullptr;
        QLabel* angleLabel = nullptr;
        QPushButton* removeBtn = nullptr;
    };

    HeatMapWidget* view_ = nullptr;
    SimulationController controller_;

    QWidget* leftPanelWidget_ = nullptr;
    QWidget* windsContainer_ = nullptr;
    QScrollArea* windsScrollArea_ = nullptr;

    QPushButton* startBtn_ = nullptr;
    QPushButton* pauseBtn_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* addWindBtn_ = nullptr;

    QLabel* resultantLabel_ = nullptr;

    QVBoxLayout* windsLayout_ = nullptr;
    std::vector<WindRow> windRows_;

    void addWindRow(double speed = 0.35, double angle = 180.0);
    void removeWindRow(QWidget* rowWidget);
    void rebuildWindTitles();
    void syncControllerFromUi();
    void updateResultantLabel();
    void wireUi();
};
