#ifndef DEPOLARIZING_NOISE
#define DEPOLARIZING_NOISE

#include "../../kraus_channel.hpp"

class DepolarizingNoise : public KrausChannel {
    public:
        explicit DepolarizingNoise(double p);
        std::vector<Eigen::MatrixXcd> getKrausOps() const override;
    private:
        double _p;
};

#endif