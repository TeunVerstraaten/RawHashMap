#include "raw_hash_map.h"

#include <gtest/gtest.h>

TEST(RawHashMapTest, InsertAndRemove) {
    static size_t counter;
    struct DestructionCounted {
        size_t _counter = counter;
        DestructionCounted() {
            ++_counter;
        }

        DestructionCounted(const DestructionCounted& other) {
            ++_counter;
        }

        ~DestructionCounted() {
            --_counter;
        }
    };

    auto hm = RawHashMap<size_t, DestructionCounted>{};

    hm[2] = DestructionCounted();
    hm[2] = DestructionCounted();

    hm.remove(2);

    EXPECT_EQ(hm.get(2), nullptr);
    EXPECT_EQ(hm.occupied(), 0);
    EXPECT_EQ(counter, 0);
}

TEST(RawHashMapTest, BasicOperatorSqBracket) {
    auto hm = RawHashMap<size_t, size_t>{};

    hm[2] = 2;

    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(2), 2);

    hm[2] += 2;

    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(2), 4);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
