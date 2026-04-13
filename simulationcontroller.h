#pragma once
#include <QObject>
#include <QTimer>
#include <vector>
#include "wind.h"
#include "smoke.h"
#include "advectionsolver.h"
#include "heatmaprenderer.h"
#include "heatmapwidget.h"

class SimulationController : public QObject {
    Q_OBJECT

public:
    explicit SimulationController(QObject* parent=nullptr);

    void setup(HeatMapWidget* view);

    void start();
    void pause();
    void reset();

    void setWaves(double amp, double freq) { wAmp_ = amp; wFreq_ = freq; }
    void setDiffusion(double kappa) { kappa_ = kappa; }
    void setStepsPerTick(int s) { stepsPerTick_ = s; }

    void clearWinds();
    void setWindCount(int count);
    void setWindAt(int index, double speed, double angleDeg);

    double resultantU() const;
    double resultantW() const;
    double resultantSpeed() const;
    double resultantAngleDeg() const;

private:
    double tSim_ = 0.0;
    double wAmp_ = 0.0;
    double wFreq_ = 0.0;
    double kappa_ = 0.0005;

    HeatMapWidget* view_ = nullptr;

    std::vector<Wind> winds_;
    Smoke smoke_;
    AdvectionSolver solver_;
    HeatMapRenderer renderer_;

    QTimer timer_;
    bool running_ = false;
    int stepsPerTick_ = 3;

    double computeDtCFL() const;
    void renderFrame();

private slots:
    void tick();
};
