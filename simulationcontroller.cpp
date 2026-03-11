#include "simulationcontroller.h"
#include <algorithm>
#include <cmath>
static constexpr double PI = 3.14159265358979323846;

SimulationController::SimulationController(QObject* parent) : QObject(parent)
{
    connect(&timer_, &QTimer::timeout, this, &SimulationController::tick);
}

void SimulationController::setup(HeatMapWidget* view)
{
    view_ = view;

    // 网格与物理尺寸（用 1x1 归一化平面）
    smoke_.initialize(220, 220, 1.0, 1.0);
    solver_.initialize(smoke_.nx(), smoke_.ny(), smoke_.dx(), smoke_.dy());

    // 默认烟囱
    std::vector<Chimney> cs;
    Chimney c1; c1.x0 = 0.75; c1.y0 = 0.5; c1.sigmaX=0.04; c1.sigmaY=0.04; c1.amplitude=1.0;
    cs.push_back(c1);
    smoke_.setChimneys(cs);

    // 默认风
    wind_.set(0.35, 180.0);

    reset();
}

void SimulationController::start()
{
    if (running_) return;
    running_ = true;
    timer_.start(16); // ~60FPS
}

void SimulationController::pause()
{
    running_ = false;
    timer_.stop();
}

void SimulationController::reset()
{
    smoke_.reset();
    for (const auto& c : smoke_.chimneys()) smoke_.addGaussian(c);
    renderFrame();
}

void SimulationController::setWind(double speed, double angleDeg)
{
    wind_.set(speed, angleDeg);
    renderFrame();
}

double SimulationController::computeDtCFL() const
{
    const double u = wind_.u();
    double w = wind_.w() + 0.08 * std::sin(2.0 * M_PI * 0.2 * tSim_);


    const double dx = smoke_.dx();
    const double dy = smoke_.dy();

    // dt advection (CFL)
    const double denom = (std::abs(u)/dx) + (std::abs(w)/dy);
    double dt_adv = (denom < 1e-12) ? 0.01 : 0.8 / denom;

    // dt diffusion (stabilité explicite 2D)
    // dt <= 0.25 * min(dx^2, dy^2) / kappa
    double dt_diff = 1e9;
    if (kappa_ > 0.0) {
        dt_diff = 0.25 * std::min(dx*dx, dy*dy) / kappa_;
    }

    return std::min(dt_adv, dt_diff);
}


void SimulationController::renderFrame()
{
    if (!view_) return;

    // 修改点 1：把 autoScale 设为 false
    // 这样 0.5 的浓度永远是中间色，而不会因为全场最大只有 0.1 就变成红色
    QImage img = renderer_.render(smoke_.field(), smoke_.nx(), smoke_.ny(), false);

    view_->setImage(img);

    // 修改点 2：传递绝对范围给界面画刻度 (通常是 0.0 到 1.0)
    view_->setColorScale(renderer_.lastMin(), renderer_.lastMax());

    view_->setChimneys(smoke_.chimneys());
    view_->setWind(wind_.u(), wind_.w());
}

void SimulationController::tick()
{
    const double dt = computeDtCFL();
    double u  = wind_.u();
    double w0 = wind_.w();
    double w  = w0 + wAmp_ * std::sin(2.0 * PI * wFreq_ * tSim_);

    // émission continue
    smoke_.emitContinuous(dt);

    for (int k = 0; k < stepsPerTick_; ++k) {

        // 1) advection (vent)
        solver_.stepUpwind(smoke_.field(),
                           smoke_.nx(), smoke_.ny(),
                           smoke_.dx(), smoke_.dy(),
                           wind_.u(), wind_.w(),
                           dt);

        // 2) diffusion (turbulence)
        solver_.stepDiffusion(smoke_.field(),
                              smoke_.nx(), smoke_.ny(),
                              smoke_.dx(), smoke_.dy(),
                              kappa_,
                              dt);
    }

    tSim_ += dt;


    renderFrame();
}

