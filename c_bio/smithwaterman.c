#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct scoring_matrix {
  int64_t x_dim;
  int64_t y_dim;
  int64_t *matrix; // TODO: int64 or smaller?
} scoring_matrix_t;

typedef struct sw_params {
  int64_t gap_score;
  int64_t mismatch_score;
  int64_t match_score;
} sw_params_t;

// TODO: inline
int64_t max2(int64_t a, int64_t b) { return a > b ? a : b; };

// TODO: inline
int64_t max4(int64_t a, int64_t b, int64_t c, int64_t d) {
  return max2(max2(a, b), max2(c, d));
}

// TODO: inline func?
// #define SM_IDX(sm, x, y) (((x) + (sm)->x_dim * (y)))

// TODO: why can't I use the above macro here?
#define SM_VAL(sm, x, y) ((sm)->matrix[((x) + (sm)->x_dim * (y))])

scoring_matrix_t *generate_scoring_matrix(char *seq_x, char *seq_y,
                                          sw_params_t *params) {

  scoring_matrix_t *sm = malloc(sizeof(scoring_matrix_t));
  sm->x_dim = (int64_t)strlen(seq_x) + 1;
  sm->y_dim = (int64_t)strlen(seq_y) + 1;
  sm->matrix = calloc(sm->x_dim * sm->y_dim, sizeof(int64_t));

  for (int64_t x = 1; x < sm->x_dim; x++) {
    for (int64_t y = 1; y < sm->y_dim; y++) {
      SM_VAL(sm, x, y) =
          max4(SM_VAL(sm, x, y - 1) + params->gap_score,
               SM_VAL(sm, x - 1, y) + params->gap_score,
               SM_VAL(sm, x - 1, y - 1) + (seq_x[x - 1] == seq_y[y - 1]
                                               ? params->match_score
                                               : params->mismatch_score),
               0);
    }
  }
  return sm;
}

void print_matrix(scoring_matrix_t *sm) {
  for (int64_t x = 0; x < sm->x_dim; x++) {
    for (int64_t y = 0; y < sm->y_dim; y++) {
      printf("%ld ", SM_VAL(sm, x, y));
    }
    printf("\n");
  }
}

sw_params_t *new_sw_params(int64_t gap_score, int64_t mismatch_score,
                           int64_t match_score) {
  sw_params_t *params = malloc(sizeof(sw_params_t));
  params->gap_score = gap_score;
  params->mismatch_score = mismatch_score;
  params->match_score = match_score;
  return params;
}

void test_scoring_matrix() {
  // TODO: note that this table is transposed from what is printed
  int64_t truth_matrix[100] = {
      0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0, 5, 5, 2, 5,  2,  5,  2,  0,  5,
      0, 2, 3, 10, 7,  4,  2,  10, 7,  4,  0, 0, 0, 8, 8,  5,  2,  7,  8,  5,
      0, 5, 5, 5,  13, 10, 10, 7,  5,  13, 0, 2, 3, 3, 10, 18, 15, 12, 12, 10,
      0, 0, 0, 1,  7,  15, 16, 13, 17, 14, 0, 0, 0, 0, 4,  12, 13, 14, 18, 15,
      0, 0, 0, 5,  2,  9,  10, 18, 15, 16, 0, 0, 0, 2, 3,  7,  7,  15, 23, 20,
  };

  scoring_matrix_t *truth_scoring_matrix =
      &(scoring_matrix_t){10, 10, truth_matrix};

  sw_params_t *params = new_sw_params(-3, -2, 5);
  scoring_matrix_t *scoring_matrix =
      generate_scoring_matrix("AATAGATGA", "ATTAGGGTG", params);

  assert(truth_scoring_matrix->x_dim == scoring_matrix->x_dim);
  assert(truth_scoring_matrix->y_dim == scoring_matrix->y_dim);
  for (int64_t x = 0; x < truth_scoring_matrix->x_dim; x++) {
    for (int64_t y = 0; y < truth_scoring_matrix->y_dim; y++) {
      assert(SM_VAL(truth_scoring_matrix, x, y) ==
             SM_VAL(scoring_matrix, x, y));
    }
  }
}

typedef struct {
  int64_t x;
  int64_t y;
} coord_t;

coord_t idxmax_last(scoring_matrix_t *sm) {
  int64_t max = 0;
  coord_t idxmax = {0, 0};
  for (int64_t x = 0; x < sm->x_dim; x++) {
    for (int64_t y = 0; y < sm->y_dim; y++) {
      int64_t val = SM_VAL(sm, x, y);
      if (val >= max) {
        max = val;
        idxmax.x = x;
        idxmax.y = y;
      }
    }
  }
  return idxmax;
}

void traceback(char *seq_x, char *seq_y, scoring_matrix_t *sm) {
  coord_t idxmax = idxmax_last(sm);
  int64_t x = idxmax.x;
  int64_t y = idxmax.y;

  int64_t diag_val = 0;
  int64_t up_val = 0;
  int64_t left_val = 0;

  printf("%lu %lu %lu %lu\n", x, y, sm->x_dim, sm->y_dim);
  for (int64_t _x = sm->x_dim; _x < x; _x--) {
    for (int64_t _y = sm->y_dim; _y < y; _y--) {
      printf("%lu %lu %lu %lu\n", x, y, sm->x_dim, sm->y_dim);
      printf("-%c %c\n", seq_x[_x], seq_y[_y]);
    }
  }
  do {
    diag_val = SM_VAL(sm, x - 1, y - 1);
    up_val = SM_VAL(sm, x, y - 1);
    left_val = SM_VAL(sm, x - 1, y);
    printf("%c %c\n", seq_x[x], seq_y[y]);

    // TODO: some sort of fun switch?
    if (diag_val >= up_val && diag_val >= left_val) {
      printf("%c %c\n", seq_x[x - 1], seq_y[y - 1]);
    } else if (up_val > diag_val && up_val >= left_val) {
      printf("%c %c\n", '-', seq_y[y - 1]);
    } else if (left_val > diag_val && left_val > up_val) {
      printf("%c %c\n", seq_x[x - 1], '-');
    }
    x--;
    y--;
  } while (diag_val != 0);
}

int main(void) {
  sw_params_t *params = new_sw_params(-3, -2, 5);

  scoring_matrix_t *scoring_matrix =
      generate_scoring_matrix("AATAGATGA", "ATTAGGGTG", params);
  print_matrix(scoring_matrix);

  test_scoring_matrix();
  coord_t idxmax = idxmax_last(scoring_matrix);
  printf("%lu %lu %lu\n", idxmax.x, idxmax.y,
         SM_VAL(scoring_matrix, idxmax.x, idxmax.y));
  traceback("AATAGATGA", "ATTAGGGTG", scoring_matrix);
}

// void test_matrix_iteration() {
//   scoring_matrix_t *sm = malloc(sizeof(scoring_matrix_t));
//   sm->x_dim = 4;
//   sm->y_dim = 4;
//   int64_t *matrix = calloc(sm->x_dim * sm->y_dim, sizeof(int64_t));
//   int64_t curr = 0;
//   for (int64_t i = 0; i < sm->x_dim; i++) {
//     for (int64_t j = 0; j < sm->y_dim; j++) {
//       matrix[SM_IDX(sm, i, j)] = curr;
//       curr++;
//     }
//   }
//   for (int64_t i = 0; i < sm->x_dim; i++) {
//     for (int64_t j = 0; j < sm->y_dim; j++) {
//       printf("%ld:%ld:%ld\t", i, j, matrix[SM_IDX(sm, i, j)]);
//     }
//     printf("\n");
//   }
// }
