package main

func main() {
	a := 5
	b := 3

	equal := a == b
	notEqual := a != b
	less := a < b
	greater := a > b
	lessEqual := a <= b
	greaterEqual := a >= b

	flag := (a > 0) && (b > 0)
	alt := (a < 0) || (b > 0)
	neg := !equal

	_ = equal
	_ = notEqual
	_ = less
	_ = greater
	_ = lessEqual
	_ = greaterEqual
	_ = flag
	_ = alt
	_ = neg
}
