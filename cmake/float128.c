// Tests whether __float128 exists.

typedef __float128 float128_t;

int main() {
  float128_t f = 0;
  (void)f;
}
