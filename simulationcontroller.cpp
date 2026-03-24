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

    smoke_.initialize(250, 250,1,1);
    solver_.initialize(smoke_.nx(), smoke_.ny(), smoke_.dx(), smoke_.dy());

    std::vector<Chimney> cs;
    Chimney c1;
    c1.x0 = 0.5;
    c1.y0 = 0.5;
    c1.sigmaX = 0.009;
    c1.sigmaY = 0.009;
    c1.amplitude = 100.0;
    cs.push_back(c1);
    smoke_.setChimneys(cs);

    winds_.clear();

    Wind w;
    w.set(0.35, 180.0);
    winds_.push_back(w);

    reset();
}

void SimulationController::start()
{
    if (running_) return;
    running_ = true;
    timer_.start(20);
}

void SimulationController::pause()
{
    running_ = false;
    timer_.stop();
}

void SimulationController::reset()
{
    smoke_.reset();
    for (const auto& c : smoke_.chimneys()) {
        smoke_.addGaussian(c);
    }
    renderFrame();
}

void SimulationController::clearWinds()
{
    winds_.clear();
    renderFrame();
}

void SimulationController::setWindCount(int count)
{
    if (count < 0) count = 0;
    winds_.resize(count);
    renderFrame();
}

void SimulationController::setWindAt(int index, double speed, double angleDeg)
{
    if (index < 0) return;
    if (index >= (int)winds_.size()) {
        winds_.resize(index + 1);
    }

    winds_[index].set(speed, angleDeg);
    renderFrame();
}

double SimulationController::resultantU() const
{
    double sum = 0.0;
    for (const auto& w : winds_) sum += w.u();
    return sum;
}

double SimulationController::resultantW() const
{
    double sum = 0.0;
    for (const auto& w : winds_) sum += w.w();
    return sum;
}

double SimulationController::resultantSpeed() const
{
    double u = resultantU();
    double w = resultantW();
    return std::sqrt(u*u + w*w);
}

double SimulationController::resultantAngleDeg() const
{
    double u = resultantU();
    double w = resultantW();
    double a = std::atan2(w, u) * 180.0 / PI;
    if (a < 0.0) a += 360.0;
    return a;
}

double SimulationController::computeDtCFL() const
{
    const double u = resultantU();
    const double w = resultantW() + wAmp_ * std::sin(2.0 * PI * wFreq_ * tSim_);

    const double dx = smoke_.dx();
    const double dy = smoke_.dy();

    const double denom = (std::abs(u)/dx) + (std::abs(w)/dy);
    double dt_adv = (denom < 1e-12) ? 0.01 : 0.8 / denom;

    double dt_diff = 1e9;
    if (kappa_ > 0.0) {
        dt_diff = 0.25 * std::min(dx*dx, dy*dy) / kappa_;
    }

    return std::min(dt_adv, dt_diff);
}

void SimulationController::renderFrame()
{
    if (!view_) return;

    QImage img = renderer_.render(smoke_.field(), smoke_.nx(), smoke_.ny(), false);

    view_->setImage(img);
    view_->setColorScale(renderer_.lastMin(), renderer_.lastMax());
    view_->setChimneys(smoke_.chimneys());
    view_->setWind(resultantU(),
                   resultantW() + wAmp_ * std::sin(2.0 * PI * wFreq_ * tSim_));
}

void SimulationController::tick()
{
    const double dt = computeDtCFL();

    const double u = resultantU();
    const double w = resultantW() + wAmp_ * std::sin(2.0 * PI * wFreq_ * tSim_);

    smoke_.emitContinuous(dt);

    for (int k = 0; k < stepsPerTick_; ++k) {
        solver_.stepUpwind(smoke_.field(),
                           smoke_.nx(), smoke_.ny(),
                           smoke_.dx(), smoke_.dy(),
                           u, w,
                           dt);

        solver_.stepDiffusion(smoke_.field(),
                              smoke_.nx(), smoke_.ny(),
                              smoke_.dx(), smoke_.dy(),
                              kappa_,
                              dt);
    }

    tSim_ += dt;
    renderFrame();
}
