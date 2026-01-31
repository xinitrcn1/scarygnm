# scarygnm

the most scary amdgpu-gcn emitter

oooo so scary

did you ever randomly walked by and in the side of the road you found a random AMD GCN assembly excerpt that goes like:

```rs
V_CNDMASK_B32 v4, v0, v1,       EXEC
V_CNDMASK_B32 v5, v0, abs(v2),  EXEC
V_CNDMASK_B32 v6, v0, -v2,      EXEC
V_CNDMASK_B32 v7, v0, -abs(v3), EXEC
```

if so, just do it in c++ like

```c++
scarygnm::Context c;
c.V_CNDMASK_B32(c.v4, c.v0, c.v1);
c.V_CNDMASK_B32(c.v5, c.v0, c.abs(c.v2));
c.V_CNDMASK_B32(c.v6, c.v0, -c.v2);
c.V_CNDMASK_B32(c.v7, c.v0, -c.abs(c.v3));
```

wow it's so easy im so scared.


now look at this beauty:

```rs
S_MOV_B32 M0, s0
V_INTERP_P1_F32 v3, v0, attr0.x
V_INTERP_P2_F32 v3, v1, attr0.x
V_INTERP_P1_F32 v2, v0, attr0.y
V_INTERP_P2_F32 v2, v1, attr0.y
V_CVT_PKRTZ_F16_F32 v2, v3, v2
V_INTERP_P1_F32 v3, v0, attr0.z
V_INTERP_P1_F32 v0, v0, attr0.w
V_INTERP_P2_F32 v3, v1, attr0.z
V_INTERP_P2_F32 v0, v1, attr0.w
V_CVT_PKRTZ_F16_F32 v0, v3, v0
```

wow so scary, worry not it gets scarier:

```c++
scarygnm::Context c;
c.S_MOV_B32(c.M0, c.s0);
c.V_INTERP_P1_F32(c.v3, c.v0, c.attr0.x);
c.V_INTERP_P1_F32(c.v3, c.v1, c.attr0.x);
c.V_INTERP_P1_F32(c.v2, c.v0, c.attr0.y);
c.V_INTERP_P1_F32(c.v2, c.v1, c.attr0.y);
c.V_CVT_PKRTZ_F16_F32(c.v2, c.v3, c.v2);
c.V_INTERP_P1_F32(c.v3, c.v0, c.attr0.z);
c.V_INTERP_P1_F32(c.v0, c.v0, c.attr0.w);
c.V_INTERP_P1_F32(c.v3, c.v1, c.attr0.z);
c.V_INTERP_P1_F32(c.v0, c.v1, c.attr0.w);
c.V_CVT_PKRTZ_F16_F32(c.v0, c.v3, c.v0);
```

indeed, scary has been achieved.
