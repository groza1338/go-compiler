package main

import "fmt"

func main() {
	var a = [2][3]int{{1, 2, 3}, {4, 5, 6}}
	var b = [2][2][2]float64{{{1.1, 2.2}, {3.3, 4.4}}, {{5.5, 6.6}, {7.7, 8.8}}}

	fmt.Println(a)
	fmt.Println(a[1][2])
	fmt.Println(b)
	fmt.Println(b[1][0][1])

	a[0][1] = 9
	fmt.Println(a)
}
