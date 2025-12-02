package main

var flagTrue = true
var flagFalse = false

func main() {
	if flagTrue && !flagFalse {
		_ = 1
	}
}
