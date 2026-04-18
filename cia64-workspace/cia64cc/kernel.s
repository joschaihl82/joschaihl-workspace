  .bss
  .data
  .align 1
.L.data.2:
  .ascii "main\0"
  .align 1
.L.data.1:
  .ascii "delay_loop\0"
  .align 1
.L.data.0:
  .ascii "hsv_to_argb\0"
  .text
  .align 1
  .type hsv_to_argb, @function
hsv_to_argb:
  ADDI x2, x2, -8
  SD x8, 0(x2)
  ADDI x8, x2, 0
  ADDI x2, x2, -160
  SD x9, -8(x8)
  SD x18, -16(x8)
  SD x19, -24(x8)
  SD x20, -32(x8)
  SD x21, -40(x8)
  SD x22, -48(x8)
  SD x23, -56(x8)
  SD x24, -64(x8)
  SD x25, -72(x8)
  SD x26, -80(x8)
  SD x27, -88(x8)
  FSD f8, -96(x8)
  FSD f9, -104(x8)
  FSD f10, -112(x8)
  FSD f11, -120(x8)
  FSD f12, -128(x8)
  FSD f13, -136(x8)
  FSD f14, -144(x8)
  FSD f15, -152(x8)
  FSD f16, -160(x8)
  FSD f17, -168(x8)
  FSD f18, -176(x8)
  FSD f19, -184(x8)
  FSW f2, -148(x8)
  FSW f1, -152(x8)
  FSW f0, -156(x8)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -144
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -140
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -136
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 13 */
  BEQ x10, x0, .Lelse_17767
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -144
  FSW f0, 0(x5)
  J .Lend_9158
.Lelse_17767:
  /* gen_expr: unhandled node kind 13 */
  BEQ x10, x0, .Lelse_39017
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -156
  FSW f0, 0(x5)
  J .Lend_18547
.Lelse_39017:
.Lend_18547:
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -132
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -128
  SW x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -124
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -120
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -116
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -112
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 28 */
.Lend_9158:
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -108
  SB x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -107
  SB x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -106
  SB x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -105
  SB x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  /* return int in x10 */
  J .L.return.hsv_to_argb
.L.return.hsv_to_argb:
  LD x9, -8(x8)
  LD x18, -16(x8)
  LD x19, -24(x8)
  LD x20, -32(x8)
  LD x21, -40(x8)
  LD x22, -48(x8)
  LD x23, -56(x8)
  LD x24, -64(x8)
  LD x25, -72(x8)
  LD x26, -80(x8)
  LD x27, -88(x8)
  FLD f8, -96(x8)
  FLD f9, -104(x8)
  FLD f10, -112(x8)
  FLD f11, -120(x8)
  FLD f12, -128(x8)
  FLD f13, -136(x8)
  FLD f14, -144(x8)
  FLD f15, -152(x8)
  FLD f16, -160(x8)
  FLD f17, -168(x8)
  FLD f18, -176(x8)
  FLD f19, -184(x8)
  ADDI x2, x8, 0
  LD x8, 0(x2)
  ADDI x2, x2, 8
  RET
  .align 1
  .type delay_loop, @function
delay_loop:
  ADDI x2, x2, -8
  SD x8, 0(x2)
  ADDI x8, x2, 0
  ADDI x2, x2, -128
  SD x9, -8(x8)
  SD x18, -16(x8)
  SD x19, -24(x8)
  SD x20, -32(x8)
  SD x21, -40(x8)
  SD x22, -48(x8)
  SD x23, -56(x8)
  SD x24, -64(x8)
  SD x25, -72(x8)
  SD x26, -80(x8)
  SD x27, -88(x8)
  FSD f8, -96(x8)
  FSD f9, -104(x8)
  FSD f10, -112(x8)
  FSD f11, -120(x8)
  FSD f12, -128(x8)
  FSD f13, -136(x8)
  FSD f14, -144(x8)
  FSD f15, -152(x8)
  FSD f16, -160(x8)
  FSD f17, -168(x8)
  FSD f18, -176(x8)
  FSD f19, -184(x8)
  SW x17, -116(x8)
.Lloop_56401:
  /* gen_expr: unhandled node kind 12 */
  BEQ x10, x0, .Lend_23807
  J .Lloop_56401
