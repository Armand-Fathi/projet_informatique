#include "mainwindow.h"
#include "heatmapwidget.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(8);

    // =========================
    // Colonne gauche fixe
    // =========================
    leftPanelWidget_ = new QWidget(this);
    leftPanelWidget_->setMinimumWidth(280);
    leftPanelWidget_->setMaximumWidth(320);
    leftPanelWidget_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto* panel = new QVBoxLayout(leftPanelWidget_);
    panel->setContentsMargins(4, 4, 4, 4);
    panel->setSpacing(8);

    addWindBtn_ = new QPushButton("Ajouter un vent", leftPanelWidget_);
    resultantLabel_ = new QLabel("Vent resultant : speed=0.00 angle=0.0", leftPanelWidget_);
    resultantLabel_->setWordWrap(true);

    startBtn_ = new QPushButton("Start", leftPanelWidget_);
    pauseBtn_ = new QPushButton("Pause", leftPanelWidget_);
    resetBtn_ = new QPushButton("Reset", leftPanelWidget_);

    // =========================
    // Scroll area pour les vents
    // =========================
    windsContainer_ = new QWidget(leftPanelWidget_);
    windsLayout_ = new QVBoxLayout(windsContainer_);
    windsLayout_->setContentsMargins(0, 0, 0, 0);
    windsLayout_->setSpacing(10);
    windsLayout_->addStretch();

    windsScrollArea_ = new QScrollArea(leftPanelWidget_);
    windsScrollArea_->setWidgetResizable(true);
    windsScrollArea_->setFrameShape(QFrame::NoFrame);
    windsScrollArea_->setWidget(windsContainer_);
    windsScrollArea_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    panel->addWidget(addWindBtn_);
    panel->addWidget(windsScrollArea_, 1);
    panel->addWidget(resultantLabel_);
    panel->addWidget(startBtn_);
    panel->addWidget(pauseBtn_);
    panel->addWidget(resetBtn_);

    root->addWidget(leftPanelWidget_, 0);

    // =========================
    // Vue principale à droite
    // =========================
    view_ = new HeatMapWidget(this);
    view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    root->addWidget(view_, 1);

    controller_.setup(view_);

    addWindRow(0.35, 180.0);

    wireUi();
    syncControllerFromUi();
    updateResultantLabel();

    setWindowTitle("prototype-v-1");
    resize(1400, 800);
}

MainWindow::~MainWindow() = default;

void MainWindow::addWindRow(double speed, double angle)
{
    WindRow row;

    row.container = new QWidget(windsContainer_);
    auto* layout = new QVBoxLayout(row.container);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    row.title = new QLabel(row.container);

    row.speedLabel = new QLabel(QString("Speed: %1").arg(speed, 0, 'f', 2), row.container);
    row.speedSlider = new QSlider(Qt::Horizontal, row.container);
    row.speedSlider->setRange(0, 200);
    row.speedSlider->setValue((int)(speed * 100.0));

    row.angleLabel = new QLabel(QString("Angle: %1").arg(angle, 0, 'f', 1), row.container);
    row.angleSlider = new QSlider(Qt::Horizontal, row.container);
    row.angleSlider->setRange(0, 360);
    row.angleSlider->setValue((int)angle);

    row.removeBtn = new QPushButton("Supprimer", row.container);

    layout->addWidget(row.title);
    layout->addWidget(row.speedLabel);
    layout->addWidget(row.speedSlider);
    layout->addWidget(row.angleLabel);
    layout->addWidget(row.angleSlider);
    layout->addWidget(row.removeBtn);

    // Toujours insérer avant le stretch final
    windsLayout_->insertWidget(windsLayout_->count() - 1, row.container);
    windRows_.push_back(row);

    rebuildWindTitles();

    connect(row.speedSlider, &QSlider::valueChanged, this, [this, rowWidget = row.container](int v){
        for (auto& r : windRows_) {
            if (r.container == rowWidget) {
                double speed = v / 100.0;
                r.speedLabel->setText(QString("Speed: %1").arg(speed, 0, 'f', 2));
                syncControllerFromUi();
                updateResultantLabel();
                return;
            }
        }
    });

    connect(row.angleSlider, &QSlider::valueChanged, this, [this, rowWidget = row.container](int a){
        for (auto& r : windRows_) {
            if (r.container == rowWidget) {
                r.angleLabel->setText(QString("Angle: %1").arg(a));
                syncControllerFromUi();
                updateResultantLabel();
                return;
            }
        }
    });

    connect(row.removeBtn, &QPushButton::clicked, this, [this, rowWidget = row.container](){
        removeWindRow(rowWidget);
    });
}

void MainWindow::removeWindRow(QWidget* rowWidget)
{
    for (auto it = windRows_.begin(); it != windRows_.end(); ++it) {
        if (it->container == rowWidget) {
            windsLayout_->removeWidget(it->container);
            delete it->container;
            windRows_.erase(it);
            break;
        }
    }

    rebuildWindTitles();
    syncControllerFromUi();
    updateResultantLabel();
}

void MainWindow::rebuildWindTitles()
{
    for (int i = 0; i < (int)windRows_.size(); ++i) {
        windRows_[i].title->setText(QString("Vent %1").arg(i + 1));
    }
}

void MainWindow::syncControllerFromUi()
{
    controller_.clearWinds();
    controller_.setWindCount((int)windRows_.size());

    for (int i = 0; i < (int)windRows_.size(); ++i) {
        double speed = windRows_[i].speedSlider->value() / 100.0;
        double angle = windRows_[i].angleSlider->value();
        controller_.setWindAt(i, speed, angle);
    }
}

void MainWindow::updateResultantLabel()
{
    resultantLabel_->setText(
        QString("Vent resultant : speed=%1 angle=%2")
            .arg(controller_.resultantSpeed(), 0, 'f', 2)
            .arg(controller_.resultantAngleDeg(), 0, 'f', 1)
        );
}

void MainWindow::wireUi()
{
    connect(addWindBtn_, &QPushButton::clicked, this, [this](){
        addWindRow(0.35, 180.0);
        syncControllerFromUi();
        updateResultantLabel();
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
