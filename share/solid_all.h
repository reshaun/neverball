#ifndef SOLID_ALL_H
#define SOLID_ALL_H

#include "solid_vary.h"

void sol_body_p(float p[3], const struct s_vary *, int pi, float t);
void sol_body_v(float v[3], const struct s_vary *, int pi, float t, float dt);
void sol_body_e(float e[3], const struct s_vary *, const struct v_body *bp,
                float dt);
void sol_body_w(float w[3], const struct s_vary *, const struct v_body *bp);

void sol_rotate(float e[3][3], const float w[3], float dt);

void sol_pendulum(struct v_ball *up,
                  const float a[3],
                  const float g[3], float dt);

void sol_swch_step(struct s_vary *, float dt, int ms);
void sol_body_step(struct s_vary *, float dt, int ms);
void sol_ball_step(struct s_vary *, float dt);

enum
{
    JUMP_OUTSIDE = 0,
    JUMP_TRIGGER,
    JUMP_INSIDE
};

enum
{
    SWCH_OUTSIDE = 0,
    SWCH_TRIGGER,
    SWCH_INSIDE
};

int            sol_item_test(struct s_vary *, float *p, float item_r);
struct b_goal *sol_goal_test(struct s_vary *, float *p, int ui);
int            sol_jump_test(struct s_vary *, float *p, int ui);
int            sol_swch_test(struct s_vary *, int ui);

#endif
