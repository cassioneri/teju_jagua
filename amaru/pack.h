#ifndef AMARU_AMARU_PACK_H_
#define AMARU_AMARU_PACK_H_

#define amaru_pack2(a1, a0) (                     \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 2)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 2))   \
  )

#define amaru_pack4(a3, a2, a1, a0) (             \
    ((amaru_u1_t) (a3) << (3 * amaru_size / 4)) | \
    ((amaru_u1_t) (a2) << (2 * amaru_size / 4)) | \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 4)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 4))   \
  )

#endif // AMARU_AMARU_PACK_H_
