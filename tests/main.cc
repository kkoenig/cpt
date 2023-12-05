#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include "cpt.h"
#include "doctest.h"

TEST_CASE("test slurp") {
  const auto buffer =
      cpt_slurp_path("./testdata/random_unsigned_integers_tiny");

  const std::string expected = "1 4294967295 71574 55361 87579 0";
  REQUIRE(buffer.size == expected.size());
  REQUIRE(buffer.data == expected);
  {
    auto s = cpt_cursor_ref(buffer);
    REQUIRE(1 == cpt_next_u32(&s));
    REQUIRE(4294967295 == cpt_next_u32(&s));
    REQUIRE(71574 == cpt_next_u32(&s));
    REQUIRE(55361 == cpt_next_u32(&s));
    REQUIRE(87579 == cpt_next_u32(&s));
    REQUIRE(0 == cpt_next_u32(&s));
  }
}

TEST_CASE("test read_i32") {
  for (int64_t i = std::numeric_limits<int32_t>::lowest();
       i < std::numeric_limits<int32_t>::max();
       i += std::numeric_limits<int32_t>::max() / 100000) {
    const auto ref = std::to_string(i);
    auto c = cpt_cursor_ref_string(ref.c_str());
    REQUIRE(i == cpt_read_i32(&c));
  }
};
