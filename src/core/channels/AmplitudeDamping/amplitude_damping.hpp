#ifndef AMPLITUDE_DAMPING
#define AMPLITUDE_DAMPING

#include "../../kraus_channel.hpp"
#include <Eigen/Dense>

class AmplitudeDamping : public KrausChannel {
    public:
        explicit AmplitudeDamping(double g);
        std::vector<Eigen::MatrixXcd> getKrausOps() const override;
    private:
        double _gamma;

};

#endif  