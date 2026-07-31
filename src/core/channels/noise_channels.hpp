#ifndef NOISE_CHANNELS

#include "core/channels/AmplitudeDamping/amplitude_damping.hpp"
#include "core/channels/BitPhaseFlips/bit_phase_flips.hpp"
#include "core/channels/DepolarizingNoise/depolarizing_noise.hpp"
#include "core/channels/PhaseDamping/phase_damping.hpp"
#include "core/kraus_channel.hpp"

class NoiseChannel {
    public:
        auto getKrausOps() const {
            return kc_->getKrausOps();
        }
        
        static NoiseChannel AmplitudeDampingChannel(double gamma) {
            return NoiseChannel(std::make_unique<AmplitudeDamping>(gamma));
        }
        static NoiseChannel BitPhaseFlipChannel(double p) {
            return NoiseChannel(std::make_unique<BitPhaseFlips>(p));
        }
        static NoiseChannel DepolarizingNoiseChannel(double p) {
            return NoiseChannel(std::make_unique<DepolarizingNoise>(p));
        }
        static NoiseChannel PhaseDampingChannel(double gamma) {
            return NoiseChannel(std::make_unique<PhaseDamping>(gamma));
        }
    private:
        std::unique_ptr<KrausChannel> kc_;
        explicit NoiseChannel(std::unique_ptr<KrausChannel> kc) : kc_(std::move(kc)) {};
};

#endif