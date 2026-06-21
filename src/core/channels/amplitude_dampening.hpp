#ifndef AMPLITUDE_DAMPENING
#define AMPLITUDE_DAMPENING

#include "../kraus_channel.hpp"
#include "../density_matrix.hpp"
#include <Eigen/Dense>

class AmplitudeDampening : public KrausChannel {
    public:
        explicit AmplitudeDampening(double g);
        std::vector<Eigen::MatrixXcd> getKrausOps() const override;
    private:
        double _gamma;

};

#endif  