package main

import "fmt"

func main() {
	var a int = 20
	var b int = 6
	fmt.Println("INT start:", a, b)
	a += b
	fmt.Println("a += b:", a)
	a -= 2
	fmt.Println("a -= 2:", a)
	a *= 3
	fmt.Println("a *= 3:", a)
	a /= 4
	fmt.Println("a /= 4:", a)
	a %= 5
	fmt.Println("a %= 5:", a)

	fmt.Println()

	var x float64 = 10.5
	var y float64 = 2.0
	fmt.Println("FLOAT start:", x, y)
	x += y
	fmt.Println("x += y:", x)
	x -= 1.25
	fmt.Println("x -= 1.25:", x)
	x *= 3.0
	fmt.Println("x *= 3.0:", x)
	x /= 4.0
	fmt.Println("x /= 4.0:", x)

	fmt.Println()

	var m int = 7
	var n float64 = 2.5
	fmt.Println("MIX start:", m, n)
	m += 2.0
	fmt.Println("m += 2.0:", m)
	m *= 3.0
	fmt.Println("m *= 3.0:", m)
	n += 3
	fmt.Println("n += 3:", n)
	n *= 2
	fmt.Println("n *= 2:", n)
}
