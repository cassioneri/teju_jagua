// Test whether __SIZEOF_FLOAT128__ is defined.
int main() {
  __uint128_t i = 0;
  (void)i;
  __float128 f = 0;
  (void)f;
}