.Lend_23807:
.L.return.delay_loop:
  LD x9, -8(x8)
  LD x18, -16(x8)
  LD x19, -24(x8)
  LD x20, -32(x8)
  LD x21, -40(x8)
  LD x22, -48(x8)
  LD x23, -56(x8)
  LD x24, -64(x8)
  LD x25, -72(x8)
  LD x26, -80(x8)
  LD x27, -88(x8)
  FLD f8, -96(x8)
  FLD f9, -104(x8)
  FLD f10, -112(x8)
  FLD f11, -120(x8)
  FLD f12, -128(x8)
  FLD f13, -136(x8)
  FLD f14, -144(x8)
  FLD f15, -152(x8)
  FLD f16, -160(x8)
  FLD f17, -168(x8)
  FLD f18, -176(x8)
  FLD f19, -184(x8)
  ADDI x2, x8, 0
  LD x8, 0(x2)
  ADDI x2, x2, 8
  RET
  .align 1
  .globl main
  .type main, @function
main:
  ADDI x2, x2, -8
  SD x8, 0(x2)
  ADDI x8, x2, 0
  ADDI x2, x2, -224
  SD x9, -8(x8)
  SD x18, -16(x8)
  SD x19, -24(x8)
  SD x20, -32(x8)
  SD x21, -40(x8)
  SD x22, -48(x8)
  SD x23, -56(x8)
  SD x24, -64(x8)
  SD x25, -72(x8)
  SD x26, -80(x8)
  SD x27, -88(x8)
  FSD f8, -96(x8)
  FSD f9, -104(x8)
  FSD f10, -112(x8)
  FSD f11, -120(x8)
  FSD f12, -128(x8)
  FSD f13, -136(x8)
  FSD f14, -144(x8)
  FSD f15, -152(x8)
  FSD f16, -160(x8)
  FSD f17, -168(x8)
  FSD f18, -176(x8)
  FSD f19, -184(x8)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -216
  SD x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -204
  SW x10, 0(x5)
.Lloop_37962:
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -192
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -188
  SW x10, 0(x5)
.Lloop_7977:
  /* gen_expr: unhandled node kind 12 */
  BEQ x10, x0, .Lend_31949
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -176
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -172
  SW x10, 0(x5)
.Lloop_22714:
  /* gen_expr: unhandled node kind 12 */
  BEQ x10, x0, .Lend_55211
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -156
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 16 */
  /* gen_expr: unhandled node kind 13 */
  BEQ x10, x0, .Lelse_16882
  /* gen_expr: unhandled node kind 16 */
  J .Lend_7931
.Lelse_16882:
.Lend_7931:
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -136
  FSW f0, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  ADDI x5, x8, -132
  SW x10, 0(x5)
  /* gen_expr: unhandled node kind 41 */
  /* gen_expr: unhandled node kind 41 */
  ADDI x12, x10, 0
  /* gen_expr: unhandled node kind 41 */
  ADDI x13, x10, 0
  ADD x10, x12, x13
  ADDI x5, x10, 0
  SW x10, 0(x5)
  /* gen_expr: unhandled node kind 16 */
  J .Lloop_22714
.Lend_55211:
  /* gen_expr: unhandled node kind 16 */
  J .Lloop_7977
.Lend_31949:
  /* gen_expr: unhandled node kind 16 */
  /* gen_expr: unhandled node kind 16 */
  J .Lloop_37962
.Lend_22764:
  /* gen_expr: unhandled node kind 41 */
  /* return int in x10 */
  J .L.return.main
  ADDI x10, x0, 0
.L.return.main:
  LD x9, -8(x8)
  LD x18, -16(x8)
  LD x19, -24(x8)
  LD x20, -32(x8)
  LD x21, -40(x8)
  LD x22, -48(x8)
  LD x23, -56(x8)
  LD x24, -64(x8)
  LD x25, -72(x8)
  LD x26, -80(x8)
  LD x27, -88(x8)
  FLD f8, -96(x8)
  FLD f9, -104(x8)
  FLD f10, -112(x8)
  FLD f11, -120(x8)
  FLD f12, -128(x8)
  FLD f13, -136(x8)
  FLD f14, -144(x8)
  FLD f15, -152(x8)
  FLD f16, -160(x8)
  FLD f17, -168(x8)
  FLD f18, -176(x8)
  FLD f19, -184(x8)
  ADDI x2, x8, 0
  LD x8, 0(x2)
  ADDI x2, x2, 8
  RET
