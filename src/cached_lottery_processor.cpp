#include "i_lottery_processor.h"
#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cstdint>

namespace hungarian {

/**
 * @brief Simple cached processor that indexes all combinations (2..5) using a compact key.
 *
 * The cache maps an encoded sorted combination -> count of players having that combination.
 */
class CachedLotteryProcessor : public hungarian::ILotteryProcessor {
public:
    void preComputeWinners(const std::vector<std::vector<uint8_t>>& players) override {
        m_cache.clear();
        for (const auto& player_numbers : players) {
            std::array<uint8_t,5> player_array{};
            for (size_t idx=0; idx<5; ++idx) player_array[idx] = player_numbers[idx];
            for (int combinationSize=2; combinationSize<=5; ++combinationSize) {
                auto keys = generateCombinationKeys(player_array, combinationSize);
                for (uint64_t key : keys) {
                    ++m_cache[{combinationSize,key}];
                }
            }
        }
    }

    Winners computeWinners(const std::array<uint8_t,5>& pick) const override {
        // S_k are sums over players of C(m_i,k) where m_i is player's match count
        uint64_t S5 = 0, S4 = 0, S3 = 0, S2 = 0;

        // five
        {
            std::vector<uint8_t> pick_sorted(pick.begin(), pick.end());
            std::sort(pick_sorted.begin(), pick_sorted.end());
            uint64_t key = encodeKey(pick_sorted);
            auto it = m_cache.find({5,key});
            if (it != m_cache.end()) S5 = it->second;
        }

        // four (5 combos)
        for (int i=0;i<5;++i) {
            std::vector<uint8_t> comb;
            for (int j=0;j<5;++j) if (j!=i) comb.push_back(pick[j]);
            std::sort(comb.begin(), comb.end());
            uint64_t key = encodeKey(comb);
            auto it = m_cache.find({4,key});
            if (it != m_cache.end()) S4 += it->second;
        }

        // three (10 combos)
        for (int i=0;i<5;++i)
            for (int j=i+1;j<5;++j) {
                std::vector<uint8_t> comb;
                for (int k=0;k<5;++k) if (k!=i && k!=j) comb.push_back(pick[k]);
                std::sort(comb.begin(), comb.end());
                uint64_t key = encodeKey(comb);
                auto it = m_cache.find({3,key});
                if (it != m_cache.end()) S3 += it->second;
            }

        // two (10 combos)
        for (int i=0;i<5;++i)
            for (int j=i+1;j<5;++j) {
                std::vector<uint8_t> comb = { pick[i], pick[j] };
                std::sort(comb.begin(), comb.end());
                uint64_t key = encodeKey(comb);
                auto it = m_cache.find({2,key});
                if (it != m_cache.end()) S2 += it->second;
            }

        // Now solve for exact counts N_m from S_k using binomial relations.
        Winners w{};
        uint64_t N5 = S5;
        uint64_t N4 = 0;
        if (S4 >= 5 * N5) N4 = S4 - 5 * N5;
        uint64_t N3 = 0;
        if (S3 >= 10 * N5 + 4 * N4) N3 = S3 - 10 * N5 - 4 * N4;
        uint64_t N2 = 0;
        if (S2 >= 10 * N5 + 6 * N4 + 3 * N3) N2 = S2 - 10 * N5 - 6 * N4 - 3 * N3;

        w.five = static_cast<uint32_t>(N5);
        w.four = static_cast<uint32_t>(N4);
        w.three = static_cast<uint32_t>(N3);
        w.two = static_cast<uint32_t>(N2);

        return w;
    }

private:
  // composite key: pair<k, encoded>
  struct KeyPair {
    int k;
    uint64_t key;
    bool operator==(KeyPair const& o) const noexcept { return k==o.k && key==o.key; }
  };

  struct KeyHash { size_t operator()(KeyPair const& p) const noexcept { return std::hash<uint64_t>()((uint64_t(p.k)<<56) ^ p.key); } };

        static uint64_t encodeKey(const std::vector<uint8_t>& comb) {
                // base-91 encoding (numbers 1..90), stable for up to 5 elements
                uint64_t encoded_key = 0;
                for (uint8_t value : comb) {
                        encoded_key = encoded_key * 91 + static_cast<uint64_t>(value);
                }
                return encoded_key;
        }

  /**
   * @internal
   * @brief Generate encoded keys for all sorted combinations of `combinationSize`.
   *
   * Produces an encoded key for each sorted combination of the provided
   * 5-element array. The encoding is stable and suitable for lookup in the
   * internal cache.
   *
   * @param numbers_array Array of 5 player numbers.
   * @param combinationSize Combination size (2..5).
   * @return Vector of encoded keys.
   */
  std::vector<uint64_t> generateCombinationKeys(const std::array<uint8_t,5>& numbers_array, int combinationSize) const;

  std::unordered_map<KeyPair,uint32_t,KeyHash> m_cache;
};

// Helper: generate sorted-encoded keys for combinations of size k from a 5-array
/**
 * @internal
 * @brief Generate sorted-encoded keys for combinations of size k from a 5-array.
 *
 * This static helper is used by the class-level `generateCombinationKeys()` to
 * produce the sequence of encoded keys for a given player's 5 numbers and a
 * requested combination size.
 *
 * @param numbers_array Input array with 5 numbers.
 * @param combinationSize Size of combinations to generate (2..5).
 * @return Vector of encoded keys for each sorted combination.
 */
static std::vector<uint64_t> generateCombinationKeysStatic(const std::array<uint8_t,5>& numbers_array, int combinationSize) {
    std::vector<uint64_t> keys;
    int n = 5;
    std::vector<int> bitmask(n);
    for (int idx=0; idx<combinationSize; ++idx) bitmask[idx]=1;
    for (int idx=combinationSize; idx<n; ++idx) bitmask[idx]=0;
    do {
        std::vector<uint8_t> comb;
        for (int idx=0; idx<n; ++idx) if (bitmask[idx]) comb.push_back(numbers_array[idx]);
        std::sort(comb.begin(), comb.end());
        // encode
        uint64_t encoded=0;
        for (uint8_t val : comb) encoded = encoded*91 + static_cast<uint64_t>(val);
        keys.push_back(encoded);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    return keys;
}

/**
 * @internal
 * @brief Wrapper that calls the static generator and returns encoded keys.
 */
std::vector<uint64_t> CachedLotteryProcessor::generateCombinationKeys(const std::array<uint8_t,5>& numbers_array, int combinationSize) const {
    return generateCombinationKeysStatic(numbers_array, combinationSize);
}

/**
 * @brief Factory to create a cached processor instance.
 *
 * Exposed for tests and application wiring.
 */
std::unique_ptr<ILotteryProcessor> createCachedProcessor() {
    return std::unique_ptr<ILotteryProcessor>(new CachedLotteryProcessor());
}

} // namespace hungarian
