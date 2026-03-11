#pragma once
#include <QMainWindow>

class HeatMapWidget;
class QSlider;
class QPushButton;
class QLabel;

#include "simulationcontroller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);
    ~MainWindow();

private:
    HeatMapWidget* view_ = nullptr;
    SimulationController controller_;

    QSlider* speedSlider_ = nullptr;
    QSlider* angleSlider_ = nullptr;
    QLabel*  speedLabel_ = nullptr;
    QLabel*  angleLabel_ = nullptr;

    QPushButton* startBtn_ = nullptr;
    QPushButton* pauseBtn_ = nullptr;
    QPushButton* resetBtn_ = nullptr;

    void wireUi();
};
