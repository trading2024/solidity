contract C {
    uint x = 0;
    uint y = 42;
    error E(uint);
    function f(bool c) public returns (uint256, uint256, uint256) {
        uint z = x;
        if (y == 42) {
            x = 21;
        } else {
            require(c, E(y));
        }
        y /= 2;
        return (x,y,z);
    }
}
// ----
// f(bool): 1 -> 0x15, 0x15, 0
// f(bool): 0 -> FAILURE, hex"002ff067", hex"0000000000000000000000000000000000000000000000000000000000000015"
