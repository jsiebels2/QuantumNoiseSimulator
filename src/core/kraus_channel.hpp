#ifndef KRAUS_CHANNEL
#define KRAUS_CHANNEL

#include <Eigen/Dense>
#include "density_matrix.hpp"


class KrausChannel {
    public:
        virtual std::vector<Eigen::MatrixXcd> getKrausOps() const = 0;
        
        void apply(DensityMatrix& dm, const std::vector<int> qubitIndices) const {
            dm.applyKrausOperator(getKrausOps(), qubitIndices);
        };

        virtual ~KrausChannel() = default;
};

#endif