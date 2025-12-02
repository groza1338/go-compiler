package main

var decimal = 1_000_000
var octal = 0o755
var hex = 0xFFEE
var floatSimple = 12.5
var floatExp = 6.022e23
var floatLeading = .25

func main() {
	_ = decimal + octal + hex
	_ = floatSimple + floatExp + floatLeading
}
