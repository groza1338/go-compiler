package main

import "fmt"

func main() {
	var a [3]int
	a[0] = 1
	a[1] = 2
	a[2] = 3
	fmt.Print(a)
	fmt.Print("\n")

	var b = [...]float64{5.4, 95.4, 0.9, 44.4}
	fmt.Print(b)
	fmt.Print("\n")
	tmp := b[0]
	b[0] = 0 // TODO Почему-то вылетает ошибка, что нельзя присвоить значение типа int во float64, хотя так можно (запусти в компиляторе на сайте)
	b[1] = tmp
	b[2] = b[1]
	b[3] = b[2]
	fmt.Print(b)
}