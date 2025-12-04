package main

func main() {
	var a [2]int
	var b [2]int

	a[0] = 1
	a[1] = a[0] + 2
	b = a

	_ = b[1]
}
