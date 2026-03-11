#include "mainwindow.h"
#include "heatmapwidget.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QHBoxLayout(central);

    // 左侧：控制面板
    auto* panel = new QVBoxLayout();
    root->addLayout(panel, 0);

    speedLabel_ = new QLabel("Speed: 0.35", this);
    angleLabel_ = new QLabel("Angle: 180", this);

    speedSlider_ = new QSlider(Qt::Horizontal, this);
    speedSlider_->setRange(0, 100);
    speedSlider_->setValue(35);

    angleSlider_ = new QSlider(Qt::Horizontal, this);
    angleSlider_->setRange(0, 360);
    angleSlider_->setValue(180);

    startBtn_ = new QPushButton("Start", this);
    pauseBtn_ = new QPushButton("Pause", this);
    resetBtn_ = new QPushButton("Reset", this);

    panel->addWidget(speedLabel_);
    panel->addWidget(speedSlider_);
    panel->addSpacing(10);
    panel->addWidget(angleLabel_);
    panel->addWidget(angleSlider_);
    panel->addSpacing(20);
    panel->addWidget(startBtn_);
    panel->addWidget(pauseBtn_);
    panel->addWidget(resetBtn_);
    panel->addStretch(1);

    // 右侧：热力图
    view_ = new HeatMapWidget(this);
    root->addWidget(view_, 1);

    controller_.setup(view_);
    wireUi();

    setWindowTitle("Plume Simulation (Advection + Upwind)");
    resize(900, 650);
}

MainWindow::~MainWindow() = default;

void MainWindow::wireUi()
{
    connect(speedSlider_, &QSlider::valueChanged, this, [this](int v){
        double speed = v / 100.0; // 0..1
        speedLabel_->setText(QString("Speed: %1").arg(speed, 0, 'f', 2));
        double angle = angleSlider_->value();
        controller_.setWind(speed, angle);
    });

    connect(angleSlider_, &QSlider::valueChanged, this, [this](int a){
        angleLabel_->setText(QString("Angle: %1").arg(a));
        double speed = speedSlider_->value() / 100.0;
        controller_.setWind(speed, (double)a);
    });

    connect(startBtn_, &QPushButton::clicked, this, [this](){
        controller_.start();
    });

    connect(pauseBtn_, &QPushButton::clicked, this, [this](){
        controller_.pause();
    });

    connect(resetBtn_, &QPushButton::clicked, this, [this](){
        controller_.reset();
    });
}
