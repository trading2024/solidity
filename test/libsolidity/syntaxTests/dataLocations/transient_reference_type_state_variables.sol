struct S {
	uint x;
	address a;
}

contract C {
	uint[] transient x;
	S transient s;
	mapping(uint => uint) transient y;
}
// ====
// stopAfter: analysis
// ----
// UnimplementedFeatureError: Transient data location is only supported for value types.