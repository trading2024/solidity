{
    let a := /* @debug.set {"assignment":"a"} */ mul(1, codesize()) /* @debug.set {} */
    let b := /* @debug.set {"assignment":"b"} */ mul(1, codesize()) /* @debug.set {} */
}
// ----
// step: commonSubexpressionEliminator
//
// {
//     let a := mul(1, codesize())
//     let b := a
// }
//
// Assembly:
//     /* "":58:68   */
//   codesize
//     /* "":55:56   */
//   0x01
//     /* "":51:69   */
//   mul
//     /* "":139:157   */
//   dup1
//     /* "":0:179   */
//   pop
//   pop
