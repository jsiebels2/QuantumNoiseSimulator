#ifndef BIT_PHASE_FLIPS
#define BIT_PHASE_FLIPS

#include "../../kraus_channel.hpp"

class BitPhaseFlips : public KrausChannel {
    public:
        explicit BitPhaseFlips(double p);
        std::vector<Eigen::MatrixXcd> getKrausOps() const override;
    private:
        double _p;
};

#endif