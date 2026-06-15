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

    EXPECT_TRUE(hm.remove(2));
    EXPECT_FALSE(hm.remove(2));

    EXPECT_EQ(hm.get(2), nullptr);
    EXPECT_EQ(hm.occupied(), 0);
    EXPECT_EQ(counter, 0);
}

TEST(RawHashMapTest, Insert) {
    auto hm = RawHashMap<size_t, size_t>{};

    EXPECT_TRUE(hm.insert(22, 22));
    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(22), 22);

    EXPECT_FALSE(hm.insert(22, 22222));
    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(22), 22);
}

TEST(RawHashMapTest, OperatorSqBracket) {
    auto hm = RawHashMap<size_t, size_t>{};

    hm[2] = 2;

    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(2), 2);

    hm[2] += 2;

    EXPECT_EQ(hm.occupied(), 1);
    EXPECT_EQ(*hm.get(2), 4);
}

TEST(RawHashMapTest, Collision) {
    auto hm = RawHashMap<size_t, size_t>{4};

    hm[0]    = 0;
    hm[128]  = 1;
    hm[256]  = 2;
    hm[512]  = 3;
    hm[1024] = 4;

    EXPECT_EQ(hm.occupied(), 5);

    EXPECT_EQ(*hm.get(0), 0);
    EXPECT_EQ(*hm.get(128), 1);
    EXPECT_EQ(*hm.get(256), 2);
    EXPECT_EQ(*hm.get(512), 3);
    EXPECT_EQ(*hm.get(1024), 4);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
