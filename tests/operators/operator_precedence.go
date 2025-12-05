package main

func main() {
	a := 2 + 3 * 4
	b := (2 + 3) * 4
	c := 10 - 2*3 + 1
	d := 10 - (2*3 + 1)
	e := 8 / 2 / 2
	f := 8 / (2 / 2)
	g := 2 + 3 * (4 + 5)
	h := (2 + 3) * (4 + 5)

	logical := (a > b) && (c < d || e == f)
	mixed := a + b*2 - c/2
}
