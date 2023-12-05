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

TEST_CASE("test slurp 2d") {
  auto buffer = cpt_slurp_path("./testdata/random_integers_grid");
  const auto buffer2d = cpt_slurp2d_buffer(buffer);
  REQUIRE(3 == buffer2d.num_rows);
  REQUIRE(18 == buffer2d.max_column_width);
  REQUIRE(7 == buffer2d.min_column_width);

  {
    REQUIRE(18 == buffer2d.rows[0].size);
    auto c = cpt_cursor_ref(buffer2d.rows[0]);
    REQUIRE(-382 == cpt_next_i32(&c));
    REQUIRE(-482 == cpt_next_i32(&c));
    REQUIRE(884 == cpt_next_i32(&c));
    REQUIRE(-961 == cpt_next_i32(&c));
    REQUIRE(cpt_cursor_eof(c));
  }
  {
    REQUIRE(15 == buffer2d.rows[1].size);
    auto c = cpt_cursor_ref(buffer2d.rows[1]);
    REQUIRE(862 == cpt_next_i32(&c));
    REQUIRE(267 == cpt_next_i32(&c));
    REQUIRE(-5 == cpt_next_i32(&c));
    REQUIRE(-742 == cpt_next_i32(&c));
    REQUIRE(cpt_cursor_eof(c));
  }
  {
    REQUIRE(7 == buffer2d.rows[2].size);
    auto c = cpt_cursor_ref(buffer2d.rows[2]);
    REQUIRE(267 == cpt_next_i32(&c));
    REQUIRE(862 == cpt_next_i32(&c));
    REQUIRE(cpt_cursor_eof(c));
  }
}

TEST_CASE("test read_i32") {
  for (int64_t i = std::numeric_limits<int32_t>::lowest();
       i < std::numeric_limits<int32_t>::max();
       i += std::numeric_limits<int32_t>::max() / 100000) {
    const auto ref = std::to_string(i);
    auto c = cpt_cursor_ref_cstring(ref.c_str());
    REQUIRE(i == cpt_read_i32(&c));
  }
};
