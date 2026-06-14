#include "raw_hash_map.h"

#include <gtest/gtest.h>

struct DestructionCounted {
    std::shared_ptr<size_t> _ptr;
    DestructionCounted(std::shared_ptr<size_t>& ptr) : _ptr(ptr) {
        ++*_ptr;
    }

    DestructionCounted(const DestructionCounted& other) : _ptr(other._ptr) {
        ++*_ptr;
    }

    ~DestructionCounted() {
        --*_ptr;
    }
};

TEST(RawHashMapTest, InsertAndRemove) {
    auto count = std::make_shared<size_t>(0);
    auto hm    = RawHashMap<size_t, DestructionCounted>{};

    hm[2] = DestructionCounted(count);
    hm[2] = DestructionCounted(count);

    hm.remove(2);

    EXPECT_EQ(hm.get(2), nullptr);
    EXPECT_EQ(hm.occupied(), 0);
    EXPECT_EQ(*count, 0);
}

TEST(RawHashMapTest, BasicOperatorSqBracket) {
    auto hm = RawHashMap<size_t, size_t>{};

    hm[2] = 2;
    hm.show();

    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(2), 2);

    // hm[2] += 2;

    // EXPECT_EQ(hm.occupied(), 1);
    // EXPECT_EQ(*hm.get(2), 4);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
