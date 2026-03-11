#pragma once
#include <QObject>
#include <QTimer>
#include "wind.h"
#include "smoke.h"
#include "advectionsolver.h"
#include "heatmaprenderer.h"
#include "heatmapwidget.h"

class SimulationController : public QObject {
    Q_OBJECT

public:
    void setWaves(double amp, double freq) { wAmp_ = amp; wFreq_ = freq; }
    void setDiffusion(double kappa) { kappa_ = kappa; }
    explicit SimulationController(QObject* parent=nullptr);

    void setup(HeatMapWidget* view);

    void start();
    void pause();
    void reset();

    void setWind(double speed, double angleDeg);
    void setStepsPerTick(int s) { stepsPerTick_ = s; }

private:
    double tSim_ = 0.0;     // temps simulation (s)
    double wAmp_ = 0.08;    // amplitude
    double wFreq_ = 0.2;    // fréquence (Hz)

    double kappa_ = 0.0005; // >>> AJOUT : diffusion (à régler)
    HeatMapWidget* view_ = nullptr;

    Wind wind_;
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
