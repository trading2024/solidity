contract C {
    uint256 counter = 0;

    error CustomError(uint256);

    function g() internal returns (bool) {
        counter++;
        return false;
    }

    function f() external {
        require(g(), CustomError(counter));
    }
}

// ----
// f() -> FAILURE, hex"110b3655", hex"0000000000000000000000000000000000000000000000000000000000000001"
