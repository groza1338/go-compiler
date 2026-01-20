package main

import "fmt"

func main() {
	var a, b int = 10, 3
	fmt.Println("INT a =", a, "b =", b)
	fmt.Println("a + b =", a+b)
	fmt.Println("a - b =", a-b)
	fmt.Println("a * b =", a*b)
	fmt.Println("a / b =", a/b) // целочисленное деление
	fmt.Println("a % b =", a%b)
	fmt.Println("a == b:", a == b)
	fmt.Println("a != b:", a != b)
	fmt.Println("a > b:", a > b)
	fmt.Println("a < b:", a < b)
	fmt.Println("a >= b:", a >= b)
	fmt.Println("a <= b:", a <= b)

	fmt.Println()

	// FLOAT
	var x, y float64 = 10.5, 3.2
	fmt.Println("FLOAT x =", x, "y =", y)
	fmt.Println("x + y =", x+y)
	fmt.Println("x - y =", x-y)
	fmt.Println("x * y =", x*y)
	fmt.Println("x / y =", x/y)
	fmt.Println("x == y:", x == y)
	fmt.Println("x != y:", x != y)
	fmt.Println("x > y:", x > y)
	fmt.Println("x < y:", x < y)
	fmt.Println("x >= y:", x >= y)
	fmt.Println("x <= y:", x <= y)

	fmt.Println()

	fmt.Println("10 + 10.5 =", 10+10.5)
	fmt.Println("10 - 10.5 =", 10-10.5)
	fmt.Println("10 * 10.5 =", 10*10.5)
	fmt.Println("10 / 10.5 =", 10/10.5)

	fmt.Println("10.5 + 3 =", 10.5+3)
	fmt.Println("10.5 - 3 =", 10.5-3)
	fmt.Println("10.5 * 3 =", 10.5*3)
	fmt.Println("10.5 / 3 =", 10.5/3)

	fmt.Println()

	// сравнения int и float (тоже ок, потому что это константы)
	fmt.Println("10 == 10.0:", 10 == 10.0)
	fmt.Println("10 != 10.0:", 10 != 10.0)
	fmt.Println("10 > 10.5:", 10 > 10.5)
	fmt.Println("10 < 10.5:", 10 < 10.5)
	fmt.Println("10 >= 10.5:", 10 >= 10.5)
	fmt.Println("10 <= 10.5:", 10 <= 10.5)
}
