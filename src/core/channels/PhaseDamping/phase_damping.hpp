#ifndef PHASE_DAMPING
#define PHASE_DAMPING

#include "../../kraus_channel.hpp"

class PhaseDamping : public KrausChannel {
    public:
        explicit PhaseDamping(double g);
        std::vector<Eigen::MatrixXcd> getKrausOps() const override;
    private:
        double _gamma;
};

#endif